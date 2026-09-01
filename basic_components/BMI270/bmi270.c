#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bmi270.h"
#include "bmi270_config.h"   /* 8KB 配置固件(来自 Bosch), BMI270 必须加载才有数据 */

static const char *TAG = "bmi270";

#define BMI270_ADDR      0x68      /* SDO=GND → 0x68 */
#define I2C_PORT         I2C_NUM_1 /* 与 AXP2101/RTC/触摸共用总线(axp2101_i2c_init 已初始化) */

/* ---------- 寄存器(BMI270 实际地址, 2026-08-31 查证: ACC_CONF=0x40, ACC_RANGE=0x41) ---------- */
#define REG_CHIP_ID      0x00
#define REG_INTERNAL_STATUS 0x21
#define REG_ACC_X_LSB    0x0C      /* 6 字节: X_L X_H Y_L Y_H Z_L Z_H */
#define REG_ACC_CONF     0x40      /* bits[6:4]=BWP, bits[3:0]=ODR */
#define REG_ACC_RANGE    0x41      /* bits[1:0]=量程: 0=±2g 1=±4g 2=±8g 3=±16g */
#define REG_INIT_CTRL    0x59      /* 配置加载控制 */
#define REG_INIT_ADDR_0  0x5B      /* 配置字地址 bits[3:0] */
#define REG_INIT_ADDR_1  0x5C      /* 配置字地址 bits[11:4] */
#define REG_INIT_DATA    0x5E      /* 配置数据(连续写) */
#define REG_PWR_CONF     0x7C
#define REG_PWR_CTRL     0x7D
#define REG_CMD          0x7E
#define REG_FEAT_PAGE    0x2F      /* 特征块页选择 */
#define REG_FEATURES     0x30      /* 特征块数据(按页访问, 块大小 16B) */
#define FEAT_BLOCK_SIZE  16

/* 步数计数器(feature 地址来自官方 bmi270.h 2026-09-01 核实):
 * 使能位: 页6 块内 start_addr=0x02(BMI270_STEP_CNT_4_STRT_ADDR) + 1 字节,
 *         bit3=step detector(0x08) bit4=step counter(0x10) bit5=step activity(0x20)
 * 输出  : 页0 块内 start_addr=0x00(BMI270_STEP_CNT_OUT_STRT_ADDR) 起 4 字节小端
 *         (offset 0x04 是 BMI270_STEP_ACT_OUT_STRT_ADDR=step activity, 2字节) */
#define STEP_CNT_CFG_PAGE    6
#define STEP_CNT_CFG_ADDR    0x02   /* BMI270_STEP_CNT_4_STRT_ADDR(页内偏移) */
#define STEP_CNT_EN_OFFSET   0x01   /* BMI2_STEP_COUNT_FEAT_EN_OFFSET */
#define STEP_CNT_EN_MASK     0x38   /* bit3 detector | bit4 counter | bit5 activity */
#define STEP_CNT_OUT_PAGE    0
#define STEP_CNT_OUT_ADDR    0x00   /* BMI270_STEP_CNT_OUT_STRT_ADDR(页内偏移) */

#define BMI270_CHIP_ID   0x24
#define CMD_SOFT_RESET   0xB6
#define CONFIG_CHUNK     32        /* 分块上传大小(Bosch 参考 32 字节) */

/* ---------- I2C 读写(复用 axp2101_i2c_init 的总线) ---------- */
static esp_err_t bmi270_write(uint8_t reg, uint8_t val)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BMI270_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, val, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t bmi270_read(uint8_t reg, uint8_t *buf, size_t n)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BMI270_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BMI270_ADDR << 1) | I2C_MASTER_READ, true);
    if (n > 1) {
        i2c_master_read(cmd, buf, n - 1, I2C_MASTER_ACK);   /* 连续读(地址自增) */
    }
    i2c_master_read_byte(cmd, buf + n - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}

/* 连续写 n 字节到指定寄存器(burst write) */
static esp_err_t bmi270_write_buf(uint8_t reg, const uint8_t *buf, size_t n)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BMI270_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write(cmd, (uint8_t *)buf, n, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}

/* ---------- 加载 8KB 配置固件(BMI270 必须, 否则传感器不输出数据) ----------
 * 流程(规格书 4.4): 关高级省电 → INIT_CTRL=0 → 分块上传(每块先设 INIT_ADDR
 * 16位字地址, 再 burst 写 INIT_DATA) → INIT_CTRL=1 → 检查 init_ok */
static esp_err_t bmi270_load_config(void)
{
    /* 1. 关高级省电(配置加载前必须) */
    bmi270_write(REG_PWR_CONF, 0x00);
    vTaskDelay(pdMS_TO_TICKS(1));            /* 450us */

    /* 2. 准备配置加载 */
    bmi270_write(REG_INIT_CTRL, 0x00);

    /* 3. 分块上传 */
    const uint8_t *cfg = bmi270_config_file;
    for (size_t i = 0; i < BMI270_CONFIG_LEN; i += CONFIG_CHUNK) {
        size_t n = (BMI270_CONFIG_LEN - i) < CONFIG_CHUNK ? (BMI270_CONFIG_LEN - i) : CONFIG_CHUNK;
        uint16_t word_addr = i / 2;           /* 配置按 16 位字寻址 */
        uint8_t addr[2] = { (uint8_t)(word_addr & 0x0F), (uint8_t)((word_addr >> 4) & 0xFF) };
        if (bmi270_write_buf(REG_INIT_ADDR_0, addr, 2) != ESP_OK) {
            ESP_LOGE(TAG, "config addr write failed @%u", (unsigned)i);
            return ESP_FAIL;
        }
        if (bmi270_write_buf(REG_INIT_DATA, cfg + i, n) != ESP_OK) {
            ESP_LOGE(TAG, "config data write failed @%u", (unsigned)i);
            return ESP_FAIL;
        }
    }

    /* 4. 完成配置加载 */
    bmi270_write(REG_INIT_CTRL, 0x01);

    /* 5. 轮询 init_ok(INTERNAL_STATUS bit0), 芯片处理 8KB 配置需要时间 */
    uint8_t st = 0;
    for (int i = 0; i < 100; i++) {           /* 最多等 100ms */
        bmi270_read(REG_INTERNAL_STATUS, &st, 1);
        if (st & 0x01) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    ESP_LOGI(TAG, "config loaded, internal_status=0x%02X (bit0=init_ok)", st);
    return (st & 0x01) ? ESP_OK : ESP_FAIL;
}

/* ---------- 初始化: 软复位 → 校验 ID → 加载配置 → 加速度计上电配置 ---------- */
esp_err_t bmi270_init(void)
{
    /* 软复位 */
    bmi270_write(REG_CMD, CMD_SOFT_RESET);
    vTaskDelay(pdMS_TO_TICKS(20));

    /* 校验芯片 ID, 确认 I2C 通信 + 地址正确 */
    uint8_t id = 0;
    if (bmi270_read(REG_CHIP_ID, &id, 1) != ESP_OK) {
        ESP_LOGE(TAG, "I2C read chip id failed");
        return ESP_ERR_NOT_FOUND;
    }
    if (id != BMI270_CHIP_ID) {
        ESP_LOGE(TAG, "chip id mismatch: 0x%02X (expect 0x24)", id);
        return ESP_ERR_NOT_FOUND;
    }
    ESP_LOGI(TAG, "chip id OK (0x%02X)", id);

    /* 加载 8KB 配置固件(必须, 否则传感器不出数据) */
    if (bmi270_load_config() != ESP_OK) {
        ESP_LOGE(TAG, "config load failed");
        return ESP_FAIL;
    }

    /* 使能加速度计: PWR_CTRL bit2=acc_en(0x04); 之前误用 0x01 那是 aux_en */
    bmi270_write(REG_PWR_CTRL, 0x04);        /* acc_en */

    /* 加速度计配置: BWP=OSR2(1) + ODR=25Hz(5) → 0x25; 量程 ±2g */
    bmi270_write(REG_ACC_CONF, 0x25);
    bmi270_write(REG_ACC_RANGE, 0x00);

    vTaskDelay(pdMS_TO_TICKS(20));           /* 等上电稳定 */
    ESP_LOGI(TAG, "accel ready (25Hz, +-2g)");
    return ESP_OK;
}

/* ---------- 读加速度原始值(LSB first) ---------- */
esp_err_t bmi270_read_accel_raw(bmi270_accel_raw_t *acc)
{
    uint8_t buf[6];
    if (!acc) return ESP_ERR_INVALID_ARG;
    if (bmi270_read(REG_ACC_X_LSB, buf, 6) != ESP_OK) {
        return ESP_FAIL;
    }
    acc->x = (int16_t)((buf[1] << 8) | buf[0]);
    acc->y = (int16_t)((buf[3] << 8) | buf[2]);
    acc->z = (int16_t)((buf[5] << 8) | buf[4]);
    return ESP_OK;
}

/* ---------- 内置步数计数器(feature, 配置固件加载后才可用) ---------- */

/* 调试: 选页后读特征块(最多 16 字节), 用于验证使能/输出位置 */
esp_err_t bmi270_read_feat_block(uint8_t page, uint8_t *buf, uint8_t n)
{
    if (!buf || n == 0 || n > FEAT_BLOCK_SIZE) {
        return ESP_ERR_INVALID_ARG;
    }
    if (bmi270_write(REG_FEAT_PAGE, page) != ESP_OK) {
        return ESP_FAIL;
    }
    return bmi270_read(REG_FEATURES, buf, n);
}

esp_err_t bmi270_step_enable(void)
{
    uint8_t block[FEAT_BLOCK_SIZE];

    /* 选页6, 读步数计数器配置块 */
    if (bmi270_write(REG_FEAT_PAGE, STEP_CNT_CFG_PAGE) != ESP_OK) {
        return ESP_FAIL;
    }
    if (bmi270_read(REG_FEATURES, block, FEAT_BLOCK_SIZE) != ESP_OK) {
        return ESP_FAIL;
    }

    /* 置使能位(块内 start_addr+1 字节的 bit4) */
    block[STEP_CNT_CFG_ADDR + STEP_CNT_EN_OFFSET] |= STEP_CNT_EN_MASK;

    /* 写回同一页 */
    if (bmi270_write_buf(REG_FEATURES, block, FEAT_BLOCK_SIZE) != ESP_OK) {
        return ESP_FAIL;
    }

    /* 回读校验 */
    uint8_t check[FEAT_BLOCK_SIZE];
    if (bmi270_read(REG_FEATURES, check, FEAT_BLOCK_SIZE) == ESP_OK) {
        ESP_LOGI(TAG, "step counter enable: cfg byte[%u]=0x%02X (bit4=%u)",
                 STEP_CNT_CFG_ADDR + STEP_CNT_EN_OFFSET,
                 check[STEP_CNT_CFG_ADDR + STEP_CNT_EN_OFFSET],
                 (check[STEP_CNT_CFG_ADDR + STEP_CNT_EN_OFFSET] >> 4) & 1);
    }
    return ESP_OK;
}

esp_err_t bmi270_read_step_count(uint32_t *count)
{
    uint8_t block[FEAT_BLOCK_SIZE];
    if (!count) {
        return ESP_ERR_INVALID_ARG;
    }
    if (bmi270_write(REG_FEAT_PAGE, STEP_CNT_OUT_PAGE) != ESP_OK) {
        return ESP_FAIL;
    }
    if (bmi270_read(REG_FEATURES, block, FEAT_BLOCK_SIZE) != ESP_OK) {
        return ESP_FAIL;
    }
    *count = (uint32_t)block[STEP_CNT_OUT_ADDR]
           | ((uint32_t)block[STEP_CNT_OUT_ADDR + 1] << 8)
           | ((uint32_t)block[STEP_CNT_OUT_ADDR + 2] << 16)
           | ((uint32_t)block[STEP_CNT_OUT_ADDR + 3] << 24);
    return ESP_OK;
}

int bmi270_read_chip_id(void)
{
    uint8_t id = 0;
    if (bmi270_read(REG_CHIP_ID, &id, 1) != ESP_OK) {
        return -1;
    }
    return id;
}

esp_err_t bmi270_read_reg(uint8_t reg, uint8_t *val)
{
    return bmi270_read(reg, val, 1);
}

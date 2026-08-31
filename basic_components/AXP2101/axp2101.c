#include "driver/i2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

#define AXP2101_ADDR   0x34          // 7位 I2C 地址
#define I2C_PORT       I2C_NUM_1     // 用哪个 I2C 外设

#define scl_gpio         (GPIO_NUM_5)
#define sda_gpio         (GPIO_NUM_4)

/* ========== 初始化 I2C（app_main 开头调用一次） ========== */
void axp2101_i2c_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_gpio,
        .scl_io_num = scl_gpio,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
}
/* ========== 写一个寄存器 ========== */
static esp_err_t axp2101_write(uint8_t reg, uint8_t val)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AXP2101_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, val, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}
/* ========== 读一个寄存器 ========== */
static esp_err_t axp2101_read(uint8_t reg, uint8_t *val)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AXP2101_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AXP2101_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, val, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}
/* ========== 读-改-写（保护其他位） ========== */
static esp_err_t axp2101_update(uint8_t reg, uint8_t mask, uint8_t val)
{
    uint8_t r = 0;
    esp_err_t ret = axp2101_read(reg, &r);
    if (ret != ESP_OK) return ret;
    r = (r & ~mask) | (val & mask);
    return axp2101_write(reg, r);
}
/* ========== ALDO1 设电压：0.5~3.5V，寄存器 0x92 ==========
 * 编码 = (电压 - 0.5) / 0.1
 * 3.3V → (3.3-0.5)/0.1 = 28 = 0x1C
 * 0.5V起，0.1V/step，31档 */
esp_err_t axp2101_set_aldo1_voltage(float volt)
{
    if (volt < 0.5f || volt > 3.5f) return ESP_ERR_INVALID_ARG;
    uint8_t code = (uint8_t)((volt - 0.5f) / 0.1f);
    return axp2101_write(0x92, code);
}
/* ========== ALDO1 使能：0x90 bit0 ========== */
esp_err_t axp2101_enable_aldo1(bool enable)
{
    return axp2101_update(0x90, 0x01, enable ? 0x01 : 0x00);
}
/* ========== DCDC1 设电压：1.5~3.4V，寄存器 0x82 ==========
 * 编码 = (电压 - 1.5) / 0.1
 * 3.3V → (3.3-1.5)/0.1 = 18 = 0x12
 * 1.5V起，0.1V/step，20档 */
esp_err_t axp2101_set_dcdc1_voltage(float volt)
{
    if (volt < 1.5f || volt > 3.4f) return ESP_ERR_INVALID_ARG;
    uint8_t code = (uint8_t)((volt - 1.5f) / 0.1f);
    return axp2101_write(0x82, code);
}
/* ========== DCDC1 使能：0x80 bit0 ========== */
esp_err_t axp2101_enable_dcdc1(bool enable)
{
    return axp2101_update(0x80, 0x01, enable ? 0x01 : 0x00);
}

/* ========== 电池/充电状态 ==========
 * REG A4H: 电池百分比(E-Gauge 燃料计, 0-100)
 * REG 00H: bit3=电池在位, bit5=VBUS 在位
 * REG 01H: bits6:5=电池电流方向(01=充电) */
int axp2101_get_battery_percent(void)
{
    uint8_t v = 0;
    if (axp2101_read(0xA4, &v) != ESP_OK) {
        return -1;
    }
    return v;
}

bool axp2101_get_battery_present(void)
{
    uint8_t v = 0;
    if (axp2101_read(0x00, &v) != ESP_OK) {
        return false;
    }
    return (v & 0x08) != 0;   /* bit3 */
}

bool axp2101_get_vbus_connected(void)
{
    uint8_t v = 0;
    if (axp2101_read(0x00, &v) != ESP_OK) {
        return false;
    }
    return (v & 0x20) != 0;   /* bit5 */
}

bool axp2101_get_charging(void)
{
    uint8_t v = 0;
    if (axp2101_read(0x01, &v) != ESP_OK) {
        return false;
    }
    return ((v >> 5) & 0x03) == 0x01;   /* 电流方向 01 = 充电 */
}

int axp2101_get_battery_voltage(void)
{
    uint8_t hi = 0, lo = 0;
    if (axp2101_read(0x34, &hi) != ESP_OK || axp2101_read(0x35, &lo) != ESP_OK) {
        return -1;
    }
    /* 14bit, 1mV/LSB: hi 取低 6 位作为高字节 */
    return ((hi & 0x3F) << 8) | lo;
}
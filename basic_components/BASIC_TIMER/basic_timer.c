#include "basic_timer.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "pcf8563";
static i2c_port_t s_port = I2C_NUM_1;   /* 默认，用 init 覆盖 */
/* ===== BCD 转换 ===== */
static uint8_t pcf8563_dec_to_bcd(int v) { return ((v / 10) << 4) | (v % 10); }
static int pcf8563_bcd_to_dec(uint8_t b)  { return ((b >> 4) * 10) + (b & 0x0F); }
/* ===== 写寄存器 ===== */
static esp_err_t pcf8563_reg_write(uint8_t reg, uint8_t val)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCF8563_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, val, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(s_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}
/* ===== 连续读 ===== */
static esp_err_t pcf8563_reg_read(uint8_t reg, uint8_t *buf, size_t n)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCF8563_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCF8563_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    for (size_t i = 0; i < n; i++) {
        i2c_master_read_byte(cmd, &buf[i], (i == n - 1) ? I2C_MASTER_NACK : I2C_MASTER_ACK);
    }
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(s_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}
/* ===== 初始化：共用总线版（只记端口号，不装驱动） ===== */
esp_err_t pcf8563_init(i2c_port_t port)
{
    s_port = port;   /* 只记住用哪条总线，总线已由 AXP2101 等初始化好 */
    /* 可选：探测一下确认芯片在线（读 0x00） */
    uint8_t reg = 0;
    esp_err_t ret = pcf8563_reg_read(0x00, &reg, 1);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "pcf8563 probe failed (check wiring/address)");
        return ret;
    }
    ESP_LOGI(TAG, "pcf8563 found, ctrl1=0x%02X", reg);
    return ESP_OK;
}
/* ===== 写时间 ===== */
esp_err_t pcf8563_set_time(const pcf8563_time_t *t)
{
    if (!t) return ESP_ERR_INVALID_ARG;
    /* ① 停钟 */
    uint8_t ctrl1 = 0;
    pcf8563_reg_read(0x00, &ctrl1, 1);
    ctrl1 |= 0x20;
    pcf8563_reg_write(0x00, ctrl1);
    /* ② 写 0x02~0x08 */
    uint8_t buf[7];
    buf[0] = pcf8563_dec_to_bcd(t->sec);
    buf[1] = pcf8563_dec_to_bcd(t->min);
    buf[2] = pcf8563_dec_to_bcd(t->hour);
    buf[3] = pcf8563_dec_to_bcd(t->day);
    buf[4] = t->weekday & 0x07;
    buf[5] = pcf8563_dec_to_bcd(t->month);
    buf[6] = pcf8563_dec_to_bcd(t->year % 100);   /* 寄存器只有 2 位年份 */
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCF8563_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x02, true);
    for (int i = 0; i < 7; i++) i2c_master_write_byte(cmd, buf[i], true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(s_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) return ret;
    /* ③ 恢复走时 */
    ctrl1 &= ~0x20;
    pcf8563_reg_write(0x00, ctrl1);
    ESP_LOGI(TAG, "pcf8563_set_time: %02d:%02d:%02d", t->hour, t->min, t->sec);
    return ESP_OK;
}
/* ===== 读时间 ===== */
esp_err_t pcf8563_get_time(pcf8563_time_t *t)
{
    if (!t) return ESP_ERR_INVALID_ARG;
    uint8_t buf[7];
    esp_err_t ret = pcf8563_reg_read(0x02, buf, 7);
    if (ret != ESP_OK) return ret;
    t->sec     = pcf8563_bcd_to_dec(buf[0] & 0x7F);
    t->min     = pcf8563_bcd_to_dec(buf[1] & 0x7F);
    t->hour    = pcf8563_bcd_to_dec(buf[2] & 0x3F);
    t->day     = pcf8563_bcd_to_dec(buf[3] & 0x3F);
    t->weekday = buf[4] & 0x07;
    t->month   = pcf8563_bcd_to_dec(buf[5] & 0x1F);
    t->year    = 2000 + pcf8563_bcd_to_dec(buf[6]);   /* 按世纪 2000 还原 4 位年份 */
    return ESP_OK;
}
/* ===== VL 位 ===== */
esp_err_t pcf8563_is_valid(bool *valid)
{
    uint8_t buf[1];
    esp_err_t ret = pcf8563_reg_read(0x02, buf, 1);
    if (ret != ESP_OK) return ret;
    *valid = (buf[0] & 0x80) ? false : true;
    return ESP_OK;
}
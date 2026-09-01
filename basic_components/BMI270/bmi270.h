#ifndef __BMI270_H_
#define __BMI270_H_

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

/* ============================================================
 * BMI270 6 轴 IMU 驱动 (basic_components/BMI270)
 * I2C: 与 AXP2101/RTC/触摸共用 I2C_NUM_1(axp2101_i2c_init 已初始化)。
 * 纯寄存器读写, 不碰 LVGL。
 * ============================================================ */

/* 加速度原始值(LSB, 16bit 有符号) */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} bmi270_accel_raw_t;

/* 初始化: 软复位 → 校验芯片ID(0x24) → 使能加速度计(25Hz, ±2g)。
 * 返回 ESP_OK = 芯片通信正常。 */
esp_err_t bmi270_init(void);

/* 读加速度原始值(LSB); ±2g 量程下灵敏度 16384 LSB/g */
esp_err_t bmi270_read_accel_raw(bmi270_accel_raw_t *acc);

/* 使能内置步数计数器(BMI270 feature, 须已加载配置固件 bmi270_init)。
 * 之后芯片内部按加速度计 ODR 自动计步, 断电/软复位清零。 */
esp_err_t bmi270_step_enable(void);

/* 读芯片内置步数计数(4 字节小端)。注意: 这是芯片内累计值(断电清零),
 * 不是 NVS 持久化值——持久化由服务层负责。 */
esp_err_t bmi270_read_step_count(uint32_t *count);

/* 调试: 读特征块(page 指定页, n 字节, 最多 16)。用于验证使能/输出位置 */
esp_err_t bmi270_read_feat_block(uint8_t page, uint8_t *buf, uint8_t n);

/* 调试: 读芯片 ID, 失败返回 -1 */
int bmi270_read_chip_id(void);
/* 调试: 读任意单字节寄存器 */
esp_err_t bmi270_read_reg(uint8_t reg, uint8_t *val);

#endif /* __BMI270_H_ */

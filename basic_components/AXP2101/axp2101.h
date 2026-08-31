#ifndef __AXP2101_H_
#define __AXP2101_H_

#include <stdbool.h>
#include "esp_err.h"

void axp2101_i2c_init(void);
esp_err_t axp2101_set_aldo1_voltage(float volt);
esp_err_t axp2101_enable_aldo1(bool enable);
esp_err_t axp2101_set_dcdc1_voltage(float volt);
esp_err_t axp2101_enable_dcdc1(bool enable);

/* ========== 电池/充电状态(读寄存器, 不碰 LVGL) ========== */
/* 电池百分比 0-100(REG A4H, E-Gauge 燃料计); 电池不在位或读失败返回 -1 */
int  axp2101_get_battery_percent(void);
/* 电池是否在位(REG 00H bit3) */
bool axp2101_get_battery_present(void);
/* USB/VBUS 是否插入(REG 00H bit5) */
bool axp2101_get_vbus_connected(void);
/* 是否正在充电(REG 01H 电流方向 == 01) */
bool axp2101_get_charging(void);
/* 电池电压 mV(REG 34H/35H, 14bit, 1mV/LSB); 读失败返回 -1 */
int  axp2101_get_battery_voltage(void);

#endif


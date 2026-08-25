#ifndef __AXP2101_H_
#define __AXP2101_H_

void axp2101_i2c_init(void);
esp_err_t axp2101_set_aldo1_voltage(float volt);
esp_err_t axp2101_enable_aldo1(bool enable);
esp_err_t axp2101_set_dcdc1_voltage(float volt);
esp_err_t axp2101_enable_dcdc1(bool enable);

#endif


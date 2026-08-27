#ifndef __basic_timer_H_
#define __basic_timer_H_

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/i2c.h"
#define PCF8563_I2C_ADDR   0x51

/* PCF8563 寄存器只有 2 位年份（0-99），本驱动按世纪 2000 处理：
 *  set_time 传 4 位年份（如 2026），驱动内部转成 2 位写入；
 *  get_time 返回 4 位年份（2000 + 寄存器值）。
 *  weekday 直接透传 0-6（建议用 tm_wday 的约定：0=周日）。
 *  min 是分钟缩写，与 month 无关。
 */
typedef struct {
    int year;      /* 4 位年份，如 2026 */
    int month;     /* 1-12 */
    int day;       /* 1-31 */
    int weekday;   /* 0-6，透传 */
    int hour;      /* 0-23 */
    int min;       /* 0-59 */
    int sec;       /* 0-59 */
} pcf8563_time_t;
/* 共用总线版：只记端口号，不装驱动（总线已由其他芯片初始化，如 AXP2101） */
esp_err_t pcf8563_init(i2c_port_t port);
esp_err_t pcf8563_set_time(const pcf8563_time_t *t);
esp_err_t pcf8563_get_time(pcf8563_time_t *t);
esp_err_t pcf8563_is_valid(bool *valid);

#endif
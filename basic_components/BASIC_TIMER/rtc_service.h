#ifndef __RTC_SERVICE_H_
#define __RTC_SERVICE_H_
#include "esp_err.h"
#include "basic_timer.h"

/* RTC 时间服务（中间层，与 LVGL/UI 完全解耦）：
 *  - 向下封装 PCF8563 驱动（basic_timer）与 SNTP/NTP 同步（myntc）；
 *  - 向上只暴露"当前时间 / 是否有效 / NTP→RTC 同步"三个能力，不关心怎么显示；
 *  - 与 AXP2101 共用 I2C_NUM_1 总线（总线由 AXP2101 初始化，这里只记端口、不装驱动）。
 *
 * 使用顺序：rtc_service_init() 一次 → rtc_service_sync_from_ntp()（拿真实时间写 RTC）
 * → 之后任意时刻 rtc_service_get_time() 读时间给 UI。
 */

esp_err_t rtc_service_init(void);                    /* 探测 RTC + 初始化 SNTP（幂等，可重复调） */
esp_err_t rtc_service_get_time(pcf8563_time_t *t);   /* 读 RTC 当前时间 */
bool     rtc_service_rtc_valid(void);                /* VL 位：true=时间有效（未丢电） */
esp_err_t rtc_service_sync_from_ntp(int timeout_ms); /* 等 NTP 真实时间→写 RTC；超时 ESP_ERR_TIMEOUT */

#endif /* __RTC_SERVICE_H_ */

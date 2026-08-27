#ifndef __UI_TIME_H_
#define __UI_TIME_H_
#include "basic_timer.h"

/* screen2 时间相关控件的 LVGL 绑定层（与时间来源解耦）：
 * 只负责"pcf8563_time_t → 时间/日期/星期 三个控件"，不关心时间从哪来（RTC/NTP）、也不上锁。
 * 调用方需持有 LVGL 锁、UI 已初始化后再调用，建议每 ~1s 触发一次。 */
void ui_time_refresh(const pcf8563_time_t *t);

#endif /* __UI_TIME_H_ */

#ifndef __STEP_SVC_H_
#define __STEP_SVC_H_

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

/* ============================================================
 * 记步服务层 (basic_components/STEP_SVC)
 * 后台常驻任务, 与页面无关。纯服务, 不碰 LVGL。
 * - 周期读 BMI270 内置步数计数器(芯片内计数断电清零 → 用 NVS 累计, 掉电不丢)
 * - 维护 7 天历史 daily[0]=今天 ... daily[6]=6天前
 * - 依赖 RTC(rtc_service)做跨天滚动; 日期存 NVS anchor, 掉电跨 N 天自动移位
 * 使用顺序: step_svc_init() 开机一次(内部完成 bmi270_init + step_enable)
 * ============================================================ */

#define STEP_SVC_HIST_DAYS 7

esp_err_t step_svc_init(void);          /* 开机调一次: BMI270 初始化 + 载入 NVS + 起后台任务 */

uint32_t step_svc_get_today(void);      /* 今日实时步数(累计值, 只增不减) */
uint32_t step_svc_get_day(uint8_t i);   /* 第 i 天: 0=今天, 1=昨天, ... 6=六天前 */
bool     step_svc_ready(void);          /* 已初始化(可安全调用 get_*) */

#endif /* __STEP_SVC_H_ */

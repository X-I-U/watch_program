#ifndef __TIMER_SVC_H_
#define __TIMER_SVC_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * 计时器/秒表 服务层 (basic_components/TIMER_SVC)
 * 后台常驻、与页面无关 —— 退出 UI 页照样计时。
 * 只依赖 esp_timer / FreeRTOS, 不碰 LVGL。
 * 秒表上限 99:59.99(5,999,990ms), 到顶自动停止。
 * ============================================================ */

typedef enum {
    TIMER_CD_IDLE = 0,   /* 未设置 / 已清零 */
    TIMER_CD_SET,        /* 已设时长, 未开始 */
    TIMER_CD_RUNNING,    /* 倒计时中 */
    TIMER_CD_PAUSED,     /* 已暂停 */
} timer_cd_state_t;

typedef enum {
    TIMER_SW_STOPPED = 0, /* 未开始 / 已清空 / 已达上限 */
    TIMER_SW_RUNNING,
    TIMER_SW_PAUSED,
} timer_sw_state_t;

/* 倒计时到点回调(服务任务上下文调用, 不能碰 LVGL, 必须快速返回) */
typedef void (*timer_cd_done_cb_t)(void);

void timer_svc_init(void);   /* 开机调用一次, 启动后台任务 */

/* ---------- 倒计时 ---------- */
void timer_cd_set(uint32_t h, uint32_t m, uint32_t s); /* 设时长(仅 IDLE/SET 有效) */
void timer_cd_start(void);     /* 开始/继续(剩余>0 才生效) */
void timer_cd_pause(void);
void timer_cd_cancel(void);    /* 清零 → IDLE */
timer_cd_state_t timer_cd_get_state(void);
uint32_t timer_cd_get_remaining_ms(void);
void timer_cd_set_done_cb(timer_cd_done_cb_t cb);

/* ---------- 秒表 ---------- */
void timer_sw_start(void);
void timer_sw_pause(void);
void timer_sw_reset(void);
timer_sw_state_t timer_sw_get_state(void);
uint32_t timer_sw_get_elapsed_ms(void);  /* 达上限后冻结在 99:59.99 */

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_SVC_H_ */

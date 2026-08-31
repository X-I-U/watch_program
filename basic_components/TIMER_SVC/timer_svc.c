#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "timer_svc.h"

static const char *TAG = "timer_svc";

#define SW_CAP_MS   (99 * 60 * 1000 + 59 * 1000 + 990)   /* 99:59.99 */

/* ---------- 倒计时 ---------- */
static volatile timer_cd_state_t s_cd_state = TIMER_CD_IDLE;
static volatile uint32_t s_cd_remaining_ms = 0;  /* 非运行时的权威剩余值 */
static volatile int64_t  s_cd_start_us = 0;
static timer_cd_done_cb_t s_cd_done_cb = NULL;

/* ---------- 秒表 ---------- */
static volatile timer_sw_state_t s_sw_state = TIMER_SW_STOPPED;
static volatile uint32_t s_sw_elapsed_ms = 0;    /* 非运行时的冻结值 */
static volatile int64_t  s_sw_start_us = 0;

/* 当前剩余(运行中 = 从起始时刻反推, 保证暂停/继续不漂移) */
static uint32_t cd_now_remaining(void)
{
    if (s_cd_state != TIMER_CD_RUNNING) {
        return s_cd_remaining_ms;
    }
    int64_t elapsed_us = esp_timer_get_time() - s_cd_start_us;
    if (elapsed_us >= (int64_t)s_cd_remaining_ms * 1000) {
        return 0;
    }
    return s_cd_remaining_ms - (uint32_t)(elapsed_us / 1000);
}

static uint32_t sw_now_elapsed(void)
{
    if (s_sw_state != TIMER_SW_RUNNING) {
        return s_sw_elapsed_ms;
    }
    int64_t elapsed_ms = (esp_timer_get_time() - s_sw_start_us) / 1000;
    uint32_t total = s_sw_elapsed_ms + (uint32_t)elapsed_ms;
    return (total > SW_CAP_MS) ? SW_CAP_MS : total;
}

/* 后台任务: 50ms 检查 倒计时到点 / 秒表达上限 */
static void timer_svc_task(void *arg)
{
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(50));

        if (s_cd_state == TIMER_CD_RUNNING && cd_now_remaining() == 0) {
            ESP_LOGI(TAG, "countdown done");
            s_cd_state = TIMER_CD_IDLE;      /* 到点即清零(用户重新设置) */
            s_cd_remaining_ms = 0;
            if (s_cd_done_cb) {
                s_cd_done_cb();              /* 服务任务上下文, 上层只许入队 */
            }
        }

        if (s_sw_state == TIMER_SW_RUNNING && sw_now_elapsed() >= SW_CAP_MS) {
            ESP_LOGI(TAG, "stopwatch reached cap 99:59.99");
            s_sw_elapsed_ms = SW_CAP_MS;
            s_sw_state = TIMER_SW_STOPPED;   /* 到顶停止 */
        }
    }
}

/* ---------- 倒计时 API ---------- */
void timer_cd_set(uint32_t h, uint32_t m, uint32_t s)
{
    if (s_cd_state == TIMER_CD_RUNNING || s_cd_state == TIMER_CD_PAUSED) {
        return;                              /* 运行/暂停中不允许改时长 */
    }
    s_cd_remaining_ms = (h * 3600 + m * 60 + s) * 1000;
    s_cd_state = (s_cd_remaining_ms > 0) ? TIMER_CD_SET : TIMER_CD_IDLE;
}

void timer_cd_start(void)
{
    if (cd_now_remaining() == 0) {
        return;                              /* 没设时间, 不开始 */
    }
    s_cd_remaining_ms = cd_now_remaining();
    s_cd_start_us = esp_timer_get_time();
    s_cd_state = TIMER_CD_RUNNING;
}

void timer_cd_pause(void)
{
    if (s_cd_state != TIMER_CD_RUNNING) {
        return;
    }
    s_cd_remaining_ms = cd_now_remaining();
    s_cd_state = TIMER_CD_PAUSED;
}

void timer_cd_cancel(void)
{
    s_cd_remaining_ms = 0;
    s_cd_state = TIMER_CD_IDLE;
}

timer_cd_state_t timer_cd_get_state(void) { return s_cd_state; }
uint32_t timer_cd_get_remaining_ms(void)  { return cd_now_remaining(); }
void timer_cd_set_done_cb(timer_cd_done_cb_t cb) { s_cd_done_cb = cb; }

/* ---------- 秒表 API ---------- */
void timer_sw_start(void)
{
    if (s_sw_state == TIMER_SW_RUNNING) {
        return;
    }
    uint32_t cur = sw_now_elapsed();
    if (cur >= SW_CAP_MS) {
        cur = 0;                             /* 到顶后重按开始 → 重新计时 */
    }
    s_sw_elapsed_ms = cur;
    s_sw_start_us = esp_timer_get_time();
    s_sw_state = TIMER_SW_RUNNING;
}

void timer_sw_pause(void)
{
    if (s_sw_state != TIMER_SW_RUNNING) {
        return;
    }
    s_sw_elapsed_ms = sw_now_elapsed();
    s_sw_state = TIMER_SW_PAUSED;
}

void timer_sw_reset(void)
{
    s_sw_elapsed_ms = 0;
    s_sw_state = TIMER_SW_STOPPED;
}

timer_sw_state_t timer_sw_get_state(void)  { return s_sw_state; }
uint32_t timer_sw_get_elapsed_ms(void)     { return sw_now_elapsed(); }

void timer_svc_init(void)
{
    xTaskCreate(timer_svc_task, "timer_svc", 4096, NULL, 3, NULL);
    ESP_LOGI(TAG, "init done");
}

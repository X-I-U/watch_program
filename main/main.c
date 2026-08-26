#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "lcd.h"
#include "ui.h"
#include "gui_guider.h"
#include "lv_dclock.h"
#include "wifista.h"
#include "nvs_flash.h"
#include "myntc.h"
#include "esp_sntp.h"
#include <time.h>
#include "speaker.h"
#include "axp2101.h"
#include "music_core.h"

static SemaphoreHandle_t lvgl_mux = NULL;

static bool example_lvgl_lock(int timeout_ms)
{
    assert(lvgl_mux && "bsp_display_start must be called first");

    const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTake(lvgl_mux, timeout_ticks) == pdTRUE;
}

static void example_lvgl_unlock(void)
{
    assert(lvgl_mux && "bsp_display_start must be called first");
    xSemaphoreGive(lvgl_mux);
}

static const char *TAG = "main";

/* 数字时钟控件的时间变量（定义在 GUI Guider 生成的 setup_scr_screen_2.c） */
extern int screen_2_digital_clock_1_hour_value;
extern int screen_2_digital_clock_1_min_value;
extern int screen_2_digital_clock_1_sec_value;
extern char screen_2_digital_clock_1_meridiem[];

static void sync_lvgl_clock_from_rtc(void);   /* 前置声明：clock_sync_task 先于定义调用 */

/* 时间是否已经有效（NTP 同步完成后 time() 才不再是 1970） */
static bool time_is_valid(void)
{
    time_t now = time(NULL);
    struct tm t = {0};
    localtime_r(&now, &t);
    return t.tm_year >= (2025 - 1900);
}

/* NTP 同步任务：等 WiFi 连上、NTP 同步后把真实时间写到 LVGL 时钟，并定期校准防漂移 */
static void clock_sync_task(void *arg)
{
    /* 一直等，直到 NTP 同步成功；期间每 5 秒重启一次 SNTP 主动触发查询
       （SNTP 默认轮询周期很长，不主动触发可能一小时才同步一次） */
    while (!time_is_valid()) {
        esp_sntp_restart();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    sync_lvgl_clock_from_rtc();
    ESP_LOGI(TAG, "NTP time synced, LVGL clock updated");

    /* 之后每 1 分钟校准一次，消除 LVGL 定时器累积的秒级漂移 */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(60 * 1000));
        sync_lvgl_clock_from_rtc();
    }
    vTaskDelete(NULL);
}

/* 把系统实时时间同步到 LVGL 数字时钟控件（12 小时制） */
static void sync_lvgl_clock_from_rtc(void)
{
    time_t now = time(NULL);
    struct tm t = {0};
    localtime_r(&now, &t);

    int h12 = t.tm_hour % 12;
    if (h12 == 0) h12 = 12;

    if (example_lvgl_lock(-1)) {
        /* 时钟控件可能已被销毁（切到其它 screen 时 screen_2 被 auto_del），先判有效避免崩溃 */
        if (lv_obj_is_valid(guider_ui.screen_2_digital_clock_1)) {
            screen_2_digital_clock_1_hour_value = h12;
            screen_2_digital_clock_1_min_value  = t.tm_min;
            screen_2_digital_clock_1_sec_value  = t.tm_sec;
            strcpy(screen_2_digital_clock_1_meridiem, (t.tm_hour < 12) ? "AM" : "PM");
            lv_dclock_set_text_fmt(guider_ui.screen_2_digital_clock_1,
                                   "%d:%02d:%02d %s",
                                   screen_2_digital_clock_1_hour_value,
                                   screen_2_digital_clock_1_min_value,
                                   screen_2_digital_clock_1_sec_value,
                                   screen_2_digital_clock_1_meridiem);
        }
        example_lvgl_unlock();
    }
}

// LVGL任务
static void example_lvgl_port_task(void *arg)
{
    //ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
    while (1) {
         //ESP_LOGI(TAG, "LVGL task");
        // Lock the mutex due to the LVGL APIs are not thread-safe
        if (example_lvgl_lock(-1)) {
            task_delay_ms = lv_timer_handler();
            // Release the mutex
            example_lvgl_unlock();
        }
        if (task_delay_ms > EXAMPLE_LVGL_TASK_MAX_DELAY_MS) {
            task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
        } else if (task_delay_ms < EXAMPLE_LVGL_TASK_MIN_DELAY_MS) {
            task_delay_ms = EXAMPLE_LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}

void app_main(void)
{
    axp2101_i2c_init();
    axp2101_set_aldo1_voltage(3.3);
    axp2101_enable_aldo1(true);

    esp_err_t nvs_ret = nvs_flash_init();
    if (nvs_ret == ESP_ERR_NVS_NO_FREE_PAGES || nvs_ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs_ret);
    // ① 阻塞等 WiFi（wifista_init 非阻塞，要额外等）
    wifista_init();
    wifi_wait_connected(10000);   // 你之前加的阻塞等待函数
    // ② 音乐业务初始化（建持久管线 + 歌单），进入音乐页后由 UI 操作播放
    music_core_init();

     lcd_display_init();
     lcd_touch_init();
     lvgl_timer_init(lvgl_init());

     lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux);
    xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, NULL);

    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (example_lvgl_lock(-1)) {
        guider_ui_init();       /* GUI Guider UI */

        // Release the mutex
        example_lvgl_unlock();
    }

    /* ---- NTP 实时时间：连 WiFi -> 同步 SNTP -> 把真实时间写到 LVGL 时钟 ---- */

    //wifista_init();             /* 连接 WiFi（SSID/密码在 wifista.h 里配置，断开会自动重连） */
    myntc_init();               /* 初始化 SNTP + 时区 CST-8 */

    /* 后台任务：等 NTP 同步后把真实时间同步到 LVGL 时钟，并定期校准 */
    //xTaskCreate(clock_sync_task, "clock_sync", 4096, NULL, 5, NULL);
}
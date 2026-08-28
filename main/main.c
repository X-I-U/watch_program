#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "lcd.h"
#include "ui.h"
#include "gui_guider.h"
#include "wifista.h"
#include "nvs_flash.h"
#include "speaker.h"
#include "axp2101.h"
#include "music_core.h"
#include "mic_driver.h"    /* Step1: MSM261S4030H0R 咪头驱动 */
#include "xiaozhi.h"       /* Stage B: 小智AI 服务层 */
#include "rtc_service.h"   /* 时间服务中间层：NTP→RTC、读 RTC */
#include "ui_time.h"       /* LVGL 绑定层：时间/日期/星期 三个控件 */

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

/* 时间显示任务：每 1s 读一次 RTC，刷 screen2 的时间/日期/星期 三个控件。
   显示只依赖 RTC，不依赖网络；NTP 只在 rtc_sync_task 里一次性写 RTC。 */
static void rtc_display_task(void *arg)
{
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        pcf8563_time_t t = {0};
        if (rtc_service_get_time(&t) != ESP_OK) {
            continue;   /* RTC 暂时读不到（总线忙等），下个周期再试 */
        }
        if (example_lvgl_lock(-1)) {
            ui_time_refresh(&t);
            example_lvgl_unlock();
        }
    }
}

/* RTC 同步任务：等 NTP 拿到真实时间 → 写入 RTC（一次性）。
   之后 RTC 由 VRTC 供电持续走时，显示任务直接从 RTC 读，无需频繁 NTP。 */
static void rtc_sync_task(void *arg)
{
    esp_err_t ret = rtc_service_sync_from_ntp(60000);   /* 最多等 60s */
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NTP->RTC sync failed (ret=%d), display will use RTC as-is", ret);
    }
    vTaskDelete(NULL);
}

/* TEMP: Stage D 自动触发一轮对话(等 WS 连上后), 验证后删除 */
static void auto_talk_task(void *arg)
{
    vTaskDelay(pdMS_TO_TICKS(12000));   /* WS 连上约 9s, 留余量 */
    for (int i = 0; i < 3; i++) {
        if (xiaozhi_talk() == ESP_OK) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
    vTaskDelete(NULL);
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
    // ① 开始连 WiFi（wifista_init 非阻塞；不再阻塞等——网络等待挪到 rtc_sync_task 内部，
    //    避免没网时屏幕空等，显示初始化立刻进行）
    wifista_init();
    // ② 音乐业务初始化（建持久管线 + 歌单，不依赖网络），进入音乐页后由 UI 操作播放
    music_core_init();

     mic_init();            /* Step1: 咪头驱动初始化 */

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

    /* ---- RTC 时间：NTP 一次写入 RTC，之后显示从 RTC 读 ---- */
    esp_err_t rtc_ret = rtc_service_init();   /* 探测 RTC + SNTP */
    if (rtc_ret != ESP_OK) {
        ESP_LOGE(TAG, "rtc_service_init failed (%d); time display will not work", rtc_ret);
    } else {
        xTaskCreate(rtc_sync_task, "rtc_sync", 4096, NULL, 5, NULL);
    }
    xTaskCreate(rtc_display_task, "rtc_disp", 4096, NULL, 5, NULL);

    /* ---- 小智AI: 放 LCD/LVGL 之后初始化(esp_xiaozhi 吃内存, 别抢 LVGL 的 DMA 缓冲) ---- */
    xiaozhi_init();
    /* 栈放 PSRAM: open_audio_channel 内部用 cJSON, 栈需求大, 不给内部 RAM 添负担 */
    xTaskCreatePinnedToCoreWithCaps(auto_talk_task, "auto_talk", 8192, NULL, 4, NULL, tskNO_AFFINITY,
                                    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);   /* TEMP: Stage D 自动触发 */
}

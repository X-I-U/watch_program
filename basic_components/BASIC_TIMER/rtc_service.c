#include "rtc_service.h"
#include "driver/i2c.h"
#include "esp_sntp.h"
#include "esp_log.h"
#include "myntc.h"
#include <time.h>

static const char *TAG = "rtc_svc";

#define RTC_I2C_PORT   I2C_NUM_1   /* 与 AXP2101/触摸共用总线 */

static bool s_sntp_started = false;

/* 系统时间是否已被 NTP 同步（SNTP 成功前 time() 还是 1970） */
static bool time_is_valid(void)
{
    time_t now = time(NULL);
    struct tm t = {0};
    localtime_r(&now, &t);
    return t.tm_year >= (2025 - 1900);
}

esp_err_t rtc_service_init(void)
{
    esp_err_t ret = pcf8563_init(RTC_I2C_PORT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "pcf8563 probe failed on I2C_NUM_%d (bus init by AXP2101?)", RTC_I2C_PORT);
        return ret;
    }
    if (!s_sntp_started) {
        myntc_init();   /* SNTP 服务器 + 时区 CST-8 */
        s_sntp_started = true;
    }
    return ESP_OK;
}

esp_err_t rtc_service_get_time(pcf8563_time_t *t)
{
    return pcf8563_get_time(t);
}

bool rtc_service_rtc_valid(void)
{
    bool valid = false;
    pcf8563_is_valid(&valid);
    return valid;
}

esp_err_t rtc_service_sync_from_ntp(int timeout_ms)
{
    /* 等 NTP：SNTP 默认轮询周期很长，主动 restart 触发查询，每 2s 试一次直到超时 */
    if (!time_is_valid()) {
        int waited = 0;
        while (waited < timeout_ms) {
            esp_sntp_restart();
            vTaskDelay(pdMS_TO_TICKS(2000));
            waited += 2000;
            if (time_is_valid()) break;
        }
        if (!time_is_valid()) {
            ESP_LOGW(TAG, "NTP sync timeout (%d ms)", timeout_ms);
            return ESP_ERR_TIMEOUT;
        }
    }

    /* 把系统时间（此时已是 NTP 真实时间）写入 RTC */
    time_t now = time(NULL);
    struct tm t = {0};
    localtime_r(&now, &t);

    pcf8563_time_t r = {
        .year    = t.tm_year + 1900,
        .month   = t.tm_mon + 1,
        .day     = t.tm_mday,
        .weekday = t.tm_wday,          /* 0=周日，与驱动透传约定一致 */
        .hour    = t.tm_hour,
        .min     = t.tm_min,
        .sec     = t.tm_sec,
    };
    esp_err_t ret = pcf8563_set_time(&r);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "write RTC failed: %d", ret);
        return ret;
    }
    pcf8563_time_t back = {0};
    if (pcf8563_get_time(&back) == ESP_OK) {
        ESP_LOGI(TAG, "NTP->RTC ok: %04d-%02d-%02d %02d:%02d:%02d wd=%d",
                 back.year, back.month, back.day, back.hour, back.min, back.sec, back.weekday);
    }
    return ESP_OK;
}

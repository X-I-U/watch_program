#include "rtc_service.h"
#include "driver/i2c.h"
#include "esp_sntp.h"
#include "esp_log.h"
#include "myntc.h"
#include <time.h>

static const char *TAG = "rtc_svc";

#define RTC_I2C_PORT   I2C_NUM_1   /* 与 AXP2101/触摸共用总线 */

static bool s_sntp_started = false;

/* 最近一次 NTP 同步是否完成(由 SNTP 同步回调置位)。
   手动校准/开机同步都据此等待"一次真正的新同步", 而不是盲写当前系统时间。 */
static volatile bool s_ntp_just_synced = false;

/* SNTP 每次成功同步都会回调这里, tv 是服务器给的 UTC 秒。
   直接按 UTC+8 写 RTC —— 不读 time()/不依赖 TZ, 因此绝不会重复加 8 小时。
   (之前: 时区靠 localtime+TZ 有时不生效→慢8h; 改成手工 +8 后又遇到系统时钟
   偶发已是本地→快8h。用回调的原始 UTC 只加一次, 彻底消除这两种偏差。) */
static void sntp_sync_cb(struct timeval *tv)
{
    if (!tv) {
        return;
    }
    time_t sec = tv->tv_sec + (8 * 3600);   /* UTC → 北京时间(CST=UTC+8) */
    struct tm t = {0};
    gmtime_r(&sec, &t);

    pcf8563_time_t r = {
        .year    = t.tm_year + 1900,
        .month   = t.tm_mon + 1,
        .day     = t.tm_mday,
        .weekday = t.tm_wday,               /* 0=周日, 与驱动透传约定一致 */
        .hour    = t.tm_hour,
        .min     = t.tm_min,
        .sec     = t.tm_sec,
    };
    esp_err_t ret = pcf8563_set_time(&r);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "NTP cb -> RTC write failed %d", ret);
    } else {
        ESP_LOGI(TAG, "NTP cb -> RTC %04d-%02d-%02d %02d:%02d:%02d wd=%d",
                 r.year, r.month, r.day, r.hour, r.min, r.sec, r.weekday);
    }
    s_ntp_just_synced = true;
}

esp_err_t rtc_service_init(void)
{
    esp_err_t ret = pcf8563_init(RTC_I2C_PORT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "pcf8563 probe failed on I2C_NUM_%d (bus init by AXP2101?)", RTC_I2C_PORT);
        return ret;
    }
    if (!s_sntp_started) {
        myntc_init();   /* SNTP 服务器 + 时区 CST-8(仅影响 localtime 日志) */
        s_sntp_started = true;
    }
    /* 关键: 每次 NTP 同步成功 → 回调里以原始 UTC+8 写 RTC */
    esp_sntp_set_time_sync_notification_cb(sntp_sync_cb);
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

/* 强制做一次新的 NTP 同步, 等 SNTP 回调把 RTC 写好后返回。
   (不再读 time() 现写 —— 回调里已保证只加一次 +8 且必定正确。)
   每次手动校准/开机都会触发一次真正查询, 避免写入旧系统时间。 */
esp_err_t rtc_service_sync_from_ntp(int timeout_ms)
{
    s_ntp_just_synced = false;
    esp_sntp_restart();   /* 主动触发一次查询 */

    int waited = 0;
    while (waited < timeout_ms) {
        vTaskDelay(pdMS_TO_TICKS(200));
        waited += 200;
        if (s_ntp_just_synced) {
            return ESP_OK;
        }
    }
    ESP_LOGW(TAG, "NTP sync timeout (%d ms)", timeout_ms);
    return ESP_ERR_TIMEOUT;
}

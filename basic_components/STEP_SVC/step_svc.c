#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "bmi270.h"
#include "rtc_service.h"
#include "step_svc.h"

static const char *TAG = "step_svc";

#define NVS_NAMESPACE  "steps"
#define NVS_KEY_HIST   "hist"    /* 7×u32 blob, [0]=今天 ... [6]=6天前 */
#define NVS_KEY_ACCUM  "accum"   /* u32: 今天已并入累计的步数(最近一次保存时) */
#define NVS_KEY_ANCHOR "anchor"  /* u32: hist[0] 对应日期 YYYYMMDD */

#define STEP_POLL_MS       1000   /* 读芯片周期 */
#define STEP_PERSIST_MS    30000  /* NVS 防抖保存周期 */

static uint32_t s_hist[STEP_SVC_HIST_DAYS]; /* [0]=今天 ... [6]=6天前 */
static uint32_t s_today_accum;              /* 已持久化的今天累计 */
static uint32_t s_chip_base;                /* 开机/最近保存时的芯片计数基线 */
static uint32_t s_anchor;                   /* hist[0] 对应日期 YYYYMMDD, 0=未知 */
static bool     s_ready = false;
static uint32_t s_last_today = 0;           /* 上次保存时的今日值(防抖用) */

/* ---------- 日期工具(YYYYMMDD <-> 自 1970-01-01 天数) ---------- */
static uint32_t date_encode(const pcf8563_time_t *t)
{
    return (uint32_t)t->year * 10000 + (uint32_t)t->month * 100 + (uint32_t)t->day;
}

/* RTC 日期是否可作 anchor: 开机后 NTP 没同步时 RTC 可能读出垃圾(如 2010-00-00),
 * 只有年月日都在合理范围才用, 避免拿错误日期去滚历史 */
static bool date_sane(const pcf8563_time_t *t)
{
    return t->year >= 2000 && t->year <= 2100 &&
           t->month >= 1 && t->month <= 12 &&
           t->day >= 1 && t->day <= 31;
}

/* Howard Hinnant days_from_civil, 返回自 1970-01-01 天数(可负) */
static int32_t days_from_civil(int y, unsigned m, unsigned d)
{
    y -= (int)(m <= 2);
    int era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = (unsigned)(y - era * 400);                                 /* [0,399] */
    unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;            /* [0,365] */
    unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;                     /* [0,146096] */
    return era * 146097 + (int32_t)doe - 719468;
}

static int32_t date_to_days(uint32_t yyyymmdd)
{
    return days_from_civil((int)(yyyymmdd / 10000),
                           (yyyymmdd / 100) % 100, yyyymmdd % 100);
}

/* ---------- NVS ---------- */
static esp_err_t step_nvs_load(void)
{
    nvs_handle_t h;
    esp_err_t r = nvs_open(NVS_NAMESPACE, NVS_READONLY, &h);
    if (r != ESP_OK) return r;

    size_t len = sizeof(s_hist);
    r = nvs_get_blob(h, NVS_KEY_HIST, s_hist, &len);
    if (r == ESP_OK && len != sizeof(s_hist)) {
        memset(s_hist, 0, sizeof(s_hist));   /* 版本/尺寸不符, 当首次 */
        r = ESP_ERR_NVS_NOT_FOUND;
    }
    if (r == ESP_OK) {
        r = nvs_get_u32(h, NVS_KEY_ACCUM, &s_today_accum);
    }
    if (r == ESP_OK) {
        r = nvs_get_u32(h, NVS_KEY_ANCHOR, &s_anchor);
    }
    nvs_close(h);
    return r;   /* ESP_ERR_NVS_NOT_FOUND = 首次上电 */
}

static esp_err_t step_nvs_save(void)
{
    nvs_handle_t h;
    esp_err_t r = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h);
    if (r != ESP_OK) return r;
    r = nvs_set_blob(h, NVS_KEY_HIST, s_hist, sizeof(s_hist));
    if (r == ESP_OK) r = nvs_set_u32(h, NVS_KEY_ACCUM, s_today_accum);
    if (r == ESP_OK) r = nvs_set_u32(h, NVS_KEY_ANCHOR, s_anchor);
    if (r == ESP_OK) r = nvs_commit(h);
    nvs_close(h);
    return r;
}

/* ---------- 跨天滚动: 日期从 anchor 变成 new_date ----------
 * final_today = 旧今天最终步数(跨过零点前的完整一天)。新今天清零。 */
static void step_rollover(uint32_t new_date, uint32_t final_today)
{
    int32_t gap = (int32_t)(date_to_days(new_date) - date_to_days(s_anchor));
    if (gap <= 0) gap = 1;                    /* 时钟回退/同天兜底 */

    if (gap >= STEP_SVC_HIST_DAYS) {
        memset(s_hist, 0, sizeof(s_hist));    /* 掉电超过 7 天, 历史全丢 */
    } else {
        uint32_t new_hist[STEP_SVC_HIST_DAYS] = { 0 };
        new_hist[gap] = final_today;          /* 旧今天 -> 第 gap 格(如隔天=昨天) */
        for (int k = 1; k < STEP_SVC_HIST_DAYS - gap; k++) {
            new_hist[gap + k] = s_hist[k];
        }
        memcpy(s_hist, new_hist, sizeof(s_hist));
    }

    s_today_accum = 0;
    s_anchor = new_date;
    s_chip_base = 0;   /* 由调用方在滚动后重读芯片设基线(跨零点芯片计数不归零) */
}

/* ---------- 读芯片当前计数(带基线) ---------- */
static esp_err_t step_read_chip(uint32_t *chip)
{
    uint32_t c = 0;
    if (bmi270_read_step_count(&c) != ESP_OK) {
        return ESP_FAIL;
    }
    *chip = c;
    return ESP_OK;
}

/* 今日实时值 = 已持久化累计 + (当前芯片 - 基线); 芯片回退(断电重置)时当没动 */
static uint32_t step_current_today(void)
{
    uint32_t chip = 0;
    if (step_read_chip(&chip) != ESP_OK) {
        return s_today_accum;
    }
    uint32_t delta = (chip >= s_chip_base) ? (chip - s_chip_base) : 0;
    return s_today_accum + delta;
}

/* ---------- 后台任务: 周期读 + 跨天检查 + 防抖保存 ---------- */
static void step_svc_task(void *arg)
{
    int64_t last_persist = esp_timer_get_time();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(STEP_POLL_MS));

        /* 1. 跨天检查(RTC 有效且日期合理才滚; init 时 RTC 无效的, 恢复后补设 anchor) */
        pcf8563_time_t t;
        if (rtc_service_rtc_valid() && rtc_service_get_time(&t) == ESP_OK && date_sane(&t)) {
            uint32_t now = date_encode(&t);
            if (s_anchor == 0) {
                s_anchor = now;              /* 首次拿到 RTC(开机后 NTP 才同步), 补 anchor */
                step_nvs_save();
            } else if (now != s_anchor) {
                uint32_t chip = 0;
                step_read_chip(&chip);
                step_rollover(now, s_today_accum + (chip - s_chip_base));
                s_chip_base = chip;
                step_nvs_save();
                s_last_today = step_current_today();
                last_persist = esp_timer_get_time();
                ESP_LOGI(TAG, "rollover -> anchor=%u today=%u hist=[%u %u %u %u %u %u %u]",
                         (unsigned)now, (unsigned)s_last_today,
                         (unsigned)s_hist[0], (unsigned)s_hist[1], (unsigned)s_hist[2],
                         (unsigned)s_hist[3], (unsigned)s_hist[4], (unsigned)s_hist[5],
                         (unsigned)s_hist[6]);
            }
        }

        /* 2. 防抖保存: 今日有变化 且 距上次≥30s 才写 NVS(省 flash 磨损) */
        uint32_t today = step_current_today();
        int64_t now_us = esp_timer_get_time();
        if (today != s_last_today && (now_us - last_persist) >= STEP_PERSIST_MS * 1000) {
            uint32_t chip = 0;
            if (step_read_chip(&chip) == ESP_OK) {
                s_today_accum = s_today_accum + (chip - s_chip_base);
                s_chip_base = chip;
                if (step_nvs_save() == ESP_OK) {
                    ESP_LOGI(TAG, "persist today=%u", (unsigned)s_today_accum);
                }
                s_last_today = today;
                last_persist = now_us;
            }
        }
    }
}

/* ---------- 公开 API ---------- */
esp_err_t step_svc_init(void)
{
    if (bmi270_init() != ESP_OK) {
        ESP_LOGE(TAG, "bmi270 init failed");
        return ESP_FAIL;
    }
    if (bmi270_step_enable() != ESP_OK) {
        ESP_LOGE(TAG, "step enable failed");
        return ESP_FAIL;
    }

    /* 开机基线: 使能后第一次读的芯片计数 */
    if (step_read_chip(&s_chip_base) != ESP_OK) {
        s_chip_base = 0;
    }

    memset(s_hist, 0, sizeof(s_hist));
    s_today_accum = 0;
    s_anchor = 0;

    esp_err_t r = step_nvs_load();
    if (r != ESP_OK && r != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "nvs load failed (%d), start fresh", r);
    }

    /* RTC 有效且日期合理才校准 anchor / 处理跨天(没同步时读出的日期是垃圾, 别用) */
    pcf8563_time_t t;
    if (rtc_service_rtc_valid() && rtc_service_get_time(&t) == ESP_OK && date_sane(&t)) {
        uint32_t now = date_encode(&t);
        if (s_anchor == 0) {
            /* 首次上电(无历史): 今天从 0 起 */
            s_anchor = now;
            s_today_accum = 0;
            step_nvs_save();
        } else if (now != s_anchor) {
            /* 掉电跨天: 旧今天落库, 今天清零 */
            uint32_t chip = 0;
            step_read_chip(&chip);
            step_rollover(now, s_today_accum + (chip - s_chip_base));
            s_chip_base = chip;
            step_nvs_save();
        }
    }
    /* RTC 无效(刚上电还没同步): 保持原样, 后台任务在 RTC 恢复后滚动 */

    s_ready = true;
    s_last_today = step_current_today();

    ESP_LOGI(TAG, "ready: today=%u hist=[%u %u %u %u %u %u %u] anchor=%u",
             (unsigned)s_last_today,
             (unsigned)s_hist[0], (unsigned)s_hist[1], (unsigned)s_hist[2],
             (unsigned)s_hist[3], (unsigned)s_hist[4], (unsigned)s_hist[5],
             (unsigned)s_hist[6], (unsigned)s_anchor);

    /* 栈必须在内部 RAM: NVS 写 flash 会禁用 cache, PSRAM 栈在写盘时会崩
       (esp_task_stack_is_sane_cache_disabled 断言) */
    xTaskCreatePinnedToCoreWithCaps(step_svc_task, "step_svc", 6144, NULL, 4, NULL,
                                    tskNO_AFFINITY, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    return ESP_OK;
}

uint32_t step_svc_get_today(void)
{
    if (!s_ready) return 0;
    return step_current_today();
}

uint32_t step_svc_get_day(uint8_t i)
{
    if (!s_ready || i >= STEP_SVC_HIST_DAYS) return 0;
    if (i == 0) return step_current_today();
    return s_hist[i];
}

bool step_svc_ready(void)
{
    return s_ready;
}

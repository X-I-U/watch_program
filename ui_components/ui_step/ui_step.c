#include <string.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "esp_log.h"
#include "rtc_service.h"
#include "step_svc.h"
#include "ui_step.h"

static const char *TAG = "ui_step";

#define STEP_GOAL           8000   /* 每日目标(与 GUI Guider chart Y 轴 0-8000 一致) */
#define CHART_ZOOM_100PCT   256    /* GUI Guider 默认 zoom 276/261, 会放大裁边, 复位 */
static const char *WEEK_CN[] = { "日", "一", "二", "三", "四", "五", "六" };  /* 0=周日 */

static void chart_push(lv_obj_t *chart, lv_chart_series_t *ser, uint32_t v);

static lv_obj_t *s_label_count;   /* screen_11_label_2: 今日步数数字 */
static lv_obj_t *s_arc;           /* screen_11_arc_1: 完成度 */
static lv_obj_t *s_chart;         /* screen_11_chart_1 */
static lv_chart_series_t *s_ser_past;   /* GUI Guider 已有系列(灰/黑) */
static lv_chart_series_t *s_ser_today;  /* 动态加的绿色系列 */
static lv_obj_t *s_day_lbl[STEP_SVC_HIST_DAYS];  /* 柱下星期标签, [0]=最左(6天前)..[6]=最右(今天) */

static uint32_t s_last_hist[STEP_SVC_HIST_DAYS];  /* chart 变化检测 */
static int      s_last_weekday = -1;
static int32_t  s_last_today   = -1;

/* ---------- 防悬空: 离开页面时任一绑定控件被释放 → 清空所有指针 ---------- */
static void ui_step_cleanup(void)
{
    s_label_count = NULL;
    s_arc = NULL;
    s_chart = NULL;
    s_ser_past = NULL;
    s_ser_today = NULL;
    for (int i = 0; i < STEP_SVC_HIST_DAYS; i++) {
        s_day_lbl[i] = NULL;
    }
    s_last_weekday = -1;
    s_last_today = -1;
}

static void ui_step_delete_cb(lv_event_t *e)
{
    ui_step_cleanup();
}

static void attach_widget_delete_cb(lv_obj_t *obj)
{
    if (obj) lv_obj_add_event_cb(obj, ui_step_delete_cb, LV_EVENT_DELETE, NULL);
}

/* 刷新 7 天柱状图: 灰=6天前..昨天, 绿=今天(右起第7根)。返回是否重绘过。 */
static void update_chart(void)
{
    uint32_t hist[STEP_SVC_HIST_DAYS];
    bool changed = false;
    for (int i = 0; i < STEP_SVC_HIST_DAYS; i++) {
        /* 值超过目标就按目标画(柱满格), 也避免溢出 lv_coord_t(int16) */
        uint32_t v = step_svc_get_day((uint8_t)i);
        hist[i] = (v > STEP_GOAL) ? STEP_GOAL : v;
        if (hist[i] != s_last_hist[i]) changed = true;
    }
    if (!changed) return;

    memcpy(s_last_hist, hist, sizeof(hist));
    if (!s_chart || !s_ser_past || !s_ser_today) return;
    if (!lv_obj_is_valid(s_chart)) return;

    /* 显式回到起点(第0根在最左), 再顺序填 7 根 */
    s_ser_past->start_point = 0;
    s_ser_today->start_point = 0;

    /* 历史(黑): 左→右 = 6天前...昨天, 今天位置留空(绿柱负责)。
       注意 0 步的柱 LVGL 也会在底部画一条余白, 用 LV_CHART_POINT_NONE 才完全不画 */
    chart_push(s_chart, s_ser_past, hist[6]);
    chart_push(s_chart, s_ser_past, hist[5]);
    chart_push(s_chart, s_ser_past, hist[4]);
    chart_push(s_chart, s_ser_past, hist[3]);
    chart_push(s_chart, s_ser_past, hist[2]);
    chart_push(s_chart, s_ser_past, hist[1]);
    chart_push(s_chart, s_ser_past, 0);

    /* 今天(绿): 只在最右一根有值, 其余留空 */
    chart_push(s_chart, s_ser_today, 0);
    chart_push(s_chart, s_ser_today, 0);
    chart_push(s_chart, s_ser_today, 0);
    chart_push(s_chart, s_ser_today, 0);
    chart_push(s_chart, s_ser_today, 0);
    chart_push(s_chart, s_ser_today, 0);
    chart_push(s_chart, s_ser_today, hist[0]);
}

/* 往图表压一个值; 0 步 → LV_CHART_POINT_NONE(不画, 避免底部余白条) */
static void chart_push(lv_obj_t *chart, lv_chart_series_t *ser, uint32_t v)
{
    lv_chart_set_next_value(chart, ser, (v > 0) ? (lv_coord_t)v : LV_CHART_POINT_NONE);
}

/* 刷新柱下星期标签: 最右=今天星期, 往左逐天回退 */
static void update_day_labels(void)
{
    pcf8563_time_t t;
    int wd = -1;
    if (rtc_service_get_time(&t) == ESP_OK) {
        wd = t.weekday % 7;   /* 0=周日 */
    }
    if (wd == s_last_weekday) return;
    s_last_weekday = wd;

    for (int i = 0; i < STEP_SVC_HIST_DAYS; i++) {
        if (!s_day_lbl[i] || !lv_obj_is_valid(s_day_lbl[i])) continue;
        int dw = (wd - (STEP_SVC_HIST_DAYS - 1 - i) + 21) % 7;
        lv_label_set_text(s_day_lbl[i], WEEK_CN[dw]);
    }
}

void ui_step_refresh(void)
{
    if (!s_label_count) return;   /* 未进过 / 已离开 screen11 */

    uint32_t today = step_svc_get_today();

    /* 今日步数 + 目标环(变化才刷) */
    if ((int32_t)today != s_last_today) {
        s_last_today = (int32_t)today;
        if (lv_obj_is_valid(s_label_count)) {
            lv_label_set_text_fmt(s_label_count, "%u", (unsigned)today);
        }
        if (s_arc && lv_obj_is_valid(s_arc)) {
            uint32_t pct = (today >= STEP_GOAL) ? 100 : (uint32_t)((uint64_t)today * 100 / STEP_GOAL);
            lv_arc_set_value(s_arc, (int)pct);
        }
    }

    update_day_labels();
    update_chart();
}

void ui_step_screen_created(lv_ui *ui)
{
    s_label_count = ui->screen_11_label_2;
    s_arc         = ui->screen_11_arc_1;
    s_chart       = ui->screen_11_chart_1;
    s_ser_past    = ui->screen_11_chart_1_0;   /* GUI Guider 已建的系列 */

    /* 双系列高亮今天: 绿色系列动态加(离开页面随 chart 一起释放, 重进再建) */
    s_ser_today = lv_chart_add_series(s_chart, lv_color_hex(0x00f71c), LV_CHART_AXIS_PRIMARY_Y);

    /* 柱下 7 个星期标签(用户在 GUI Guider 里加的"一~日", 已用中文字体):
       左→右 = 6天前 → 今天, 文字由 update_day_labels 按今天星期动态填 */
    lv_obj_t *lbls[STEP_SVC_HIST_DAYS] = {
        ui->screen_11_label_10,   /* 最左 = 一 */
        ui->screen_11_label_9,    /* 二 */
        ui->screen_11_label_8,    /* 三 */
        ui->screen_11_label_7,    /* 四 */
        ui->screen_11_label_6,    /* 五 */
        ui->screen_11_label_4,    /* 六 */
        ui->screen_11_label_3,    /* 最右 = 日 = 今天 */
    };
    for (int i = 0; i < STEP_SVC_HIST_DAYS; i++) {
        s_day_lbl[i] = lbls[i];
    }

    /* GUI Guider 的 zoom 不是 100%, 复位防柱子被裁 */
    if (lv_obj_is_valid(s_chart)) {
        lv_chart_set_zoom_x(s_chart, CHART_ZOOM_100PCT);
        lv_chart_set_zoom_y(s_chart, CHART_ZOOM_100PCT);
    }

    /* 离开页面被释放时停刷新 */
    attach_widget_delete_cb(s_label_count);
    attach_widget_delete_cb(s_arc);
    attach_widget_delete_cb(s_chart);
    for (int i = 0; i < STEP_SVC_HIST_DAYS; i++) {
        attach_widget_delete_cb(s_day_lbl[i]);
    }

    /* 初始状态 */
    memset(s_last_hist, 0xFF, sizeof(s_last_hist));   /* 强制首刷 */
    s_last_weekday = -1;
    s_last_today = -1;
    ui_step_refresh();
}

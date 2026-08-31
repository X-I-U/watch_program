#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "timer_svc.h"
#include "fonts.h"
#include "timer_ui.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

/* ============================================================
 * 计时器/秒表 UI 绑定层
 * 逻辑在 TIMER_SVC(后台常驻)。本层只做: 滚轮设时 / 按钮 / 显示 / 指示 / 到点弹窗。
 * ============================================================ */

/* ---------- screen5 控件引用 ---------- */
static lv_obj_t *s_carousel;          /* screen_5_carousel_1 */
static lv_obj_t *s_el1, *s_el2;       /* 两页 element */
static lv_obj_t *s_roller_h, *s_roller_m, *s_roller_s;  /* 计时器滚轮 */
static lv_obj_t *s_t_start, *s_t_pause, *s_t_cancel;    /* 计时器按钮 */
static lv_obj_t *s_sw_label, *s_sw_start, *s_sw_pause, *s_sw_reset; /* 秒表 */
static lv_obj_t *s_icon_timer, *s_icon_sw;              /* 指示图标 */
static lv_timer_t *s_timer;           /* 刷新定时器(10ms) */
static int64_t s_leave_us = 0;        /* 离开判定(带 500ms 缓冲) */

/* 去重缓存 */
static timer_cd_state_t s_last_cd_state = (timer_cd_state_t)-1;
static timer_sw_state_t s_last_sw_state = (timer_sw_state_t)-1;
static uint32_t s_last_rem_sec = 0xFFFFFFFF;
static int      s_last_sw_ms = -1;
static int      s_last_page = -1;

/* ============================================================
 * 全局到点弹窗: 服务任务 → 队列 → 常驻 LVGL 定时器 → 当前屏幕弹窗
 * ============================================================ */
static QueueHandle_t s_done_q = NULL;

static void timer_ui_done_cb(void)   /* 服务任务上下文, 只能入队 */
{
    if (s_done_q) {
        uint8_t ev = 1;
        xQueueSend(s_done_q, &ev, 0);
    }
}

static void popup_close_cb(lv_event_t *e)
{
    lv_obj_t *mask = lv_event_get_user_data(e);
    if (mask) {
        lv_obj_del(mask);   /* 删掉整层遮罩(含弹窗框) */
    }
}

static void timer_ui_global_timer_cb(lv_timer_t *timer)
{
    uint8_t ev;
    if (xQueueReceive(s_done_q, &ev, 0) != pdTRUE) {
        return;
    }
    /* 在当前屏幕(lv_scr_act)弹窗 —— 不管用户在哪个界面 */
    lv_obj_t *scr = lv_scr_act();

    lv_obj_t *mask = lv_obj_create(scr);
    lv_obj_set_size(mask, lv_obj_get_width(scr), lv_obj_get_height(scr));
    lv_obj_center(mask);
    lv_obj_clear_flag(mask, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(mask, LV_OBJ_FLAG_CLICKABLE);       /* 挡后面点击 */
    lv_obj_set_style_bg_color(mask, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(mask, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *box = lv_obj_create(mask);
    lv_obj_set_size(box, 180, 96);
    lv_obj_center(box);
    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(box, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(box, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(box, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *title = lv_label_create(box);
    lv_label_set_text(title, "时间到");
    lv_obj_set_style_text_font(title, &lv_font_sourcesans_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(title, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 14);

    lv_obj_t *btn = lv_btn_create(box);
    lv_obj_set_size(btn, 80, 30);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -8);
    lv_obj_add_event_cb(btn, popup_close_cb, LV_EVENT_CLICKED, mask);
    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, "确定");
    lv_obj_set_style_text_font(lbl, &lv_font_sourcesans_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(lbl);
}

void timer_ui_global_init(void)
{
    if (s_done_q) {
        return;
    }
    s_done_q = xQueueCreate(4, 1);
    timer_cd_set_done_cb(timer_ui_done_cb);
    lv_timer_create(timer_ui_global_timer_cb, 200, NULL);   /* 常驻, 任意屏幕都能弹 */
}

/* ============================================================
 * 工具
 * ============================================================ */
static void set_visible(lv_obj_t *obj, bool vis)
{
    if (!obj) return;
    if (vis) lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    else     lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

/* 指示图标: on=原色, off=变灰+降透明度 */
static void set_indicator(lv_obj_t *icon, bool on)
{
    if (!icon) return;
    if (on) {
        lv_obj_set_style_img_recolor_opa(icon, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_opa(icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        lv_obj_set_style_img_recolor(icon, lv_color_hex(0x999999), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_opa(icon, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

/* 滚轮锁定: 运行/暂停时禁止改动 */
static void lock_roller(lv_obj_t *r, bool lock)
{
    if (!r) return;
    if (lock) {
        lv_obj_clear_flag(r, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    } else {
        lv_obj_add_flag(r, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    }
}

/* ============================================================
 * 计时器滚轮(3 列, 每列显示 3 行)
 * ============================================================ */
static void roller_build_options(char *buf, size_t n, int max)
{
    int len = 0;
    for (int i = 0; i <= max; i++) {
        len += snprintf(buf + len, n - len, "%s%02d", i ? "\n" : "", i);
        if (len >= (int)n - 4) break;
    }
}

static lv_obj_t *make_roller(lv_obj_t *parent, const char *opts, int x)
{
    lv_obj_t *r = lv_roller_create(parent);
    lv_roller_set_options(r, opts, LV_ROLLER_MODE_NORMAL);
    /* 先设字体, 再定可见行数(行高按字体算), 不要覆盖高度——否则高度和行不匹配,
       选中高亮会跨到上下行、滚轮吸附不干净。 */
    lv_obj_set_style_text_font(r, &lv_font_montserratMedium_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(r, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(r, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(r, 48);
    lv_roller_set_visible_row_count(r, 3);
    lv_obj_set_pos(r, x, 38);
    lv_obj_set_scrollbar_mode(r, LV_SCROLLBAR_MODE_OFF);

    /* 选中行: 半透明蓝底 + 黑字, 柔和不遮上下行 */
    lv_obj_set_style_bg_color(r, lv_color_hex(0x2195f6), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(r, LV_OPA_30, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(r, lv_color_black(), LV_PART_SELECTED | LV_STATE_DEFAULT);
    return r;
}

static void create_rollers(lv_obj_t *parent)
{
    static char opts_h[128], opts_ms[256];
    roller_build_options(opts_h,  sizeof(opts_h),  23);
    roller_build_options(opts_ms, sizeof(opts_ms), 59);

    s_roller_h = make_roller(parent, opts_h,  18);   /* 时 00-23 */
    s_roller_m = make_roller(parent, opts_ms, 90);   /* 分 00-59 */
    s_roller_s = make_roller(parent, opts_ms, 162);  /* 秒 00-59 */
}

/* ============================================================
 * 按钮回调
 * ============================================================ */
static void timer_start_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    timer_cd_state_t st = timer_cd_get_state();
    if (st == TIMER_CD_IDLE || st == TIMER_CD_SET) {
        int h = lv_roller_get_selected(s_roller_h);
        int m = lv_roller_get_selected(s_roller_m);
        int s = lv_roller_get_selected(s_roller_s);
        timer_cd_set(h, m, s);
    }
    timer_cd_start();
}

static void timer_pause_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    timer_cd_pause();
}

static void timer_cancel_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    timer_cd_cancel();
}

static void sw_start_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    timer_sw_start();
}

static void sw_pause_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    timer_sw_pause();
}

static void sw_reset_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    timer_sw_reset();
}

/* ============================================================
 * 同步: 按钮状态(总是) / 显示(仅当前页)
 * ============================================================ */
static void sync_timer_buttons(void)
{
    timer_cd_state_t st = timer_cd_get_state();
    if (st == s_last_cd_state) return;
    s_last_cd_state = st;

    bool running = (st == TIMER_CD_RUNNING);
    set_visible(s_t_start, !running);
    set_visible(s_t_pause,  running);
    lock_roller(s_roller_h, st == TIMER_CD_RUNNING || st == TIMER_CD_PAUSED);
    lock_roller(s_roller_m, st == TIMER_CD_RUNNING || st == TIMER_CD_PAUSED);
    lock_roller(s_roller_s, st == TIMER_CD_RUNNING || st == TIMER_CD_PAUSED);
}

static void sync_timer_display(void)
{
    uint32_t rem = timer_cd_get_remaining_ms();
    uint32_t rem_sec = rem / 1000;
    if (rem_sec == s_last_rem_sec) return;
    s_last_rem_sec = rem_sec;

    lv_roller_set_selected(s_roller_h, rem / 3600000,        LV_ANIM_OFF);
    lv_roller_set_selected(s_roller_m, (rem / 60000) % 60,   LV_ANIM_OFF);
    lv_roller_set_selected(s_roller_s, (rem / 1000) % 60,    LV_ANIM_OFF);
}

static void sync_sw_buttons(void)
{
    timer_sw_state_t st = timer_sw_get_state();
    if (st == s_last_sw_state) return;
    s_last_sw_state = st;
    bool running = (st == TIMER_SW_RUNNING);
    set_visible(s_sw_start, !running);
    set_visible(s_sw_pause,  running);
}

static void sync_sw_display(void)
{
    uint32_t ms = timer_sw_get_elapsed_ms();
    if ((int)ms == s_last_sw_ms) return;
    s_last_sw_ms = (int)ms;

    uint32_t mm = ms / 60000, ss = (ms / 1000) % 60, cc = (ms / 10) % 100;
    lv_label_set_text_fmt(s_sw_label, "%02lu:%02lu.%02lu",
                          (unsigned long)mm, (unsigned long)ss, (unsigned long)cc);
}

static void sync_indicators(void)
{
    lv_obj_t *act = lv_carousel_get_element_act(s_carousel);
    int timer_page = (act == s_el1);
    if (timer_page == s_last_page) return;
    s_last_page = timer_page;
    set_indicator(s_icon_timer, timer_page);
    set_indicator(s_icon_sw,    !timer_page);
}

/* ============================================================
 * 刷新定时器(10ms: 秒表百分秒)
 * ============================================================ */
static void timer_ui_cleanup(void);

static void timer_ui_refresh_cb(lv_timer_t *timer)
{
    /* 离开判定: lv_scr_act() 在淡入开始前仍是旧屏, 带 500ms 缓冲防误判 */
    if (!lv_obj_is_valid(guider_ui.screen_5) || lv_scr_act() != guider_ui.screen_5) {
        if (s_leave_us == 0) {
            s_leave_us = esp_timer_get_time();
        }
        if (esp_timer_get_time() - s_leave_us > 500000) {
            timer_ui_cleanup();
        }
        return;
    }
    s_leave_us = 0;

    sync_indicators();
    sync_timer_buttons();
    sync_sw_buttons();

    /* 显示: 当前页才刷 */
    lv_obj_t *act = lv_carousel_get_element_act(s_carousel);
    if (act == s_el1) {
        sync_timer_display();
    } else {
        sync_sw_display();
    }
}

/* ============================================================
 * 清理(离开 screen5): 服务不停, 只清 UI
 * ============================================================ */
static void timer_ui_cleanup(void)
{
    if (s_timer) { lv_timer_del(s_timer); s_timer = NULL; }
    s_carousel = s_el1 = s_el2 = NULL;
    s_roller_h = s_roller_m = s_roller_s = NULL;
    s_t_start = s_t_pause = s_t_cancel = NULL;
    s_sw_label = s_sw_start = s_sw_pause = s_sw_reset = NULL;
    s_icon_timer = s_icon_sw = NULL;
}

static void timer_ui_delete_cb(lv_event_t *e)
{
    timer_ui_cleanup();
}

/* 给绑定的子控件挂删除回调: 离开页面时 lv_obj_clean(screen_5) 会立刻释放所有子控件
   (早于 screen_5 自身的删除), 任一子控件被释放就停刷新定时器, 防访问悬空指针崩溃。 */
static void attach_widget_delete_cb(lv_obj_t *obj)
{
    if (obj) lv_obj_add_event_cb(obj, timer_ui_delete_cb, LV_EVENT_DELETE, NULL);
}

/* ============================================================
 * 入口: 进 screen5 时由 events_init_screen_5 调用
 * ============================================================ */
void timer_ui_screen_created(lv_ui *ui)
{
    s_carousel = ui->screen_5_carousel_1;
    s_el1 = ui->screen_5_carousel_1_element_1;
    s_el2 = ui->screen_5_carousel_1_element_2;

    s_t_start  = ui->screen_5_img_3;   /* 开始(设计图里 img_3 是开始图标) */
    s_t_pause  = ui->screen_5_img_4;   /* 暂停(设计图里 img_4 是暂停图标) */
    s_t_cancel = ui->screen_5_img_10;  /* 取消 */

    s_sw_label = ui->screen_5_label_10;
    s_sw_start = ui->screen_5_img_8;   /* 开始(设计图里 img_8 是开始图标) */
    s_sw_pause = ui->screen_5_img_9;   /* 暂停(设计图里 img_9 是暂停图标) */
    s_sw_reset = ui->screen_5_img_7;

    s_icon_timer = ui->screen_5_img_1;
    s_icon_sw    = ui->screen_5_img_6;

    /* 隐藏静态 3×3 数字格, 用滚轮替代 */
    lv_obj_add_flag(ui->screen_5_label_1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_label_2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_label_3, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_label_4, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_label_5, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_label_6, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_label_7, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_label_8, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_label_9, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_cont_1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_cont_2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_5_cont_3, LV_OBJ_FLAG_HIDDEN);
    create_rollers(ui->screen_5_carousel_1_element_1);

    /* 绑定按钮 */
    lv_obj_add_event_cb(s_t_start,  timer_start_cb,  LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_t_pause,  timer_pause_cb,  LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_t_cancel, timer_cancel_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_sw_start, sw_start_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_sw_pause, sw_pause_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_sw_reset, sw_reset_cb, LV_EVENT_CLICKED, NULL);

    /* 给所有引用控件挂删除回调(离开页面被 lv_obj_clean 释放时立刻停刷新定时器) */
    attach_widget_delete_cb(s_carousel);
    attach_widget_delete_cb(s_sw_label);
    attach_widget_delete_cb(s_roller_h);
    attach_widget_delete_cb(s_roller_m);
    attach_widget_delete_cb(s_roller_s);
    attach_widget_delete_cb(s_t_start);
    attach_widget_delete_cb(s_t_pause);
    attach_widget_delete_cb(s_t_cancel);
    attach_widget_delete_cb(s_sw_start);
    attach_widget_delete_cb(s_sw_pause);
    attach_widget_delete_cb(s_sw_reset);
    attach_widget_delete_cb(s_icon_timer);
    attach_widget_delete_cb(s_icon_sw);

    /* 去重缓存重置(新页面第一次同步必须强制刷) */
    s_last_cd_state = (timer_cd_state_t)-1;
    s_last_sw_state = (timer_sw_state_t)-1;
    s_last_rem_sec  = 0xFFFFFFFF;
    s_last_sw_ms    = -1;
    s_last_page     = -1;
    s_leave_us      = 0;

    /* 立即同步一次(设初始按钮/显示, 避免进页闪一下) */
    sync_timer_buttons();
    sync_sw_buttons();
    sync_indicators();
    sync_timer_display();
    sync_sw_display();

    /* 刷新定时器(10ms) */
    s_timer = lv_timer_create(timer_ui_refresh_cb, 10, NULL);

    /* 离开 screen5 时清理(服务不停) */
    lv_obj_add_event_cb(ui->screen_5, timer_ui_delete_cb, LV_EVENT_DELETE, NULL);
}

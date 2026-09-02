#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifista.h"
#include "rtc_service.h"
#include "fonts.h"          /* lv_font_sourcesans_16: GB2312 一级中文字体(弹窗/同步结果用) */
#include "settings_ui.h"

static const char *TAG = "settings";

/* ============================================================
 * WiFi 网络表: 加新 WiFi 只需在这加一行(会动态生成列表条目)。
 * ============================================================ */
typedef struct {
    const char *ssid;
    const char *pwd;
} wifi_net_t;

static const wifi_net_t s_wifi_nets[] = {
    { "mywifi",          "fxc520146" },
    { "ChinaNet-MNVNZ7", "18913247625" },
};
#define WIFI_NET_CNT (sizeof(s_wifi_nets) / sizeof(s_wifi_nets[0]))

#define WIFI_CONNECT_TIMEOUT_MS   15000   /* 连不上就弹"连接失败" */
#define WIFI_POPUP_DISMISS_MS     2000    /* 成功/失败弹窗停留后自动关 */
#define WIFI_ENTRY_X              18
#define WIFI_ENTRY_Y0             59
#define WIFI_ENTRY_STEP           68
#define WIFI_ENTRY_W              204
#define WIFI_ENTRY_H              51

/* WiFi 连接状态机 */
typedef enum { WIFI_ST_IDLE, WIFI_ST_CONNECTING, WIFI_ST_CONNECTED, WIFI_ST_FAILED } wifi_state_t;
/* 时间同步状态 */
typedef enum { SYNC_IDLE, SYNC_RUNNING, SYNC_OK, SYNC_FAIL } sync_state_t;

/* 绑定的 GUI Guider 控件 */
static lv_obj_t *s_page_wifi = NULL;   /* cont_4 */
static lv_obj_t *s_cont_sync = NULL;   /* cont_9 */
static lv_obj_t *s_lbl_sync  = NULL;   /* label_9 */
static lv_obj_t *s_spinner   = NULL;   /* spinner_1 */
static lv_obj_t *s_entries[WIFI_NET_CNT];   /* 动态 WiFi 条目 */

/* WiFi 状态 */
static wifi_state_t s_wifi_state = WIFI_ST_IDLE;
static int64_t      s_state_us   = 0;   /* 进入当前状态的时刻 */
static int64_t      s_dismiss_us = 0;   /* 弹窗自动关闭时刻 */

/* 弹窗 */
static lv_obj_t *s_popup_mask = NULL;
static lv_obj_t *s_popup_label = NULL;
static lv_obj_t *s_popup_spinner = NULL;

/* 时间同步 */
static sync_state_t s_sync_state = SYNC_IDLE;
static char s_sync_msg[40] = "";

static void wifi_popup_show(const char *msg);
static void wifi_popup_set_text(const char *msg);
static void wifi_popup_hide(void);

/* ============================================================
 * 防悬空: screen10 删除时清所有指针(弹窗单独处理)
 * ============================================================ */
static void settings_ui_cleanup(void)
{
    s_page_wifi = NULL;
    s_cont_sync = NULL;
    s_lbl_sync = NULL;
    s_spinner = NULL;
    for (int i = 0; i < WIFI_NET_CNT; i++) s_entries[i] = NULL;
    s_wifi_state = WIFI_ST_IDLE;
}

static void settings_ui_delete_cb(lv_event_t *e)
{
    settings_ui_cleanup();
}

/* 弹窗删除只清弹窗指针(不影响其他绑定) */
static void popup_delete_cb(lv_event_t *e)
{
    s_popup_mask = NULL;
    s_popup_label = NULL;
    s_popup_spinner = NULL;
}

static void attach_del(lv_obj_t *obj)
{
    if (obj) lv_obj_add_event_cb(obj, settings_ui_delete_cb, LV_EVENT_DELETE, NULL);
}

/* ============================================================
 * WiFi 连接弹窗(挂在当前活动屏幕上, 离开 screen10 随之销毁)
 * ============================================================ */
static void wifi_popup_show(const char *msg)
{
    if (s_popup_mask && lv_obj_is_valid(s_popup_mask)) {
        lv_label_set_text(s_popup_label, msg);
        return;
    }
    lv_obj_t *scr = lv_scr_act();
    if (!scr) return;

    s_popup_mask = lv_obj_create(scr);
    lv_obj_remove_style_all(s_popup_mask);
    lv_obj_set_size(s_popup_mask, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_opa(s_popup_mask, LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_popup_mask, lv_color_black(), LV_PART_MAIN);
    lv_obj_clear_flag(s_popup_mask, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(s_popup_mask);
    lv_obj_add_event_cb(s_popup_mask, popup_delete_cb, LV_EVENT_DELETE, NULL);

    lv_obj_t *box = lv_obj_create(s_popup_mask);
    lv_obj_set_size(box, 170, 96);
    lv_obj_set_style_bg_color(box, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_radius(box, 14, LV_PART_MAIN);
    lv_obj_center(box);

    s_popup_spinner = lv_spinner_create(box, 900, 60);
    lv_obj_set_size(s_popup_spinner, 36, 36);
    lv_obj_align(s_popup_spinner, LV_ALIGN_TOP_MID, 0, 14);

    s_popup_label = lv_label_create(box);
    lv_label_set_text(s_popup_label, msg);
    lv_obj_set_style_text_color(s_popup_label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(s_popup_label, &lv_font_sourcesans_16, LV_PART_MAIN);
    lv_obj_align(s_popup_label, LV_ALIGN_BOTTOM_MID, 0, -16);
}

static void wifi_popup_set_text(const char *msg)
{
    if (s_popup_label && lv_obj_is_valid(s_popup_label)) {
        lv_label_set_text(s_popup_label, msg);
    }
    /* 出结果时停掉转圈 */
    if (s_popup_spinner && lv_obj_is_valid(s_popup_spinner)) {
        lv_obj_add_flag(s_popup_spinner, LV_OBJ_FLAG_HIDDEN);
    }
}

static void wifi_popup_hide(void)
{
    if (s_popup_mask && lv_obj_is_valid(s_popup_mask)) {
        lv_obj_del(s_popup_mask);   /* 删除会触发 popup_delete_cb 清指针 */
    }
}

/* ============================================================
 * 动态 WiFi 条目 + 点击连网
 * ============================================================ */
static void wifi_entry_click_cb(lv_event_t *e)
{
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    if (idx < 0 || idx >= WIFI_NET_CNT) return;

    esp_err_t r = wifista_connect(s_wifi_nets[idx].ssid, s_wifi_nets[idx].pwd);
    s_state_us = esp_timer_get_time();
    if (r == ESP_OK) {
        s_wifi_state = WIFI_ST_CONNECTING;
        wifi_popup_show("连接中...");
    } else {
        s_wifi_state = WIFI_ST_FAILED;
        wifi_popup_show("连接失败");
        s_dismiss_us = esp_timer_get_time() + WIFI_POPUP_DISMISS_MS * 1000;
    }
}

static void wifi_entry_create(int idx, lv_obj_t *parent)
{
    lv_obj_t *e = lv_obj_create(parent);
    lv_obj_set_size(e, WIFI_ENTRY_W, WIFI_ENTRY_H);
    lv_obj_set_pos(e, WIFI_ENTRY_X, WIFI_ENTRY_Y0 + idx * WIFI_ENTRY_STEP);
    lv_obj_set_style_border_width(e, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(e, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_radius(e, 20, LV_PART_MAIN);
    lv_obj_set_style_bg_color(e, lv_color_white(), LV_PART_MAIN);
    lv_obj_clear_flag(e, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl = lv_label_create(e);
    lv_label_set_text(lbl, s_wifi_nets[idx].ssid);
    lv_obj_set_pos(lbl, 30, 11);
    lv_obj_set_style_text_color(lbl, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(lbl, &lv_font_montserratMedium_20, LV_PART_MAIN);

    lv_obj_add_event_cb(e, wifi_entry_click_cb, LV_EVENT_CLICKED, (void *)(intptr_t)idx);
    /* 关键: 离开 screen10 时 lv_obj_clean() 只删子控件不触发屏幕 DELETE,
       必须挂在条目本身上, 任一被释放就清全部指针, 防 refresh 访问悬空 */
    attach_del(e);
    s_entries[idx] = e;
}

/* 连接状态机推进 + 高亮当前网络 */
static void wifi_conn_update(void)
{
    int64_t now = esp_timer_get_time();

    /* 状态转移 */
    if (s_wifi_state == WIFI_ST_CONNECTING) {
        if (wifista_is_connected()) {
            s_wifi_state = WIFI_ST_CONNECTED;
            wifi_popup_set_text("连接成功");
            s_dismiss_us = now + WIFI_POPUP_DISMISS_MS * 1000;
        } else if (now - s_state_us > WIFI_CONNECT_TIMEOUT_MS * 1000) {
            s_wifi_state = WIFI_ST_FAILED;
            wifi_popup_set_text("连接失败");
            s_dismiss_us = now + WIFI_POPUP_DISMISS_MS * 1000;
        }
    }
    /* 成功/失败弹窗到时自动关 */
    if ((s_wifi_state == WIFI_ST_CONNECTED || s_wifi_state == WIFI_ST_FAILED)
        && now > s_dismiss_us) {
        wifi_popup_hide();
        s_wifi_state = WIFI_ST_IDLE;
    }

    /* 高亮当前已连网络(边框变绿) */
    char cur[33] = {0};
    bool conn = wifista_is_connected();
    wifista_get_current_ssid(cur, sizeof(cur));
    for (int i = 0; i < WIFI_NET_CNT; i++) {
        if (!s_entries[i]) continue;
        bool active = conn && strcmp(cur, s_wifi_nets[i].ssid) == 0;
        lv_obj_set_style_border_color(s_entries[i],
                                      active ? lv_color_hex(0x00f71c) : lv_color_hex(0xffffff),
                                      LV_PART_MAIN);
    }
}

/* ============================================================
 * 时间校准: 手动同步(独立任务, 不卡 LVGL 转圈)
 * ============================================================ */
static void sync_task(void *arg)
{
    if (!wifista_is_connected()) {
        snprintf(s_sync_msg, sizeof(s_sync_msg), "请先连接WiFi");
        s_sync_state = SYNC_FAIL;
    } else {
        esp_err_t r = rtc_service_sync_from_ntp(15000);
        if (r == ESP_OK) {
            pcf8563_time_t t;
            if (rtc_service_get_time(&t) == ESP_OK) {
                snprintf(s_sync_msg, sizeof(s_sync_msg), "同步成功 %02d:%02d",
                         t.hour, t.min);
            } else {
                snprintf(s_sync_msg, sizeof(s_sync_msg), "同步成功");
            }
            s_sync_state = SYNC_OK;
        } else {
            snprintf(s_sync_msg, sizeof(s_sync_msg), "同步失败");
            s_sync_state = SYNC_FAIL;
        }
    }
    vTaskDelete(NULL);
}

static int64_t s_sync_done_us = 0;   /* 结果开始显示的时刻, 2s 后自动关校准页 */

static void settings_sync_click_cb(lv_event_t *e)
{
    if (s_sync_state == SYNC_RUNNING) return;   /* 已在同步 */
    s_sync_state = SYNC_RUNNING;
    s_sync_done_us = 0;
    snprintf(s_sync_msg, sizeof(s_sync_msg), "同步中...");
    xTaskCreate(sync_task, "time_sync", 4096, NULL, 5, NULL);
}

static void sync_display_update(void)
{
    if (!s_cont_sync || !lv_obj_is_valid(s_cont_sync)) return;

    if (s_sync_state == SYNC_RUNNING) {
        if (lv_obj_is_valid(s_spinner)) lv_obj_clear_flag(s_spinner, LV_OBJ_FLAG_HIDDEN);
        if (lv_obj_is_valid(s_lbl_sync)) lv_label_set_text(s_lbl_sync, s_sync_msg);
    } else if (s_sync_state == SYNC_OK || s_sync_state == SYNC_FAIL) {
        if (s_sync_done_us == 0) s_sync_done_us = esp_timer_get_time();
        if (lv_obj_is_valid(s_spinner)) lv_obj_add_flag(s_spinner, LV_OBJ_FLAG_HIDDEN);
        if (lv_obj_is_valid(s_lbl_sync)) lv_label_set_text(s_lbl_sync, s_sync_msg);
        /* 结果显示 2s 后自动关校准页(回设置菜单) */
        if (esp_timer_get_time() - s_sync_done_us > 2000 * 1000) {
            lv_obj_add_flag(s_cont_sync, LV_OBJ_FLAG_HIDDEN);
            s_sync_state = SYNC_IDLE;
            s_sync_done_us = 0;
        }
    }
}

/* ============================================================
 * 公开 API
 * ============================================================ */
void settings_ui_refresh(void)
{
    if (!s_page_wifi) return;   /* 未进过 / 已离开 screen10 */

    wifi_conn_update();
    sync_display_update();
}

void settings_ui_screen_created(lv_ui *ui)
{
    s_page_wifi = ui->screen_10_cont_4;
    s_cont_sync = ui->screen_10_cont_9;
    s_lbl_sync  = ui->screen_10_label_9;
    s_spinner   = ui->screen_10_spinner_1;

    /* 隐藏 GUI Guider 的静态 WiFi 条目(cont_5/6/7), 改动态列表(加 WiFi 只改代码表) */
    lv_obj_add_flag(ui->screen_10_cont_5, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_10_cont_6, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_10_cont_7, LV_OBJ_FLAG_HIDDEN);

    for (int i = 0; i < WIFI_NET_CNT; i++) {
        wifi_entry_create(i, s_page_wifi);
    }

    /* 时间校准: 用户逻辑负责显示 cont_9, 这里额外挂回调触发手动同步 */
    lv_obj_add_event_cb(ui->screen_10_cont_3, settings_sync_click_cb, LV_EVENT_CLICKED, NULL);

    /* 状态复位(重新进页面从干净状态开始) */
    s_wifi_state = WIFI_ST_IDLE;
    s_sync_state = SYNC_IDLE;
    s_sync_done_us = 0;
    s_popup_mask = NULL;   /* 离开时弹窗已随 screen10 销毁 */
    s_popup_label = NULL;
    s_popup_spinner = NULL;

    /* 防悬空: 挂到绑定的 GUI Guider 控件 + 动态条目上(条目已各自 attach)。
       lv_obj_clean(screen10) 只删子控件, 任一被释放就清指针停刷新 */
    attach_del(ui->screen_10_cont_4);
    attach_del(ui->screen_10_cont_9);
    attach_del(ui->screen_10_label_9);
    attach_del(ui->screen_10_spinner_1);

    /* 时间校准结果用 sourcesans_16 中文字体(重生成后已含 成功/失败/连接 等字) */
    lv_obj_set_style_text_font(ui->screen_10_label_9, &lv_font_sourcesans_16, LV_PART_MAIN);
    lv_label_set_text(ui->screen_10_label_9, "点按同步");

    settings_ui_refresh();
}

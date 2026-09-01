#include <stdio.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "axp2101.h"
#include "ui_battery.h"

static const char *TAG = "battery";

/* 变化检测: 电量/充电状态只在变化时刷, 避免每秒无谓重绘 */
static int  s_last_pct = -1;
static bool s_last_chg = false;
static bool s_last_present = true;
static int64_t s_last_log_us = 0;
static lv_obj_t *s_scr_bar = NULL, *s_scr_lbl = NULL, *s_scr_img = NULL;  /* 上次更新过的控件指针 */

void ui_battery_refresh(void)
{
    /* 滑走 screen2 会删除、返回会重建 → GUI Guider 静态值(如 label "30%")重新出现。
       用控件指针变化检测重建, 强制重新应用一次真实电量, 否则变化检测以为没变就漏刷。 */
    lv_obj_t *bar = guider_ui.screen_2_bar_1;
    lv_obj_t *lbl = guider_ui.screen_2_label_3;
    lv_obj_t *img = guider_ui.screen_2_img_10;
    if (bar != s_scr_bar || lbl != s_scr_lbl || img != s_scr_img) {
        s_scr_bar = bar;
        s_scr_lbl = lbl;
        s_scr_img = img;
        s_last_pct = -1;         /* 强制重写 */
        s_last_chg = false;
        s_last_present = true;
    }

    bool present = axp2101_get_battery_present();

    /* 电池不在位(仅 USB 调试) → 显示 0%, 隐藏充电图标 */
    if (!present) {
        if (s_last_present) {
            if (lv_obj_is_valid(guider_ui.screen_2_bar_1)) {
                lv_bar_set_value(guider_ui.screen_2_bar_1, 0, LV_ANIM_OFF);
            }
            if (lv_obj_is_valid(guider_ui.screen_2_label_3)) {
                lv_label_set_text(guider_ui.screen_2_label_3, "0%");
            }
            if (lv_obj_is_valid(guider_ui.screen_2_img_10)) {
                lv_obj_add_flag(guider_ui.screen_2_img_10, LV_OBJ_FLAG_HIDDEN);
            }
            s_last_present = false;
            s_last_pct = -1;
            s_last_chg = false;
        }
        return;
    }
    s_last_present = true;

    int pct = axp2101_get_battery_percent();
    if (pct < 0) {
        return;   /* I2C 读失败, 下轮再试 */
    }
    if (pct > 100) pct = 100;

    /* 百分比变化才刷 bar + 数字 */
    if (pct != s_last_pct) {
        s_last_pct = pct;
        if (lv_obj_is_valid(guider_ui.screen_2_bar_1)) {
            lv_bar_set_value(guider_ui.screen_2_bar_1, pct, LV_ANIM_OFF);
        }
        if (lv_obj_is_valid(guider_ui.screen_2_label_3)) {
            lv_label_set_text_fmt(guider_ui.screen_2_label_3, "%d%%", pct);
        }
    }

    /* 充电状态变化才切图标 */
    bool chg = axp2101_get_charging();
    if (chg != s_last_chg) {
        s_last_chg = chg;
        if (lv_obj_is_valid(guider_ui.screen_2_img_10)) {
            if (chg) lv_obj_clear_flag(guider_ui.screen_2_img_10, LV_OBJ_FLAG_HIDDEN);
            else     lv_obj_add_flag(guider_ui.screen_2_img_10, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /* 调试: 每 60s 打一次 百分比+电压, 方便对照放电曲线验证燃料计准不准 */
    int64_t now_us = esp_timer_get_time();
    if (now_us - s_last_log_us > 60 * 1000 * 1000) {
        s_last_log_us = now_us;
        int mv = axp2101_get_battery_voltage();
        if (mv >= 0) {
            ESP_LOGI(TAG, "percent=%d%% voltage=%d.%03dV %s",
                     pct, mv / 1000, mv % 1000, chg ? "CHARGING" : "");
        }
    }
}

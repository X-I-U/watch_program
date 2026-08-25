/**
 * calc_ui.c —— 计算器 UI 适配层
 *
 * 按键识别策略：不硬编码 screen_7_cont_N 的序号，而是遍历 screen_7
 * 的子对象按内容识别——
 *   子对象里有 Label 且文本是单个字符  -> 该字符就是按键（0-9 . + - * / % C =）
 *   子对象里有 Image                 -> 删除键（backspace）
 * 这样 GUI Guider 重导出改控件命名/顺序也不容易错。
 *
 * 线程安全：本文件的 s_calc 只在 LVGL 任务上下文（事件回调）里被访问，
 * 天然单线程，不需要额外加锁。
 */

#include "calc_ui.h"

#include <stdint.h>

#include "calc_core.h"

static calc_t    s_calc;
static lv_obj_t *s_display_label;   /* 顶部显示 label */

/* LVGL 点击事件回调：按键 -> 核心逻辑 -> 刷新显示 */
static void calc_key_cb(lv_event_t *e)
{
    char key = (char)(intptr_t)lv_event_get_user_data(e);
    calc_input(&s_calc, key);
    lv_label_set_text(s_display_label, s_calc.display);
}

void calc_ui_screen_created(lv_ui *ui)
{
    calc_init(&s_calc);
    s_display_label = ui->screen_7_label_20;

    /* 顶部显示：超出省略号截断；右对齐（从最右侧开始）；初始显示 "0" */
    lv_label_set_long_mode(s_display_label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(s_display_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(s_display_label, s_calc.display);

    /* 遍历 screen_7 的直接子对象，找出按钮容器（跳过显示 label） */
    uint32_t n = lv_obj_get_child_cnt(ui->screen_7);
    for (uint32_t i = 0; i < n; i++) {
        lv_obj_t *btn = lv_obj_get_child(ui->screen_7, i);
        if (btn == s_display_label) continue;

        lv_obj_t *sub = lv_obj_get_child(btn, 0);
        if (sub == NULL) continue;

        char key;
        if (lv_obj_check_type(sub, &lv_img_class)) {
            key = CALC_KEY_BACKSPACE;           /* 删除图片键 */
        } else {
            const char *txt = lv_label_get_text(sub);
            if (txt == NULL || txt[0] == '\0' || txt[1] != '\0') continue;   /* 只要单字符键 */
            key = txt[0];
        }

        lv_obj_add_event_cb(btn, calc_key_cb, LV_EVENT_CLICKED, (void *)(intptr_t)key);
    }
}

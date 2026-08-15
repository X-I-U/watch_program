/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include "gui_guider.h"
#include "widgets_init.h"
#include <stdlib.h>
#include <string.h>


__attribute__((unused)) void kb_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);
    if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

__attribute__((unused)) void ta_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
#if LV_USE_KEYBOARD || LV_USE_ZH_KEYBOARD
    lv_obj_t *ta = lv_event_get_target(e);
#endif
    lv_obj_t *kb = lv_event_get_user_data(e);
    if (code == LV_EVENT_FOCUSED || code == LV_EVENT_CLICKED)
    {
#if LV_USE_ZH_KEYBOARD != 0
        lv_zh_keyboard_set_textarea(kb, ta);
#endif
#if LV_USE_KEYBOARD != 0
        lv_keyboard_set_textarea(kb, ta);
#endif
        lv_obj_move_foreground(kb);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    if (code == LV_EVENT_CANCEL || code == LV_EVENT_DEFOCUSED)
    {

#if LV_USE_ZH_KEYBOARD != 0
        lv_zh_keyboard_set_textarea(kb, ta);
#endif
#if LV_USE_KEYBOARD != 0
        lv_keyboard_set_textarea(kb, ta);
#endif
        lv_obj_move_background(kb);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

#if LV_USE_ANALOGCLOCK != 0
void clock_count(int *hour, int *min, int *sec)
{
    (*sec)++;
    if(*sec == 60)
    {
        *sec = 0;
        (*min)++;
    }
    if(*min == 60)
    {
        *min = 0;
        if(*hour < 12)
        {
            (*hour)++;
        } else {
            (*hour)++;
            *hour = *hour %12;
        }
    }
}
#endif


const lv_img_dsc_t * screen_2_animimg_1_imgs[29] = {
    &screen_2_animimg_11,
    &screen_2_animimg_12,
    &screen_2_animimg_13,
    &screen_2_animimg_15,
    &screen_2_animimg_18,
    &screen_2_animimg_111,
    &screen_2_animimg_113,
    &screen_2_animimg_115,
    &screen_2_animimg_117,
    &screen_2_animimg_119,
    &screen_2_animimg_121,
    &screen_2_animimg_123,
    &screen_2_animimg_125,
    &screen_2_animimg_127,
    &screen_2_animimg_129,
    &screen_2_animimg_131,
    &screen_2_animimg_133,
    &screen_2_animimg_135,
    &screen_2_animimg_137,
    &screen_2_animimg_139,
    &screen_2_animimg_141,
    &screen_2_animimg_144,
    &screen_2_animimg_146,
    &screen_2_animimg_148,
    &screen_2_animimg_150,
    &screen_2_animimg_152,
    &screen_2_animimg_154,
    &screen_2_animimg_156,
    &screen_2_animimg_158,
};
extern int screen_2_digital_clock_1_hour_value;
extern int screen_2_digital_clock_1_min_value;
extern int screen_2_digital_clock_1_sec_value;
extern char screen_2_digital_clock_1_meridiem[];

void screen_2_digital_clock_1_timer(lv_timer_t *timer)
{
    clock_count_12(&screen_2_digital_clock_1_hour_value, &screen_2_digital_clock_1_min_value, &screen_2_digital_clock_1_sec_value, screen_2_digital_clock_1_meridiem);
    if (lv_obj_is_valid(guider_ui.screen_2_digital_clock_1))
    {
        lv_dclock_set_text_fmt(guider_ui.screen_2_digital_clock_1, "%d:%02d:%02d %s", screen_2_digital_clock_1_hour_value, screen_2_digital_clock_1_min_value, screen_2_digital_clock_1_sec_value, screen_2_digital_clock_1_meridiem);
    }
}


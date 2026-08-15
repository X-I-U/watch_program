/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *screen_2;
	bool screen_2_del;
	lv_obj_t *screen_2_tileview_1;
	lv_obj_t *screen_2_tileview_1_tile;
	lv_obj_t *screen_2_cont_1;
	lv_obj_t *screen_2_cont_2;
	lv_obj_t *screen_2_cont_4;
	lv_obj_t *screen_2_cont_3;
	lv_obj_t *screen_2_cont_5;
	lv_obj_t *screen_2_cont_6;
	lv_obj_t *screen_2_animimg_1;
	lv_obj_t *screen_2_digital_clock_1;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, int32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_screen_2(lv_ui *ui);
#include "extra/widgets/animimg/lv_animimg.h"
LV_IMG_DECLARE(screen_2_animimg_11);
LV_IMG_DECLARE(screen_2_animimg_12);
LV_IMG_DECLARE(screen_2_animimg_13);
LV_IMG_DECLARE(screen_2_animimg_15);
LV_IMG_DECLARE(screen_2_animimg_18);
LV_IMG_DECLARE(screen_2_animimg_111);
LV_IMG_DECLARE(screen_2_animimg_113);
LV_IMG_DECLARE(screen_2_animimg_115);
LV_IMG_DECLARE(screen_2_animimg_117);
LV_IMG_DECLARE(screen_2_animimg_119);
LV_IMG_DECLARE(screen_2_animimg_121);
LV_IMG_DECLARE(screen_2_animimg_123);
LV_IMG_DECLARE(screen_2_animimg_125);
LV_IMG_DECLARE(screen_2_animimg_127);
LV_IMG_DECLARE(screen_2_animimg_129);
LV_IMG_DECLARE(screen_2_animimg_131);
LV_IMG_DECLARE(screen_2_animimg_133);
LV_IMG_DECLARE(screen_2_animimg_135);
LV_IMG_DECLARE(screen_2_animimg_137);
LV_IMG_DECLARE(screen_2_animimg_139);
LV_IMG_DECLARE(screen_2_animimg_141);
LV_IMG_DECLARE(screen_2_animimg_144);
LV_IMG_DECLARE(screen_2_animimg_146);
LV_IMG_DECLARE(screen_2_animimg_148);
LV_IMG_DECLARE(screen_2_animimg_150);
LV_IMG_DECLARE(screen_2_animimg_152);
LV_IMG_DECLARE(screen_2_animimg_154);
LV_IMG_DECLARE(screen_2_animimg_156);
LV_IMG_DECLARE(screen_2_animimg_158);

LV_FONT_DECLARE(lv_font_Alatsi_Regular_40)


#ifdef __cplusplus
}
#endif
#endif

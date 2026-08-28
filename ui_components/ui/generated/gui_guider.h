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
	lv_obj_t *screen_2_img_1;
	lv_obj_t *screen_2_cont_2;
	lv_obj_t *screen_2_img_5;
	lv_obj_t *screen_2_cont_4;
	lv_obj_t *screen_2_img_6;
	lv_obj_t *screen_2_cont_3;
	lv_obj_t *screen_2_img_2;
	lv_obj_t *screen_2_cont_5;
	lv_obj_t *screen_2_img_3;
	lv_obj_t *screen_2_cont_6;
	lv_obj_t *screen_2_img_4;
	lv_obj_t *screen_2_animimg_1;
	lv_obj_t *screen_2_datetext_1;
	lv_obj_t *screen_2_cont_7;
	lv_obj_t *screen_2_label_1;
	lv_obj_t *screen_2_cont_8;
	lv_obj_t *screen_2_label_2;
	lv_obj_t *screen_2_bar_1;
	lv_obj_t *screen_2_label_3;
	lv_obj_t *screen_2_img_7;
	lv_obj_t *screen_3;
	bool screen_3_del;
	lv_obj_t *screen_3_cont_1;
	lv_obj_t *screen_3_cont_2;
	lv_obj_t *screen_3_cont_3;
	lv_obj_t *screen_3_cont_4;
	lv_obj_t *screen_3_cont_5;
	lv_obj_t *screen_3_cont_6;
	lv_obj_t *screen_3_cont_7;
	lv_obj_t *screen_3_cont_8;
	lv_obj_t *screen_3_cont_9;
	lv_obj_t *screen_3_cont_17;
	lv_obj_t *screen_3_cont_16;
	lv_obj_t *screen_3_cont_15;
	lv_obj_t *screen_3_cont_14;
	lv_obj_t *screen_3_cont_13;
	lv_obj_t *screen_3_cont_12;
	lv_obj_t *screen_3_cont_11;
	lv_obj_t *screen_3_cont_10;
	lv_obj_t *screen_3_label_2;
	lv_obj_t *screen_3_label_1;
	lv_obj_t *screen_3_label_3;
	lv_obj_t *screen_3_img_1;
	lv_obj_t *screen_3_cont_18;
	lv_obj_t *screen_3_label_4;
	lv_obj_t *screen_4;
	bool screen_4_del;
	lv_obj_t *screen_4_carousel_1;
	lv_obj_t *screen_4_carousel_1_element_1;
	lv_obj_t *screen_4_carousel_1_element_2;
	lv_obj_t *screen_4_label_1;
	lv_obj_t *screen_4_label_2;
	lv_obj_t *screen_4_label_3;
	lv_obj_t *screen_4_label_4;
	lv_obj_t *screen_4_label_5;
	lv_obj_t *screen_4_label_6;
	lv_obj_t *screen_4_label_7;
	lv_obj_t *screen_4_label_39;
	lv_obj_t *screen_4_img_1;
	lv_obj_t *screen_4_label_40;
	lv_obj_t *screen_5;
	bool screen_5_del;
	lv_obj_t *screen_5_carousel_1;
	lv_obj_t *screen_5_carousel_1_element_1;
	lv_obj_t *screen_5_carousel_1_element_2;
	lv_obj_t *screen_5_cont_2;
	lv_obj_t *screen_5_label_2;
	lv_obj_t *screen_5_cont_3;
	lv_obj_t *screen_5_label_3;
	lv_obj_t *screen_5_cont_1;
	lv_obj_t *screen_5_img_3;
	lv_obj_t *screen_5_label_4;
	lv_obj_t *screen_5_label_5;
	lv_obj_t *screen_5_label_6;
	lv_obj_t *screen_5_label_7;
	lv_obj_t *screen_5_label_8;
	lv_obj_t *screen_5_label_9;
	lv_obj_t *screen_5_label_1;
	lv_obj_t *screen_5_img_4;
	lv_obj_t *screen_5_cont_4;
	lv_obj_t *screen_5_cont_5;
	lv_obj_t *screen_5_label_11;
	lv_obj_t *screen_5_label_12;
	lv_obj_t *screen_5_label_13;
	lv_obj_t *screen_5_label_14;
	lv_obj_t *screen_5_label_15;
	lv_obj_t *screen_5_tileview_1;
	lv_obj_t *screen_5_tileview_1_tile;
	lv_obj_t *screen_5_cont_6;
	lv_obj_t *screen_5_sw_1;
	lv_obj_t *screen_5_label_16;
	lv_obj_t *screen_5_cont_7;
	lv_obj_t *screen_5_sw_2;
	lv_obj_t *screen_5_label_17;
	lv_obj_t *screen_5_cont_8;
	lv_obj_t *screen_5_sw_3;
	lv_obj_t *screen_5_label_18;
	lv_obj_t *screen_5_btn_1;
	lv_obj_t *screen_5_btn_1_label;
	lv_obj_t *screen_5_label_10;
	lv_obj_t *screen_5_img_1;
	lv_obj_t *screen_5_img_2;
	lv_obj_t *screen_5_img_5;
	lv_obj_t *screen_6;
	bool screen_6_del;
	lv_obj_t *screen_6_img_2;
	lv_obj_t *screen_6_img_5;
	lv_obj_t *screen_6_img_7;
	lv_obj_t *screen_6_img_3;
	lv_obj_t *screen_6_img_4;
	lv_obj_t *screen_6_img_6;
	lv_obj_t *screen_6_img_8;
	lv_obj_t *screen_6_slider_1;
	lv_obj_t *screen_6_label_1;
	lv_obj_t *screen_6_tileview_1;
	lv_obj_t *screen_6_tileview_1_tile;
	lv_obj_t *screen_6_cont_1;
	lv_obj_t *screen_6_label_2;
	lv_obj_t *screen_6_cont_2;
	lv_obj_t *screen_6_label_3;
	lv_obj_t *screen_6_cont_3;
	lv_obj_t *screen_6_label_4;
	lv_obj_t *screen_6_cont_4;
	lv_obj_t *screen_6_label_5;
	lv_obj_t *screen_6_cont_5;
	lv_obj_t *screen_6_label_6;
	lv_obj_t *screen_6_cont_6;
	lv_obj_t *screen_6_label_7;
	lv_obj_t *screen_7;
	bool screen_7_del;
	lv_obj_t *screen_7_cont_1;
	lv_obj_t *screen_7_label_1;
	lv_obj_t *screen_7_cont_2;
	lv_obj_t *screen_7_label_2;
	lv_obj_t *screen_7_cont_3;
	lv_obj_t *screen_7_label_13;
	lv_obj_t *screen_7_cont_4;
	lv_obj_t *screen_7_label_5;
	lv_obj_t *screen_7_cont_5;
	lv_obj_t *screen_7_label_8;
	lv_obj_t *screen_7_cont_9;
	lv_obj_t *screen_7_label_11;
	lv_obj_t *screen_7_cont_7;
	lv_obj_t *screen_7_label_6;
	lv_obj_t *screen_7_cont_6;
	lv_obj_t *screen_7_label_3;
	lv_obj_t *screen_7_cont_13;
	lv_obj_t *screen_7_label_9;
	lv_obj_t *screen_7_cont_12;
	lv_obj_t *screen_7_label_14;
	lv_obj_t *screen_7_cont_11;
	lv_obj_t *screen_7_label_10;
	lv_obj_t *screen_7_cont_17;
	lv_obj_t *screen_7_label_7;
	lv_obj_t *screen_7_cont_16;
	lv_obj_t *screen_7_label_15;
	lv_obj_t *screen_7_cont_15;
	lv_obj_t *screen_7_label_12;
	lv_obj_t *screen_7_cont_21;
	lv_obj_t *screen_7_img_1;
	lv_obj_t *screen_7_cont_20;
	lv_obj_t *screen_7_label_16;
	lv_obj_t *screen_7_cont_22;
	lv_obj_t *screen_7_label_17;
	lv_obj_t *screen_7_cont_23;
	lv_obj_t *screen_7_label_18;
	lv_obj_t *screen_7_cont_24;
	lv_obj_t *screen_7_label_19;
	lv_obj_t *screen_7_label_20;
	lv_obj_t *screen_8;
	bool screen_8_del;
	lv_obj_t *screen_8_label_1;
	lv_obj_t *screen_8_label_2;
	lv_obj_t *screen_9;
	bool screen_9_del;
	lv_obj_t *screen_9_img_1;
	lv_obj_t *screen_9_label_1;
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
void setup_scr_screen_3(lv_ui *ui);
void setup_scr_screen_4(lv_ui *ui);
void setup_scr_screen_5(lv_ui *ui);
void setup_scr_screen_6(lv_ui *ui);
void setup_scr_screen_7(lv_ui *ui);
void setup_scr_screen_8(lv_ui *ui);
void setup_scr_screen_9(lv_ui *ui);
LV_IMG_DECLARE(_gaming_alpha_34x33);
LV_IMG_DECLARE(_tool_alpha_42x43);
LV_IMG_DECLARE(_ai_alpha_36x33);
LV_IMG_DECLARE(_calendar1_alpha_29x35);
LV_IMG_DECLARE(_music1_alpha_33x38);
LV_IMG_DECLARE(_clock2_alpha_36x33);
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
LV_IMG_DECLARE(_wifi_alpha_27x25);
LV_IMG_DECLARE(_back_alpha_45x40);
LV_IMG_DECLARE(_back_alpha_37x31);
LV_IMG_DECLARE(_stop_alpha_35x36);
LV_IMG_DECLARE(_start_alpha_47x35);
LV_IMG_DECLARE(_timer2_alpha_41x36);
LV_IMG_DECLARE(_clock_alpha_42x39);
LV_IMG_DECLARE(_back_alpha_37x34);
LV_IMG_DECLARE(_blank_alpha_51x51);
LV_IMG_DECLARE(_stop_alpha_33x32);
LV_IMG_DECLARE(_start_alpha_41x33);
LV_IMG_DECLARE(_right_alpha_44x38);
LV_IMG_DECLARE(_left_alpha_42x40);
LV_IMG_DECLARE(_menu_alpha_37x36);
LV_IMG_DECLARE(_JAY2_alpha_110x109);
LV_IMG_DECLARE(_delate_alpha_41x25);
LV_IMG_DECLARE(_xiaozhi_alpha_96x90);

LV_FONT_DECLARE(lv_font_montserratMedium_19)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Regular_20)
LV_FONT_DECLARE(lv_font_montserratMedium_25)
LV_FONT_DECLARE(lv_font_montserratMedium_10)
LV_FONT_DECLARE(lv_font_Acme_Regular_40)
LV_FONT_DECLARE(lv_font_Acme_Regular_25)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_20)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Regular_16)
LV_FONT_DECLARE(lv_font_Acme_Regular_30)
LV_FONT_DECLARE(lv_font_Acme_Regular_50)
LV_FONT_DECLARE(lv_font_Acme_Regular_35)


#ifdef __cplusplus
}
#endif
#endif

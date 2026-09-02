/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen_6(lv_ui *ui)
{
    //Write codes screen_6
    ui->screen_6 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_6, 240, 300);
    lv_obj_set_scrollbar_mode(ui->screen_6, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_6, lv_color_hex(0x173040), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_6, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_img_2
    ui->screen_6_img_2 = lv_img_create(ui->screen_6);
    lv_obj_add_flag(ui->screen_6_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_6_img_2, &_blank_alpha_51x51);
    lv_img_set_pivot(ui->screen_6_img_2, 50,50);
    lv_img_set_angle(ui->screen_6_img_2, 0);
    lv_obj_set_pos(ui->screen_6_img_2, 90, 214);
    lv_obj_set_size(ui->screen_6_img_2, 51, 51);

    //Write style for screen_6_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_6_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_6_img_2, lv_color_hex(0xb69393), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_6_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_6_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_img_5
    ui->screen_6_img_5 = lv_img_create(ui->screen_6);
    lv_obj_add_flag(ui->screen_6_img_5, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_6_img_5, &_stop_alpha_33x32);
    lv_img_set_pivot(ui->screen_6_img_5, 50,50);
    lv_img_set_angle(ui->screen_6_img_5, 0);
    lv_obj_set_pos(ui->screen_6_img_5, 103, 223);
    lv_obj_set_size(ui->screen_6_img_5, 33, 32);
    lv_obj_add_flag(ui->screen_6_img_5, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_6_img_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_6_img_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_6_img_5, lv_color_hex(0xc78b97), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_6_img_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_img_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_6_img_5, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_img_7
    ui->screen_6_img_7 = lv_img_create(ui->screen_6);
    lv_obj_add_flag(ui->screen_6_img_7, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_6_img_7, &_start_alpha_41x33);
    lv_img_set_pivot(ui->screen_6_img_7, 50,50);
    lv_img_set_angle(ui->screen_6_img_7, 0);
    lv_obj_set_pos(ui->screen_6_img_7, 95, 223);
    lv_obj_set_size(ui->screen_6_img_7, 41, 33);

    //Write style for screen_6_img_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_6_img_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_6_img_7, lv_color_hex(0xc39aad), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_6_img_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_img_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_6_img_7, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_img_3
    ui->screen_6_img_3 = lv_img_create(ui->screen_6);
    lv_obj_add_flag(ui->screen_6_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_6_img_3, &_right_alpha_44x38);
    lv_img_set_pivot(ui->screen_6_img_3, 50,50);
    lv_img_set_angle(ui->screen_6_img_3, 0);
    lv_obj_set_pos(ui->screen_6_img_3, 166, 220);
    lv_obj_set_size(ui->screen_6_img_3, 44, 38);

    //Write style for screen_6_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_6_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_6_img_3, lv_color_hex(0xc38787), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_6_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_6_img_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_img_4
    ui->screen_6_img_4 = lv_img_create(ui->screen_6);
    lv_obj_add_flag(ui->screen_6_img_4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_6_img_4, &_left_alpha_42x40);
    lv_img_set_pivot(ui->screen_6_img_4, 50,50);
    lv_img_set_angle(ui->screen_6_img_4, 0);
    lv_obj_set_pos(ui->screen_6_img_4, 23, 217);
    lv_obj_set_size(ui->screen_6_img_4, 42, 40);

    //Write style for screen_6_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_6_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_6_img_4, lv_color_hex(0xc78484), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_6_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_6_img_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_img_6
    ui->screen_6_img_6 = lv_img_create(ui->screen_6);
    lv_obj_add_flag(ui->screen_6_img_6, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_6_img_6, &_menu_alpha_37x36);
    lv_img_set_pivot(ui->screen_6_img_6, 50,50);
    lv_img_set_angle(ui->screen_6_img_6, 0);
    lv_obj_set_pos(ui->screen_6_img_6, 8, 9);
    lv_obj_set_size(ui->screen_6_img_6, 37, 36);

    //Write style for screen_6_img_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_6_img_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_6_img_6, lv_color_hex(0xd71a1a), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_6_img_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_img_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_6_img_6, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_img_8
    ui->screen_6_img_8 = lv_img_create(ui->screen_6);
    lv_obj_add_flag(ui->screen_6_img_8, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_6_img_8, &_JAY2_alpha_110x109);
    lv_img_set_pivot(ui->screen_6_img_8, 50,50);
    lv_img_set_angle(ui->screen_6_img_8, 0);
    lv_obj_set_pos(ui->screen_6_img_8, 65, 14);
    lv_obj_set_size(ui->screen_6_img_8, 110, 109);

    //Write style for screen_6_img_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_6_img_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_6_img_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_img_8, 40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_6_img_8, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_slider_1
    ui->screen_6_slider_1 = lv_slider_create(ui->screen_6);
    lv_slider_set_range(ui->screen_6_slider_1, 0, 100);
    lv_slider_set_mode(ui->screen_6_slider_1, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_6_slider_1, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_6_slider_1, 19, 184);
    lv_obj_set_size(ui->screen_6_slider_1, 201, 5);

    //Write style for screen_6_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_6_slider_1, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_6_slider_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_6_slider_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_slider_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_6_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_6_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_6_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_6_slider_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_6_slider_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_6_slider_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_slider_1, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_6_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_6_slider_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_6_slider_1, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_6_slider_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_slider_1, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_6_label_1
    ui->screen_6_label_1 = lv_label_create(ui->screen_6);
    lv_label_set_text(ui->screen_6_label_1, "晴天");
    lv_label_set_long_mode(ui->screen_6_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_6_label_1, 62, 143);
    lv_obj_set_size(ui->screen_6_label_1, 100, 20);

    //Write style for screen_6_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_6_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_6_label_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_6_label_1, &lv_font_SourceHanSansSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_6_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_6_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_6_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_6_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_6_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_6_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_6_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_6_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_6_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_6_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_tileview_1
    ui->screen_6_tileview_1 = lv_tileview_create(ui->screen_6);
    ui->screen_6_tileview_1_tile = lv_tileview_add_tile(ui->screen_6_tileview_1, 0, 0, LV_DIR_RIGHT);
    lv_obj_set_pos(ui->screen_6_tileview_1, 0, 0);
    lv_obj_set_size(ui->screen_6_tileview_1, 240, 300);
    lv_obj_set_scrollbar_mode(ui->screen_6_tileview_1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_6_tileview_1, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_6_tileview_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_6_tileview_1, 168, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_6_tileview_1, lv_color_hex(0x0767c1), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_6_tileview_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_tileview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_6_tileview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_6_tileview_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_6_tileview_1, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_6_tileview_1, lv_color_hex(0xeaeff3), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_6_tileview_1, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_tileview_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);



    //Write codes screen_6_cont_1
    ui->screen_6_cont_1 = lv_obj_create(ui->screen_6_tileview_1_tile);
    lv_obj_set_pos(ui->screen_6_cont_1, 11, 9);
    lv_obj_set_size(ui->screen_6_cont_1, 220, 59);
    lv_obj_set_scrollbar_mode(ui->screen_6_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_6_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_6_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_6_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_6_cont_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_6_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_cont_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_6_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_6_cont_1, lv_color_hex(0xa6c8ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_6_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_6_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_6_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_6_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_6_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_6_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_label_2
    ui->screen_6_label_2 = lv_label_create(ui->screen_6_cont_1);
    lv_label_set_text(ui->screen_6_label_2, "晴天——翻自周杰伦                             ");
    lv_label_set_long_mode(ui->screen_6_label_2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_6_label_2, 23, 17);
    lv_obj_set_size(ui->screen_6_label_2, 180, 22);

    //Write style for screen_6_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_6_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_6_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_6_label_2, &lv_font_SourceHanSansSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_6_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_6_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_6_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_6_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_6_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_6_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_6_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_6_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_6_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_6_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_cont_2
    ui->screen_6_cont_2 = lv_obj_create(ui->screen_6_tileview_1_tile);
    lv_obj_set_pos(ui->screen_6_cont_2, 11, 81);
    lv_obj_set_size(ui->screen_6_cont_2, 220, 59);
    lv_obj_set_scrollbar_mode(ui->screen_6_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_6_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_6_cont_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_6_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_6_cont_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_6_cont_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_cont_2, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_6_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_6_cont_2, lv_color_hex(0xb5d2ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_6_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_6_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_6_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_6_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_6_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_6_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_6_label_3
    ui->screen_6_label_3 = lv_label_create(ui->screen_6_cont_2);
    lv_label_set_text(ui->screen_6_label_3, "花海——翻自周杰伦                             ");
    lv_label_set_long_mode(ui->screen_6_label_3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_6_label_3, 24, 16);
    lv_obj_set_size(ui->screen_6_label_3, 180, 22);

    //Write style for screen_6_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_6_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_6_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_6_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_6_label_3, &lv_font_SourceHanSansSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_6_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_6_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_6_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_6_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_6_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_6_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_6_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_6_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_6_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_6_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_6.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_6);

    //Init events for screen.
    events_init_screen_6(ui);
}

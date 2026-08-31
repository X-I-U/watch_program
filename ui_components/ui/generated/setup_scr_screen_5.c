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



void setup_scr_screen_5(lv_ui *ui)
{
    //Write codes screen_5
    ui->screen_5 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_5, 240, 300);
    lv_obj_set_scrollbar_mode(ui->screen_5, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_5, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_carousel_1
    ui->screen_5_carousel_1 = lv_carousel_create(ui->screen_5);
    lv_carousel_set_element_width(ui->screen_5_carousel_1, 240);
    lv_obj_set_size(ui->screen_5_carousel_1, 240, 230);
    ui->screen_5_carousel_1_element_1 = lv_carousel_add_element(ui->screen_5_carousel_1, 0);
    ui->screen_5_carousel_1_element_2 = lv_carousel_add_element(ui->screen_5_carousel_1, 1);
    lv_obj_set_pos(ui->screen_5_carousel_1, 0, 0);
    lv_obj_set_size(ui->screen_5_carousel_1, 240, 230);
    lv_obj_set_scrollbar_mode(ui->screen_5_carousel_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_5_carousel_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_5_carousel_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_5_carousel_1, lv_color_hex(0xd2d2d2), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_5_carousel_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_carousel_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_carousel_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_5_carousel_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_5_carousel_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_carousel_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_5_carousel_1_extra_element_items_default
    static lv_style_t style_screen_5_carousel_1_extra_element_items_default;
    ui_init_style(&style_screen_5_carousel_1_extra_element_items_default);

    lv_style_set_bg_opa(&style_screen_5_carousel_1_extra_element_items_default, 0);
    lv_style_set_outline_width(&style_screen_5_carousel_1_extra_element_items_default, 0);
    lv_style_set_border_width(&style_screen_5_carousel_1_extra_element_items_default, 0);
    lv_style_set_radius(&style_screen_5_carousel_1_extra_element_items_default, 5);
    lv_style_set_shadow_width(&style_screen_5_carousel_1_extra_element_items_default, 0);
    lv_obj_add_style(ui->screen_5_carousel_1_element_2, &style_screen_5_carousel_1_extra_element_items_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_5_carousel_1_element_1, &style_screen_5_carousel_1_extra_element_items_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_FOCUSED for &style_screen_5_carousel_1_extra_element_items_focused
    static lv_style_t style_screen_5_carousel_1_extra_element_items_focused;
    ui_init_style(&style_screen_5_carousel_1_extra_element_items_focused);

    lv_style_set_bg_opa(&style_screen_5_carousel_1_extra_element_items_focused, 0);
    lv_style_set_outline_width(&style_screen_5_carousel_1_extra_element_items_focused, 0);
    lv_style_set_border_width(&style_screen_5_carousel_1_extra_element_items_focused, 0);
    lv_style_set_radius(&style_screen_5_carousel_1_extra_element_items_focused, 5);
    lv_style_set_shadow_width(&style_screen_5_carousel_1_extra_element_items_focused, 0);
    lv_obj_add_style(ui->screen_5_carousel_1_element_2, &style_screen_5_carousel_1_extra_element_items_focused, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_add_style(ui->screen_5_carousel_1_element_1, &style_screen_5_carousel_1_extra_element_items_focused, LV_PART_MAIN|LV_STATE_FOCUSED);



    //Write codes screen_5_cont_2
    ui->screen_5_cont_2 = lv_obj_create(ui->screen_5_carousel_1_element_1);
    lv_obj_set_pos(ui->screen_5_cont_2, 97, 92);
    lv_obj_set_size(ui->screen_5_cont_2, 43, 41);
    lv_obj_set_scrollbar_mode(ui->screen_5_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_5_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_cont_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_5_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_5_cont_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_5_cont_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_5_cont_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_5_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_label_2
    ui->screen_5_label_2 = lv_label_create(ui->screen_5_carousel_1_element_1);
    lv_label_set_text(ui->screen_5_label_2, "01");
    lv_label_set_long_mode(ui->screen_5_label_2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_5_label_2, 25, 138);
    lv_obj_set_size(ui->screen_5_label_2, 40, 27);

    //Write style for screen_5_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_label_2, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_cont_3
    ui->screen_5_cont_3 = lv_obj_create(ui->screen_5_carousel_1_element_1);
    lv_obj_set_pos(ui->screen_5_cont_3, 174, 92);
    lv_obj_set_size(ui->screen_5_cont_3, 43, 41);
    lv_obj_set_scrollbar_mode(ui->screen_5_cont_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_5_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_cont_3, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_5_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_5_cont_3, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_5_cont_3, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_5_cont_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_5_cont_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_label_3
    ui->screen_5_label_3 = lv_label_create(ui->screen_5_carousel_1_element_1);
    lv_label_set_text(ui->screen_5_label_3, "99");
    lv_label_set_long_mode(ui->screen_5_label_3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_5_label_3, 27, 58);
    lv_obj_set_size(ui->screen_5_label_3, 40, 27);

    //Write style for screen_5_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_label_3, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_cont_1
    ui->screen_5_cont_1 = lv_obj_create(ui->screen_5_carousel_1_element_1);
    lv_obj_set_pos(ui->screen_5_cont_1, 23, 93);
    lv_obj_set_size(ui->screen_5_cont_1, 43, 41);
    lv_obj_set_scrollbar_mode(ui->screen_5_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_5_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_5_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_5_cont_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_5_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_5_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_5_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_3
    ui->screen_5_img_3 = lv_img_create(ui->screen_5_carousel_1_element_1);
    lv_obj_add_flag(ui->screen_5_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_5_img_3, &_stop_alpha_35x37);
    lv_img_set_pivot(ui->screen_5_img_3, 50,50);
    lv_img_set_angle(ui->screen_5_img_3, 0);
    lv_obj_set_pos(ui->screen_5_img_3, 141, 179);
    lv_obj_set_size(ui->screen_5_img_3, 35, 37);

    //Write style for screen_5_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_label_4
    ui->screen_5_label_4 = lv_label_create(ui->screen_5_carousel_1_element_1);
    lv_label_set_text(ui->screen_5_label_4, "00");
    lv_label_set_long_mode(ui->screen_5_label_4, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_5_label_4, 99, 99);
    lv_obj_set_size(ui->screen_5_label_4, 40, 27);

    //Write style for screen_5_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_label_4, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_label_5
    ui->screen_5_label_5 = lv_label_create(ui->screen_5_carousel_1_element_1);
    lv_label_set_text(ui->screen_5_label_5, "01");
    lv_label_set_long_mode(ui->screen_5_label_5, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_5_label_5, 97, 141);
    lv_obj_set_size(ui->screen_5_label_5, 40, 27);

    //Write style for screen_5_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_label_5, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_label_5, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_label_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_label_6
    ui->screen_5_label_6 = lv_label_create(ui->screen_5_carousel_1_element_1);
    lv_label_set_text(ui->screen_5_label_6, "59");
    lv_label_set_long_mode(ui->screen_5_label_6, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_5_label_6, 99, 59);
    lv_obj_set_size(ui->screen_5_label_6, 40, 27);

    //Write style for screen_5_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_label_6, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_label_6, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_label_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_label_7
    ui->screen_5_label_7 = lv_label_create(ui->screen_5_carousel_1_element_1);
    lv_label_set_text(ui->screen_5_label_7, "00");
    lv_label_set_long_mode(ui->screen_5_label_7, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_5_label_7, 175, 99);
    lv_obj_set_size(ui->screen_5_label_7, 40, 27);

    //Write style for screen_5_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_label_7, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_label_7, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_label_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_label_8
    ui->screen_5_label_8 = lv_label_create(ui->screen_5_carousel_1_element_1);
    lv_label_set_text(ui->screen_5_label_8, "01");
    lv_label_set_long_mode(ui->screen_5_label_8, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_5_label_8, 173, 140);
    lv_obj_set_size(ui->screen_5_label_8, 40, 27);

    //Write style for screen_5_label_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_label_8, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_label_8, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_label_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_label_8, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_label_9
    ui->screen_5_label_9 = lv_label_create(ui->screen_5_carousel_1_element_1);
    lv_label_set_text(ui->screen_5_label_9, "59");
    lv_label_set_long_mode(ui->screen_5_label_9, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_5_label_9, 177, 62);
    lv_obj_set_size(ui->screen_5_label_9, 40, 27);

    //Write style for screen_5_label_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_label_9, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_label_9, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_label_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_label_9, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_label_1
    ui->screen_5_label_1 = lv_label_create(ui->screen_5_carousel_1_element_1);
    lv_label_set_text(ui->screen_5_label_1, "00");
    lv_label_set_long_mode(ui->screen_5_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_5_label_1, 26, 100);
    lv_obj_set_size(ui->screen_5_label_1, 40, 27);

    //Write style for screen_5_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_label_1, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_4
    ui->screen_5_img_4 = lv_img_create(ui->screen_5_carousel_1_element_1);
    lv_obj_add_flag(ui->screen_5_img_4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_5_img_4, &_start_alpha_47x35);
    lv_img_set_pivot(ui->screen_5_img_4, 50,50);
    lv_img_set_angle(ui->screen_5_img_4, 0);
    lv_obj_set_pos(ui->screen_5_img_4, 129, 181);
    lv_obj_set_size(ui->screen_5_img_4, 47, 35);
    lv_obj_add_flag(ui->screen_5_img_4, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_5_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_5_img_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_10
    ui->screen_5_img_10 = lv_img_create(ui->screen_5_carousel_1_element_1);
    lv_obj_add_flag(ui->screen_5_img_10, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_5_img_10, &_cancel_alpha_41x38);
    lv_img_set_pivot(ui->screen_5_img_10, 50,50);
    lv_img_set_angle(ui->screen_5_img_10, 0);
    lv_obj_set_pos(ui->screen_5_img_10, 60, 177);
    lv_obj_set_size(ui->screen_5_img_10, 41, 38);

    //Write style for screen_5_img_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_10, true, LV_PART_MAIN|LV_STATE_DEFAULT);



    //Write codes screen_5_label_10
    ui->screen_5_label_10 = lv_label_create(ui->screen_5_carousel_1_element_2);
    lv_label_set_text(ui->screen_5_label_10, "00:00:00");
    lv_label_set_long_mode(ui->screen_5_label_10, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_5_label_10, 28, 85);
    lv_obj_set_size(ui->screen_5_label_10, 180, 34);

    //Write style for screen_5_label_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_5_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_label_10, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_label_10, &lv_font_montserratMedium_35, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_label_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_5_label_10, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_5_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_5_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_5_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_5_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_5_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_7
    ui->screen_5_img_7 = lv_img_create(ui->screen_5_carousel_1_element_2);
    lv_obj_add_flag(ui->screen_5_img_7, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_5_img_7, &_retry_alpha_41x37);
    lv_img_set_pivot(ui->screen_5_img_7, 50,50);
    lv_img_set_angle(ui->screen_5_img_7, 0);
    lv_obj_set_pos(ui->screen_5_img_7, 50, 143);
    lv_obj_set_size(ui->screen_5_img_7, 41, 37);

    //Write style for screen_5_img_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_7, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_8
    ui->screen_5_img_8 = lv_img_create(ui->screen_5_carousel_1_element_2);
    lv_obj_add_flag(ui->screen_5_img_8, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_5_img_8, &_stop_alpha_36x33);
    lv_img_set_pivot(ui->screen_5_img_8, 50,50);
    lv_img_set_angle(ui->screen_5_img_8, 0);
    lv_obj_set_pos(ui->screen_5_img_8, 151, 151);
    lv_obj_set_size(ui->screen_5_img_8, 36, 33);

    //Write style for screen_5_img_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_8, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_9
    ui->screen_5_img_9 = lv_img_create(ui->screen_5_carousel_1_element_2);
    lv_obj_add_flag(ui->screen_5_img_9, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_5_img_9, &_start_alpha_42x34);
    lv_img_set_pivot(ui->screen_5_img_9, 50,50);
    lv_img_set_angle(ui->screen_5_img_9, 0);
    lv_obj_set_pos(ui->screen_5_img_9, 148, 150);
    lv_obj_set_size(ui->screen_5_img_9, 42, 34);

    //Write style for screen_5_img_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_5_img_9, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_9, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_1
    ui->screen_5_img_1 = lv_img_create(ui->screen_5);
    lv_obj_add_flag(ui->screen_5_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_5_img_1, &_timer2_alpha_41x36);
    lv_img_set_pivot(ui->screen_5_img_1, 50,50);
    lv_img_set_angle(ui->screen_5_img_1, 0);
    lv_obj_set_pos(ui->screen_5_img_1, 36, 242);
    lv_obj_set_size(ui->screen_5_img_1, 41, 36);

    //Write style for screen_5_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_5
    ui->screen_5_img_5 = lv_img_create(ui->screen_5);
    lv_obj_add_flag(ui->screen_5_img_5, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_5_img_5, &_back_alpha_37x34);
    lv_img_set_pivot(ui->screen_5_img_5, 50,50);
    lv_img_set_angle(ui->screen_5_img_5, 0);
    lv_obj_set_pos(ui->screen_5_img_5, 4, 11);
    lv_obj_set_size(ui->screen_5_img_5, 37, 34);

    //Write style for screen_5_img_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_5_img_5, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_5, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_5_img_6
    ui->screen_5_img_6 = lv_img_create(ui->screen_5);
    lv_obj_add_flag(ui->screen_5_img_6, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_5_img_6, &_clock7_alpha_52x49);
    lv_img_set_pivot(ui->screen_5_img_6, 50,50);
    lv_img_set_angle(ui->screen_5_img_6, 0);
    lv_obj_set_pos(ui->screen_5_img_6, 147, 235);
    lv_obj_set_size(ui->screen_5_img_6, 52, 49);

    //Write style for screen_5_img_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_5_img_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_5_img_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_img_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_5_img_6, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_5.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_5);

    //Init events for screen.
    events_init_screen_5(ui);
}

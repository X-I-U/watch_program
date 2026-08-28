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



void setup_scr_screen_9(lv_ui *ui)
{
    //Write codes screen_9
    ui->screen_9 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_9, 240, 300);
    lv_obj_set_scrollbar_mode(ui->screen_9, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_9, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_9, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_9_img_1
    ui->screen_9_img_1 = lv_img_create(ui->screen_9);
    lv_obj_add_flag(ui->screen_9_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_9_img_1, &_xiaozhi_alpha_96x90);
    lv_img_set_pivot(ui->screen_9_img_1, 50,50);
    lv_img_set_angle(ui->screen_9_img_1, 0);
    lv_obj_set_pos(ui->screen_9_img_1, 71, 75);
    lv_obj_set_size(ui->screen_9_img_1, 96, 90);

    //Write style for screen_9_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_9_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_9_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_9_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_9_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_9_label_1
    ui->screen_9_label_1 = lv_label_create(ui->screen_9);
    lv_label_set_text(ui->screen_9_label_1, "xiao zhi");
    lv_label_set_long_mode(ui->screen_9_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_9_label_1, 71, 174);
    lv_obj_set_size(ui->screen_9_label_1, 100, 32);

    //Write style for screen_9_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_9_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_9_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_9_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_9_label_1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_9_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_9_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_9_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_9_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_9_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_9_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_9_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_9_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_9_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_9_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_9.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_9);

    //Init events for screen.
    events_init_screen_9(ui);
}

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

    //Write codes screen_5_tabview_1
    ui->screen_5_tabview_1 = lv_tabview_create(ui->screen_5, LV_DIR_TOP, 50);
    lv_obj_set_pos(ui->screen_5_tabview_1, 0, 0);
    lv_obj_set_size(ui->screen_5_tabview_1, 240, 300);
    lv_obj_set_scrollbar_mode(ui->screen_5_tabview_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_5_tabview_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_5_tabview_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_5_tabview_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_5_tabview_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_5_tabview_1, lv_color_hex(0xd732ae), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_5_tabview_1, &lv_font_SourceHanSansSC_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_5_tabview_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_5_tabview_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_5_tabview_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_5_tabview_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_5_tabview_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_5_tabview_1, lv_color_hex(0x737373), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_5_tabview_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_5_tabview_1, 9, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_5_tabview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_5_tabview_1_extra_btnm_main_default
    static lv_style_t style_screen_5_tabview_1_extra_btnm_main_default;
    ui_init_style(&style_screen_5_tabview_1_extra_btnm_main_default);

    lv_style_set_bg_opa(&style_screen_5_tabview_1_extra_btnm_main_default, 255);
    lv_style_set_bg_color(&style_screen_5_tabview_1_extra_btnm_main_default, lv_color_hex(0x9ce8ff));
    lv_style_set_bg_grad_dir(&style_screen_5_tabview_1_extra_btnm_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_5_tabview_1_extra_btnm_main_default, 0);
    lv_style_set_radius(&style_screen_5_tabview_1_extra_btnm_main_default, 8);
    lv_obj_add_style(lv_tabview_get_tab_btns(ui->screen_5_tabview_1), &style_screen_5_tabview_1_extra_btnm_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_5_tabview_1_extra_btnm_items_default
    static lv_style_t style_screen_5_tabview_1_extra_btnm_items_default;
    ui_init_style(&style_screen_5_tabview_1_extra_btnm_items_default);

    lv_style_set_text_color(&style_screen_5_tabview_1_extra_btnm_items_default, lv_color_hex(0x291f1f));
    lv_style_set_text_font(&style_screen_5_tabview_1_extra_btnm_items_default, &lv_font_Acme_Regular_16);
    lv_style_set_text_opa(&style_screen_5_tabview_1_extra_btnm_items_default, 255);
    lv_obj_add_style(lv_tabview_get_tab_btns(ui->screen_5_tabview_1), &style_screen_5_tabview_1_extra_btnm_items_default, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_5_tabview_1_extra_btnm_items_checked
    static lv_style_t style_screen_5_tabview_1_extra_btnm_items_checked;
    ui_init_style(&style_screen_5_tabview_1_extra_btnm_items_checked);

    lv_style_set_text_color(&style_screen_5_tabview_1_extra_btnm_items_checked, lv_color_hex(0x007185));
    lv_style_set_text_font(&style_screen_5_tabview_1_extra_btnm_items_checked, &lv_font_Alatsi_Regular_18);
    lv_style_set_text_opa(&style_screen_5_tabview_1_extra_btnm_items_checked, 255);
    lv_style_set_border_width(&style_screen_5_tabview_1_extra_btnm_items_checked, 4);
    lv_style_set_border_opa(&style_screen_5_tabview_1_extra_btnm_items_checked, 255);
    lv_style_set_border_color(&style_screen_5_tabview_1_extra_btnm_items_checked, lv_color_hex(0x007185));
    lv_style_set_border_side(&style_screen_5_tabview_1_extra_btnm_items_checked, LV_BORDER_SIDE_RIGHT);
    lv_style_set_radius(&style_screen_5_tabview_1_extra_btnm_items_checked, 4);
    lv_style_set_bg_opa(&style_screen_5_tabview_1_extra_btnm_items_checked, 0);
    lv_obj_add_style(lv_tabview_get_tab_btns(ui->screen_5_tabview_1), &style_screen_5_tabview_1_extra_btnm_items_checked, LV_PART_ITEMS|LV_STATE_CHECKED);

    //Write codes 倒计时
    ui->screen_5_tabview_1_tab_1 = lv_tabview_add_tab(ui->screen_5_tabview_1,"倒计时");
    lv_obj_t * screen_5_tabview_1_tab_1_label = lv_label_create(ui->screen_5_tabview_1_tab_1);
    lv_label_set_text(screen_5_tabview_1_tab_1_label, "con1");

    //Write codes tab
    ui->screen_5_tabview_1_tab_2 = lv_tabview_add_tab(ui->screen_5_tabview_1,"tab");
    lv_obj_t * screen_5_tabview_1_tab_2_label = lv_label_create(ui->screen_5_tabview_1_tab_2);
    lv_label_set_text(screen_5_tabview_1_tab_2_label, "con2");

    //The custom code of screen_5.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_5);

}

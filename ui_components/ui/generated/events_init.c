/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"
/* 各功能屏的自定义绑定层：GUI Guider 重导出后这几行会被抹掉，需重加 */
#include "calc_ui.h"
#include "game2048_ui.h"
#include "music_ui.h"
#include "calendar_ui.h"
#include "xiaozhi_ui.h"
#include "timer_ui.h"
#include "ui_step.h"
#include "settings_ui.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


static void screen_2_img_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_3, guider_ui.screen_3_del, &guider_ui.screen_2_del, setup_scr_screen_3, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_2_img_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_7, guider_ui.screen_7_del, &guider_ui.screen_2_del, setup_scr_screen_7, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_2_img_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_9, guider_ui.screen_9_del, &guider_ui.screen_2_del, setup_scr_screen_9, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_2_cont_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_LEAVE:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_2_img_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_4, guider_ui.screen_4_del, &guider_ui.screen_2_del, setup_scr_screen_4, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_2_img_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_6, guider_ui.screen_6_del, &guider_ui.screen_2_del, setup_scr_screen_6, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_2_img_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_5, guider_ui.screen_5_del, &guider_ui.screen_2_del, setup_scr_screen_5, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_2_img_8_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_10, guider_ui.screen_10_del, &guider_ui.screen_2_del, setup_scr_screen_10, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_2_img_9_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_11, guider_ui.screen_11_del, &guider_ui.screen_2_del, setup_scr_screen_11, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_2 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_2_img_1, screen_2_img_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_img_5, screen_2_img_5_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_img_6, screen_2_img_6_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_cont_3, screen_2_cont_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_img_2, screen_2_img_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_img_3, screen_2_img_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_img_4, screen_2_img_4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_img_8, screen_2_img_8_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_img_9, screen_2_img_9_event_handler, LV_EVENT_ALL, ui);
}

static void screen_3_img_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.screen_3_del, setup_scr_screen_2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_3 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_3_img_1, screen_3_img_1_event_handler, LV_EVENT_ALL, ui);
    game2048_ui_screen_created(ui);   /* 重导出后要补 */
}

static void screen_4_img_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.screen_4_del, setup_scr_screen_2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_4 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_4_img_1, screen_4_img_1_event_handler, LV_EVENT_ALL, ui);
    calendar_ui_screen_created(ui);   /* 重导出后要补 */
}

static void screen_5_img_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.screen_5_del, setup_scr_screen_2, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_5 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_5_img_5, screen_5_img_5_event_handler, LV_EVENT_ALL, ui);
    timer_ui_screen_created(ui);   /* 重导出后要补 */
}

static void screen_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.screen_6_del, setup_scr_screen_2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_6_img_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.screen_6_img_5, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_6_img_7, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_6_img_7_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_6_img_5, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_6_img_7, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_6_img_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_6_tileview_1, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_6_cont_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.screen_6_tileview_1, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_6_cont_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.screen_6_tileview_1, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_6 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_6, screen_6_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_6_img_5, screen_6_img_5_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_6_img_7, screen_6_img_7_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_6_img_6, screen_6_img_6_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_6_cont_1, screen_6_cont_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_6_cont_2, screen_6_cont_2_event_handler, LV_EVENT_ALL, ui);
    music_ui_screen_created(ui);   /* 重导出后要补 */
}

static void screen_7_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.screen_7_del, setup_scr_screen_2, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_7 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_7, screen_7_event_handler, LV_EVENT_ALL, ui);
    calc_ui_screen_created(ui);   /* 重导出后要补 */
}

static void screen_9_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.screen_9_del, setup_scr_screen_2, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_9 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_9, screen_9_event_handler, LV_EVENT_ALL, ui);
    xiaozhi_ui_screen_created(ui);   /* 重导出后要补 */
}

static void screen_10_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.screen_10_del, setup_scr_screen_2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_10_cont_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_10_cont_4, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_10_cont_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_10_cont_8, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_10_cont_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_10_cont_9, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_10_img_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.screen_10_cont_4, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_10_img_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.screen_10_cont_8, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_10 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_10, screen_10_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_10_cont_1, screen_10_cont_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_10_cont_2, screen_10_cont_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_10_cont_3, screen_10_cont_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_10_img_3, screen_10_img_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_10_img_5, screen_10_img_5_event_handler, LV_EVENT_ALL, ui);
    settings_ui_screen_created(ui);
}

static void screen_11_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.screen_11_del, setup_scr_screen_2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_11 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_11, screen_11_event_handler, LV_EVENT_ALL, ui);
    ui_step_screen_created(ui);   /* 记步绑定层(重导出后要补) */
}


void events_init(lv_ui *ui)
{

}

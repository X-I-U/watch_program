/*
 * settings_ui.h — screen10 设置板块绑定层
 * WiFi 设置(动态列表 + 点击连网 + 弹窗状态) + 时间校准(手动同步)。
 * 数据来自 basic_components/WIFISTA + rtc_service。
 */
#ifndef __SETTINGS_UI_H_
#define __SETTINGS_UI_H_

#include "lvgl.h"
#include "gui_guider.h"

void settings_ui_screen_created(lv_ui *ui);   /* 进 screen10 时由 events_init_screen_10 调用 */
void settings_ui_refresh(void);               /* 每秒在 LVGL 锁内调用(不在 screen10 自动空转) */

#endif /* __SETTINGS_UI_H_ */

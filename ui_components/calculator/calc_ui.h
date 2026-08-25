/**
 * calc_ui.h —— 计算器 UI 适配层（screen_7 ↔ calc_core）
 *
 * 唯一的 LVGL 接触面：把 screen_7 的按钮绑定到 calc_core，
 * 并把结果显示到顶部 label。只依赖 lvgl + gui_guider.h，不碰驱动。
 */
#ifndef CALC_UI_H
#define CALC_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

/**
 * 在 screen_7 创建完成时调用一次（从生成代码 events_init_screen_7 里调）。
 * 负责：绑定所有按键事件、初始化顶部显示。
 */
void calc_ui_screen_created(lv_ui *ui);
 //在event_init.c中添加#include "calc_ui.h"
 //在函数events_init_screen_7()中调用calc_ui_screen_created(ui);

#ifdef __cplusplus
}
#endif

#endif /* CALC_UI_H */

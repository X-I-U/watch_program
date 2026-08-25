/**
 * game2048_ui.h —— 2048 UI 适配层（screen_3 ↔ game2048_core）
 *
 * 唯一的 LVGL 接触面：把 screen_3 的棋盘格子和分数控件绑定到 core，
 * 手势滑动驱动 core.move，并把结果刷到 16 个数字块 + 分数。
 * 只依赖 lvgl + gui_guider.h，不碰驱动，同 calc_ui。
 */
#ifndef GAME2048_UI_H
#define GAME2048_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

/**
 * 在 screen_3 创建完成时调用一次（从生成代码 events_init_screen_3 里调）。
 * 负责：识别 16 个格子、建立数字块 label、绑定手势、初始化并刷新棋盘。
 * 每次进入 screen_3 都会调用，等于新开一局。
 */
void game2048_ui_screen_created(lv_ui *ui);

//在event_init.c中添加#include "game2048_ui.h"
//在函数events_init_screen_3()中调用game2048_ui_screen_created(ui);

#ifdef __cplusplus
}
#endif

#endif /* GAME2048_UI_H */

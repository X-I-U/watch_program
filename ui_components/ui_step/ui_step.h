/*
 * ui_step.h — screen11 记步绑定层
 * 刷新 screen_11_label_2(今日步数) + screen_11_arc_1(目标完成度) +
 * screen_11_chart_1(7 天柱状图, 双系列: 灰=历史, 绿=今天) + 柱下星期标签。
 * 数据来自 basic_components/STEP_SVC(纯服务, 不碰 LVGL)。
 */
#ifndef __UI_STEP_H_
#define __UI_STEP_H_

#include "lvgl.h"
#include "gui_guider.h"

void ui_step_screen_created(lv_ui *ui);   /* 进 screen11 时由 events_init_screen_11 调用 */
void ui_step_refresh(void);               /* 每秒在 LVGL 锁内调用(无 screen11 时自动空转) */

#endif /* __UI_STEP_H_ */

/*
 * xiaozhi_ui.h — 小智AI UI 绑定层 (ui_components/xiaozhi_ui/)
 * 职责: 只碰 LVGL, 把 XIAOZHI 服务层的状态/对话文本接到 screen9 上。
 * 进页 → 连 → 自动对话; 三态显隐(连接中/聆听中/回复中); 对话日志纵向滚动。
 * 服务层接口见 basic_components/XIAOZHI/xiaozhi.h(不碰 LVGL)。
 *
 * 调用: 在 events_init.c 的 events_init_screen_9() 中调用(生成文件, GUI Guider
 * 重导出后需重新加 include + 调用, 同 calc/music/calendar 的流程)。
 */
#ifndef __XIAOZHI_UI_H_
#define __XIAOZHI_UI_H_

#include "gui_guider.h"

void xiaozhi_ui_screen_created(lv_ui *ui);

#endif /* __XIAOZHI_UI_H_ */

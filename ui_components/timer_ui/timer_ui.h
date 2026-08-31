/*
 * timer_ui.h — 计时器/秒表 UI 绑定层 (ui_components/timer_ui/)
 * 只碰 LVGL; 逻辑在 basic_components/TIMER_SVC(后台常驻, 退出页面照样计时)。
 * 计时器: 3个滚轮设时 → 开始/暂停/取消; 秒表: 开始/暂停/清空, 显示 MM:SS.CC。
 * 底部图标 = 当前页指示(高亮/变灰); 倒计时到点 → 全局弹窗(任意屏幕)。
 *
 * 调用: timer_ui_global_init() 在开机时(挂 LVGL 锁)调一次;
 *       timer_ui_screen_created() 在 events_init_screen_5() 中调用(重导出后需重加)。
 */
#ifndef __TIMER_UI_H_
#define __TIMER_UI_H_

#include "gui_guider.h"

void timer_ui_global_init(void);       /* 开机一次: 注册到点回调 + 常驻弹窗定时器 */
void timer_ui_screen_created(lv_ui *ui);

#endif /* __TIMER_UI_H_ */

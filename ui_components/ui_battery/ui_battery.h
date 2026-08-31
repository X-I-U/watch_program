/*
 * ui_battery.h — screen2 电量显示绑定层
 * 刷新 screen_2_bar_1(进度条) + screen_2_label_3(百分比) + screen_2_img_10(充电图标)。
 * 数据来自 basic_components/AXP2101(纯寄存器, 不碰 LVGL)。
 */
#ifndef __UI_BATTERY_H_
#define __UI_BATTERY_H_

void ui_battery_refresh(void);   /* 读 AXP → 刷 screen2 电量(带变化检测) */

#endif /* __UI_BATTERY_H_ */

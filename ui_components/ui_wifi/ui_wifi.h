#ifndef __UI_WIFI_H_
#define __UI_WIFI_H_
/* screen2 顶部 WiFi 图标绑定层 (ui_components/ui_wifi/)
 * 只碰 LVGL; 连接状态来自 basic_components/WIFISTA(wifista_is_connected)。
 * ui_wifi_refresh(): 每秒在 LVGL 锁内调用一次 —— 已连 WiFi 正常显示,
 *                    未连上就把图标透明度拉低(灰淡), 直观表示"无网"。 */
void ui_wifi_refresh(void);
#endif /* __UI_WIFI_H_ */

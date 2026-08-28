#ifndef __XIAOZHI_H_
#define __XIAOZHI_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * 小智AI 服务层 (basic_components/XIAOZHI)
 * 向上: 对应用暴露极简接口
 * 向下: 依赖 mic_driver / speaker / wifista / esp_xiaozhi
 * 原则: 不碰 UI/LVGL, 底层驱动与顶层应用解耦
 * ============================================================ */

/* 初始化并连接:
 *   等 WiFi → get_info → 未绑定则打印 6 位激活码(去 xiaozhi.me 控制台添加设备);
 *   已绑定则 chat_init + start 连接服务器。
 * 返回 ESP_OK = 已进入连接流程(未绑定时会打印激活码并返回)。 */
esp_err_t xiaozhi_init(void);

/* 开始一轮对话(触发): 开音频通道 → 听 → 答。Stage D 实现。 */
esp_err_t xiaozhi_talk(void);

#ifdef __cplusplus
}
#endif

#endif /* __XIAOZHI_H_ */

#ifndef __XIAOZHI_H_
#define __XIAOZHI_H_

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * 小智AI 服务层 (basic_components/XIAOZHI)
 * 向上: 对应用/UI 暴露极简接口(状态查询 + 文本回调 + 开关)
 * 向下: 依赖 mic_driver / speaker / wifista / esp_xiaozhi
 * 原则: 不碰 UI/LVGL, 底层驱动与顶层应用解耦
 * ============================================================ */

/* 状态枚举: UI 据此显示 连接中/聆听中/回复中 三态 */
typedef enum {
    XIAOZHI_STATE_IDLE = 0,   /* 未连接(尚未启动/连接失败) */
    XIAOZHI_STATE_CONNECTING, /* 连接中(WiFi/TLS/握手) */
    XIAOZHI_STATE_NEED_BIND,  /* 未绑定, 需用激活码去 xiaozhi.me 添加设备 */
    XIAOZHI_STATE_CONNECTED,  /* 已连服务器, 空闲 */
    XIAOZHI_STATE_LISTENING,  /* 聆听中(正在采音, 等用户说话) */
    XIAOZHI_STATE_SPEAKING,   /* 回复中(TTS 播放) */
} xiaozhi_state_t;

/* 对话文本回调: 收到一条 user/ai 文本时调用。
   注意: 在 websocket 任务上下文回调, 不能碰 LVGL! 必须把 text 拷贝走,
   且 text 指针只在回调内有效。 */
typedef void (*xiaozhi_text_cb_t)(const char *role, const char *text);

/* 初始化并连接(幂等):
 *   等 WiFi → get_info → 未绑定则拿到激活码(状态变 NEED_BIND);
 *   已绑定则 chat_init + start 连接服务器。
 * 已连接/已初始化时再调用是安全的(直接返回)。 */
esp_err_t xiaozhi_init(void);

/* 开始一轮对话(触发): 开音频通道 → 听 → 答。 */
esp_err_t xiaozhi_talk(void);

/* 查询当前状态(UI 据此驱动三态显隐)。 */
xiaozhi_state_t xiaozhi_get_state(void);

/* 激活/停用(UI 页进入/离开时调用):
 *   active=false 时关闭音频通道并停止聆听, 且 xz_conv 不再自动开通道
 *   (实现"只有在小智页才听")。 */
void xiaozhi_set_active(bool active);

/* 注册对话文本回调(收到 CHAT_TEXT 事件时推送)。传 NULL 取消。 */
void xiaozhi_set_text_cb(xiaozhi_text_cb_t cb);

/* 未绑定时的 6 位激活码(状态为 NEED_BIND 时有效), 无则返回 NULL。 */
const char *xiaozhi_get_activation_code(void);

#ifdef __cplusplus
}
#endif

#endif /* __XIAOZHI_H_ */

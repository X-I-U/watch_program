#ifndef __WIFISTA_H_
#define __WIFISTA_H_

#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"

#define DEFAULT_SSID "mywifi"
#define DEFAULT_PWD  "fxc520146"

void wifista_init(void);
bool wifi_wait_connected(int timeout_ms);   /* 阻塞等连接(测试用) */

/* 切到指定网络: 改配置 → 取消自动重连 → 断开旧的 → 连新的(异步, 结果看 is_connected) */
esp_err_t wifista_connect(const char *ssid, const char *pwd);

/* 当前是否已连上(网卡 up 且有 IP) */
bool wifista_is_connected(void);

/* 读当前配置的 ssid(注意是"配置的", 可能还没连上) */
esp_err_t wifista_get_current_ssid(char *buf, size_t len);

#endif /* __WIFISTA_H_ */

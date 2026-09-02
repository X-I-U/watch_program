#include <string.h>
#include "wifista.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

/* 断开后 1 秒自动重连(用一次性定时器, 避免 esp_wifi_connect() 忙循环)。
   文件级 static, wifista_connect 切网时要取消它, 防旧网络误触发。 */
static esp_timer_handle_t s_retry_timer = NULL;

static void wifista_reconnect_timer_cb(void *arg)
{
    esp_wifi_connect();
}

static void wifista_schedule_reconnect(void)
{
    if (s_retry_timer == NULL) {
        const esp_timer_create_args_t args = {
            .callback = &wifista_reconnect_timer_cb,
            .name = "wifi_retry",
        };
        if (esp_timer_create(&args, &s_retry_timer) != ESP_OK) return;
    }
    esp_timer_start_once(s_retry_timer, 1000 * 1000);  /* 1 秒后重连 */
}

void wifista_event_handler(void* event_handler_arg,esp_event_base_t event_base,int32_t event_id,void* event_data)
{
    if(event_base == WIFI_EVENT)
    {
        if(event_id == WIFI_EVENT_STA_START)
        {
            esp_wifi_connect();
        }
        else if(event_id == WIFI_EVENT_STA_CONNECTED)
        {
            // lcd_show_string(1,1,"connected   ",YELLOW,BLACK);
        }
        else if(event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            // lcd_show_string(1,1,"disconnected",YELLOW,BLACK);
            wifista_schedule_reconnect();  /* 自动重连，不要 stop（否则热点后开就再也连不上） */
        }
    }
    else if(event_base == IP_EVENT)
    {
        if(event_id == IP_EVENT_STA_GOT_IP)
        {
            esp_netif_ip_info_t *event = (esp_netif_ip_info_t *)event_data;
            ESP_LOGI("wifi", "got ip " IPSTR, IP2STR(&event->ip));
        }
    }
}

void wifista_init(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wifista_event_handler,NULL);
    esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,&wifista_event_handler,NULL);
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t wifista_config = {
        .sta = {
            .ssid = DEFAULT_SSID,
            .password = DEFAULT_PWD,
        }
    };
    esp_wifi_set_config(WIFI_IF_STA, &wifista_config);

    esp_wifi_start();
}

/* 切到指定网络: 先 stop 整个 wifi(连接中/重试中直接 set_config 会报
   "sta is connecting, cannot set config"), 再改配置, 再 start(STA_START 事件自动 connect) */
esp_err_t wifista_connect(const char *ssid, const char *pwd)
{
    if (!ssid || ssid[0] == '\0') return ESP_ERR_INVALID_ARG;

    wifi_config_t cfg = {0};
    strncpy((char *)cfg.sta.ssid, ssid, sizeof(cfg.sta.ssid) - 1);
    if (pwd) strncpy((char *)cfg.sta.password, pwd, sizeof(cfg.sta.password) - 1);

    /* 取消自动重连定时器, 防旧网络状态干扰 */
    if (s_retry_timer) esp_timer_stop(s_retry_timer);

    esp_err_t r = esp_wifi_stop();   /* 停止后 set_config 才允许 */
    if (r != ESP_OK && r != ESP_ERR_WIFI_NOT_STARTED) {
        ESP_LOGW("wifi", "wifi stop failed %d", r);
    }
    r = esp_wifi_set_config(WIFI_IF_STA, &cfg);
    if (r != ESP_OK) return r;
    return esp_wifi_start();   /* 触发 WIFI_EVENT_STA_START → 自动 esp_wifi_connect() */
}

bool wifista_is_connected(void)
{
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (!netif || !esp_netif_is_netif_up(netif)) return false;
    esp_netif_ip_info_t ip;
    if (esp_netif_get_ip_info(netif, &ip) != ESP_OK) return false;
    return ip.ip.addr != 0;
}

esp_err_t wifista_get_current_ssid(char *buf, size_t len)
{
    if (!buf || len == 0) return ESP_ERR_INVALID_ARG;
    wifi_config_t cfg = {0};
    esp_err_t r = esp_wifi_get_config(WIFI_IF_STA, &cfg);
    if (r != ESP_OK) return r;
    strncpy(buf, (char *)cfg.sta.ssid, len - 1);
    buf[len - 1] = '\0';
    return ESP_OK;
}

/* 阻塞等待 WiFi 连上（最多等 N 秒），测试用 */
bool wifi_wait_connected(int timeout_ms)
{
    int waited = 0;
    while (waited < timeout_ms) {
        if (esp_netif_is_netif_up(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"))) {
            // 网卡 up 了，再确认拿到 IP
            esp_netif_ip_info_t ip;
            if (esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip) == ESP_OK
                && ip.ip.addr != 0) {
                ESP_LOGI("wifi", "IP: " IPSTR, IP2STR(&ip.ip));
                return true;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
        waited += 100;
    }
    ESP_LOGW("wifi", "WiFi connect timeout");
    return false;
}

#include "wifista.h"
#include "esp_wifi.h"
#include "esp_timer.h"

/* 断开后 1 秒自动重连（用一次性定时器，避免 esp_wifi_connect() 忙循环） */
static void wifista_reconnect_timer_cb(void *arg)
{
    esp_wifi_connect();
}

static void wifista_schedule_reconnect(void)
{
    static esp_timer_handle_t retry_timer = NULL;
    if (retry_timer == NULL) {
        const esp_timer_create_args_t args = {
            .callback = &wifista_reconnect_timer_cb,
            .name = "wifi_retry",
        };
        if (esp_timer_create(&args, &retry_timer) != ESP_OK) return;
    }
    esp_timer_start_once(retry_timer, 1000 * 1000);  /* 1 秒后重连 */
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
            // lcd_show_num(2,1,esp_ip4_addr1_16(&event->ip),3,GREEN,BLACK);
            // lcd_show_string(2,4,".",GREEN,BLACK);
            // lcd_show_num(2,5,esp_ip4_addr2_16(&event->ip),3,GREEN,BLACK);
            // lcd_show_string(2,8,".",GREEN,BLACK);
            // lcd_show_num(2,9,esp_ip4_addr3_16(&event->ip),3,GREEN,BLACK);
            // lcd_show_string(2,12,".",GREEN,BLACK);
            // lcd_show_num(2,13,esp_ip4_addr4_16(&event->ip),3,GREEN,BLACK);
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
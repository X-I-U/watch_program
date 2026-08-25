#include "myntc.h"
#include "esp_sntp.h"
#include <time.h>


void myntc_init(void)
{
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_setservername(1, "cn.pool.ntp.org");
    esp_sntp_setservername(2, "ntp1.aliyun.com");
    esp_sntp_init();
    setenv ("TZ", "CST-8", 1);
    tzset();
}
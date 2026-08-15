#include "key.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void key_init(void)
{
    gpio_config_t gpio_structure = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ull<<GPIO_NUM_9),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&gpio_structure);
}

uint8_t key_scan(void)
{
    uint8_t key_num = 0 ;
    if(gpio_get_level(GPIO_NUM_9) == 0)
    {
        vTaskDelay(20);
        while(gpio_get_level(GPIO_NUM_9) == 0); 
        vTaskDelay(20);
        key_num = 1;
    }
    return key_num;
}
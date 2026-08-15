#ifndef __LED_H_
#define __LED_H_

#include "driver/gpio.h"

void led_init(void);
void gpio_toggle(gpio_num_t gpio_num);

#endif
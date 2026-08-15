#ifndef __LCD_H_
#define __LCD_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_tk018.h"
#include "esp_lcd_touch_cst816s.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lv_demos.h"

#define EXAMPLE_LCD_H_RES              240
#define EXAMPLE_LCD_V_RES              300
#define LCD_BIT_PER_PIXEL              16


//Qspi
#define LCD_HOST    SPI2_HOST
#define EXAMPLE_PIN_NUM_LCD_CS            (GPIO_NUM_16)
#define EXAMPLE_PIN_NUM_LCD_PCLK          (GPIO_NUM_6)
#define EXAMPLE_PIN_NUM_LCD_DATA0         (GPIO_NUM_15)
#define EXAMPLE_PIN_NUM_LCD_DATA1         (GPIO_NUM_7)
#define EXAMPLE_PIN_NUM_LCD_DATA2         (GPIO_NUM_17)
#define EXAMPLE_PIN_NUM_LCD_DATA3         (GPIO_NUM_18)
#define EXAMPLE_PIN_NUM_LCD_RST           -1

//iic
#define TOUCH_HOST  I2C_NUM_1
#define EXAMPLE_PIN_NUM_TOUCH_SCL         (GPIO_NUM_5)
#define EXAMPLE_PIN_NUM_TOUCH_SDA         (GPIO_NUM_4)
#define EXAMPLE_PIN_NUM_TOUCH_RST         -1
#define EXAMPLE_PIN_NUM_TOUCH_INT         -1

// LVGLtask配置
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 1
#define EXAMPLE_LVGL_TASK_STACK_SIZE   (4 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY     2

extern lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
extern lv_disp_drv_t disp_drv;      // contains callback functions

extern esp_lcd_panel_handle_t panel_handle;
extern esp_lcd_touch_handle_t tp;

void lcd_display_init();
void lcd_touch_init();
lv_disp_t *lvgl_init();
void lvgl_timer_init(lv_disp_t *disp);

#endif
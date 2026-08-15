/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/cdefs.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "esp_log.h"

#include "esp_lcd_tk018.h"

#define LCD_OPCODE_WRITE_CMD        (0x02ULL)
#define LCD_OPCODE_READ_CMD         (0x0BULL)
#define LCD_OPCODE_WRITE_COLOR      (0x32ULL)

#define SPD2010_CMD_SET             (0xFF)
#define SPD2010_CMD_SET_BYTE0       (0x20)
#define SPD2010_CMD_SET_BYTE1       (0x10)
#define SPD2010_CMD_SET_USER        (0x00)

static const char *TAG = "spd2010";

static esp_err_t panel_spd2010_del(esp_lcd_panel_t *panel);
static esp_err_t panel_spd2010_reset(esp_lcd_panel_t *panel);
static esp_err_t panel_spd2010_init(esp_lcd_panel_t *panel);
static esp_err_t panel_spd2010_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);
static esp_err_t panel_spd2010_invert_color(esp_lcd_panel_t *panel, bool invert_color_data);
static esp_err_t panel_spd2010_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y);
static esp_err_t panel_spd2010_swap_xy(esp_lcd_panel_t *panel, bool swap_axes);
static esp_err_t panel_spd2010_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t panel_spd2010_disp_on_off(esp_lcd_panel_t *panel, bool off);

typedef struct {
    esp_lcd_panel_t base;
    esp_lcd_panel_io_handle_t io;
    int reset_gpio_num;
    int x_gap;
    int y_gap;
    uint8_t fb_bits_per_pixel;
    uint8_t madctl_val; // save current value of LCD_CMD_MADCTL register
    uint8_t colmod_val; // save current value of LCD_CMD_COLMOD register
    const spd2010_lcd_init_cmd_t *init_cmds;
    uint16_t init_cmds_size;
    struct {
        unsigned int use_qspi_interface: 1;
        unsigned int reset_level: 1;
    } flags;
} spd2010_panel_t;

esp_err_t esp_lcd_new_panel_tk018(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel)
{
    ESP_RETURN_ON_FALSE(io && panel_dev_config && ret_panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    esp_err_t ret = ESP_OK;
    spd2010_panel_t *spd2010 = NULL;
    spd2010 = calloc(1, sizeof(spd2010_panel_t));
    ESP_GOTO_ON_FALSE(spd2010, ESP_ERR_NO_MEM, err, TAG, "no mem for spd2010 panel");

    if (panel_dev_config->reset_gpio_num >= 0) {
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << panel_dev_config->reset_gpio_num,
        };
        ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "configure GPIO for RST line failed");
    }

    switch (panel_dev_config->rgb_ele_order) {
    case LCD_RGB_ELEMENT_ORDER_RGB:
        spd2010->madctl_val = 0;
        break;
    case LCD_RGB_ELEMENT_ORDER_BGR:
        spd2010->madctl_val |= LCD_CMD_BGR_BIT;
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG, "unsupported color element order");
        break;
    }

    uint8_t fb_bits_per_pixel = 0;
    switch (panel_dev_config->bits_per_pixel) {
    case 16: // RGB565
        spd2010->colmod_val = 0x55;
        fb_bits_per_pixel = 16;
        break;
    case 18: // RGB666
        spd2010->colmod_val = 0x66;
        // each color component (R/G/B) should occupy the 6 high bits of a byte, which means 3 full bytes are required for a pixel
        fb_bits_per_pixel = 24;
        break;
    case 24: // RGB888
        spd2010->colmod_val = 0x77;
        fb_bits_per_pixel = 24;
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG, "unsupported pixel width");
        break;
    }

    spd2010->io = io;
    spd2010->reset_gpio_num = panel_dev_config->reset_gpio_num;
    spd2010->fb_bits_per_pixel = fb_bits_per_pixel;
    spd2010_vendor_config_t *vendor_config = (spd2010_vendor_config_t *)panel_dev_config->vendor_config;
    if (vendor_config) {
        spd2010->init_cmds = vendor_config->init_cmds;
        spd2010->init_cmds_size = vendor_config->init_cmds_size;
        spd2010->flags.use_qspi_interface = vendor_config->flags.use_qspi_interface;
    }
    spd2010->flags.reset_level = panel_dev_config->flags.reset_active_high;
    spd2010->base.del = panel_spd2010_del;
    spd2010->base.reset = panel_spd2010_reset;
    spd2010->base.init = panel_spd2010_init;
    spd2010->base.draw_bitmap = panel_spd2010_draw_bitmap;
    spd2010->base.invert_color = panel_spd2010_invert_color;
    spd2010->base.set_gap = panel_spd2010_set_gap;
    spd2010->base.mirror = panel_spd2010_mirror;
    spd2010->base.swap_xy = panel_spd2010_swap_xy;
    spd2010->base.disp_on_off = panel_spd2010_disp_on_off;
    *ret_panel = &(spd2010->base);
    ESP_LOGD(TAG, "new spd2010 panel @%p", spd2010);

    ESP_LOGI(TAG, "LCD panel create success");

    return ESP_OK;

err:
    if (spd2010) {
        if (panel_dev_config->reset_gpio_num >= 0) {
            gpio_reset_pin(panel_dev_config->reset_gpio_num);
        }
        free(spd2010);
    }
    return ret;
}

static esp_err_t tx_param(spd2010_panel_t *spd2010, esp_lcd_panel_io_handle_t io, int lcd_cmd, const void *param, size_t param_size)
{

    // 命令字 = [0x02][0x00][cmd][0x00] QSPI/SPI 通用
    lcd_cmd &= 0xff;
    lcd_cmd <<= 8;
    lcd_cmd |= LCD_OPCODE_WRITE_CMD << 24;

    return esp_lcd_panel_io_tx_param(io, lcd_cmd, param, param_size);
}

static esp_err_t tx_color(spd2010_panel_t *spd2010, esp_lcd_panel_io_handle_t io, int lcd_cmd, const void *param, size_t param_size)
{
    
    // 颜色命令 QSPI 用 0x32 切换四线 SPI 用 0x02 单线
    uint8_t opcode = spd2010->flags.use_qspi_interface ? LCD_OPCODE_WRITE_COLOR : LCD_OPCODE_WRITE_CMD;
    lcd_cmd = (opcode << 24) | ((lcd_cmd & 0xFF) << 8);

    return esp_lcd_panel_io_tx_color(io, lcd_cmd, param, param_size);
}

static esp_err_t panel_spd2010_del(esp_lcd_panel_t *panel)
{
    spd2010_panel_t *spd2010 = __containerof(panel, spd2010_panel_t, base);

    if (spd2010->reset_gpio_num >= 0) {
        gpio_reset_pin(spd2010->reset_gpio_num);
    }
    ESP_LOGD(TAG, "del spd2010 panel @%p", spd2010);
    free(spd2010);
    return ESP_OK;
}

static esp_err_t panel_spd2010_reset(esp_lcd_panel_t *panel)
{
    spd2010_panel_t *spd2010 = __containerof(panel, spd2010_panel_t, base);
    esp_lcd_panel_io_handle_t io = spd2010->io;

    // Perform hardware reset
    if (spd2010->reset_gpio_num >= 0) {
        gpio_set_level(spd2010->reset_gpio_num, spd2010->flags.reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(spd2010->reset_gpio_num, !spd2010->flags.reset_level);
        vTaskDelay(pdMS_TO_TICKS(120));
    } else { // Perform software reset
        ESP_RETURN_ON_ERROR(tx_param(spd2010, io, LCD_CMD_SWRESET, NULL, 0), TAG, "send command failed");
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    return ESP_OK;
}

static const spd2010_lcd_init_cmd_t vendor_specific_init_default[] = {
    //  {cmd, { data }, data_size, delay_ms}

    //  初始化流程分为三个阶段：
    //  1. 扩展命令集配置 0xFF切换页面 电源、时序、Gamma校正
    //  2. 基本命令集配置 0xfd切回 像素格式、显示方向、TE信号
    //  3. 启动显示：退出休眠 -> 显示开启

    // 选择命令页 CMD_SET  切换到扩展命令集页面
    {0xFF, (uint8_t []){0x20, 0x10, 0x10}, 3, 0},
    // 内部电源控制
    {0x0C, (uint8_t []){0x11}, 1, 0},
    // 休眠退出 SLEEP OUT
    {0x10, (uint8_t []){0x02}, 1, 0},
    // 休眠退出 SLEEP OUT2
    {0x11, (uint8_t []){0x11}, 1, 0},
    // VCI1 电源电压设置
    {0x15, (uint8_t []){0x42}, 1, 0},
    // VCI2 电源电压设置
    {0x16, (uint8_t []){0x11}, 1, 0},
    // VCI3 电源电压设置
    {0x1A, (uint8_t []){0x02}, 1, 0},
    // 再次休眠退出，延时 10ms 等待电压稳定
    {0x11, (uint8_t []){}, 0, 10},
    // 选择命令页：切换到显示功能设置页面
    {0xfd, (uint8_t []){0x06, 0x08}, 2, 0},
    // 分辨率设置
    {0x61, (uint8_t []){0x07, 0x04}, 2, 0},
    // 显示区域时序设置
    {0x62, (uint8_t []){0x00, 0x44, 0x45}, 3, 0},
    // 水平时序参数
    {0x63, (uint8_t []){0x41, 0x07, 0x12, 0x12}, 4, 0},
    // 垂直时序参数
    {0x64, (uint8_t []){0x37}, 1, 0},
    // 垂直时序参数2
    {0x65, (uint8_t []){0x09, 0x10, 0x21}, 3, 0},
    // 垂直时序参数3
    {0x66, (uint8_t []){0x09, 0x10, 0x21}, 3, 0},
    // 垂直时序参数4
    {0x67, (uint8_t []){0x20, 0x40}, 2, 0},
    // 同步信号与时钟参数
    {0x68, (uint8_t []){0x90, 0x4c, 0x7C, 0x66}, 4, 0},
    // 帧率控制
    {0xb1, (uint8_t []){0x0F, 0x02, 0x01}, 3, 0},
    // 显示反转控制
    {0xB4, (uint8_t []){0x01}, 1, 0},
    // 空白行 Blanking 时序
    {0xB5, (uint8_t []){0x02, 0x02, 0x0a, 0x14}, 4, 0},
    // 显示功能控制
    {0xB6, (uint8_t []){0x04, 0x01, 0x9f, 0x00, 0x02}, 5, 0},
    // VCOM 参考电压
    {0xdf, (uint8_t []){0x11}, 1, 0},
    // 正极性 Gamma 校正1
    {0xE2, (uint8_t []){0x13, 0x00, 0x00, 0x30, 0x33, 0x3f}, 6, 0},
    // 正极性 Gamma 校正2
    {0xE5, (uint8_t []){0x3f, 0x33, 0x30, 0x00, 0x00, 0x13}, 6, 0},
    // 正极性 Gamma 校正3
    {0xE1, (uint8_t []){0x00, 0x57}, 2, 0},
    // 负极性 Gamma 校正3
    {0xE4, (uint8_t []){0x58, 0x00}, 2, 0},
    // 正极性 Gamma 校正4
    {0xE0, (uint8_t []){0x01, 0x03, 0x0e, 0x0e, 0x0c, 0x15, 0x19}, 7, 0},
    // 负极性 Gamma 校正4
    {0xE3, (uint8_t []){0x1a, 0x16, 0x0C, 0x0f, 0x0e, 0x0d, 0x02, 0x01}, 8, 0},
    // 正极性 Gamma 校正5
    {0xE6, (uint8_t []){0x00, 0xff}, 2, 0},
    // 负极性 Gamma 校正1
    {0xE7, (uint8_t []){0x01, 0x04, 0x03, 0x03, 0x00, 0x12}, 6, 0},
    // 负极性 Gamma 校正2
    {0xE8, (uint8_t []){0x00, 0x70, 0x00}, 3, 0},
    // VGH/VGL 电荷泵控制
    {0xEc, (uint8_t []){0x52}, 1, 0},
    // 正常模式命令集选择
    {0xF1, (uint8_t []){0x01, 0x01, 0x02}, 3, 0},
    // 显示功能控制2
    {0xF6, (uint8_t []){0x09, 0x10, 0x00, 0x00}, 4, 0},

    // 解锁/密钥配置 QSPI 寄存器写入前必须解锁
    {0xDE, (uint8_t []){0x00}, 1, 0},
    {0xDF, (uint8_t []){0x98, 0x55}, 2, 0},

    // QSPI 读取通道配置
    {0xCE, (uint8_t []){0x0D, 0x00}, 2, 0}, // 00=D1 read 模式
    {0xD8, (uint8_t []){0x08, 0x00}, 2, 0},

    // 选择命令页：切回基本命令集
    {0xfd, (uint8_t []){0xfa, 0xfc}, 2, 0},
    // 像素格式 16bpp SPI格式 0x05
    {0x3a, (uint8_t []){0x05}, 1, 0},
    // 内存访问控制 MADCTL BGR顺序
    {0x36, (uint8_t []){0x08}, 1, 0},
    // TE 撕裂效应 信号 V-blanking模式下启用
    {0x35, (uint8_t []){0x00}, 1, 0},
    // 显示反转关闭，延时 1ms
    {0x21, (uint8_t []){}, 0, 1},
    // 像素格式 16bpp RGB565 MIPI DCS标准 0x55
    {0x3A, (uint8_t []){0x55}, 1, 0},
    // 内存访问控制 MADCTL BGR
    {0x36, (uint8_t []){0x08}, 1, 0},
    // 休眠退出 SLEEP OUT 延时 100ms
    {0x11, (uint8_t []){}, 0, 100},
    // 显示开启 DISPLAY ON
    {0x29, (uint8_t []){}, 0, 0},

    // 初始化完成：面板已配置为 240x300, RGB565, BGR, QSPI/SPI 模式

};

static esp_err_t panel_spd2010_init(esp_lcd_panel_t *panel)
{
    spd2010_panel_t *spd2010 = __containerof(panel, spd2010_panel_t, base);
    esp_lcd_panel_io_handle_t io = spd2010->io;
    const spd2010_lcd_init_cmd_t *init_cmds = NULL;
    uint16_t init_cmds_size = 0;
    bool is_user_set = true;
    bool is_cmd_overwritten = false;

    ESP_RETURN_ON_ERROR(tx_param(spd2010, io, SPD2010_CMD_SET, (uint8_t[]) {
        SPD2010_CMD_SET_BYTE0, SPD2010_CMD_SET_BYTE1, SPD2010_CMD_SET_USER
    }, 3), TAG, "send command failed");
    ESP_RETURN_ON_ERROR(tx_param(spd2010, io, LCD_CMD_MADCTL, (uint8_t[]) {
        spd2010->madctl_val,
    }, 1), TAG, "send command failed");
    ESP_RETURN_ON_ERROR(tx_param(spd2010, io, LCD_CMD_COLMOD, (uint8_t[]) {
        spd2010->colmod_val,
    }, 1), TAG, "send command failed");

    // vendor specific initialization, it can be different between manufacturers
    // should consult the LCD supplier for initialization sequence code
    if (spd2010->init_cmds) {
        init_cmds = spd2010->init_cmds;
        init_cmds_size = spd2010->init_cmds_size;
    } else {
        init_cmds = vendor_specific_init_default;
        init_cmds_size = sizeof(vendor_specific_init_default) / sizeof(spd2010_lcd_init_cmd_t);
    }

    for (int i = 0; i < init_cmds_size; i++) {
        // Check if the command has been used or conflicts with the internal only when command2 is disable
        if (is_user_set && (init_cmds[i].data_bytes > 0)) {
            switch (init_cmds[i].cmd) {
            case LCD_CMD_MADCTL:
                is_cmd_overwritten = true;
                spd2010->madctl_val = ((uint8_t *)init_cmds[i].data)[0];
                break;
            case LCD_CMD_COLMOD:
                is_cmd_overwritten = true;
                spd2010->colmod_val = ((uint8_t *)init_cmds[i].data)[0];
                break;
            default:
                is_cmd_overwritten = false;
                break;
            }

            if (is_cmd_overwritten) {
                is_cmd_overwritten = false;
                ESP_LOGW(TAG, "The %02Xh command has been used and will be overwritten by external initialization sequence",
                         init_cmds[i].cmd);
            }
        }

        // Send command
        ESP_RETURN_ON_ERROR(tx_param(spd2010, io, init_cmds[i].cmd, init_cmds[i].data, init_cmds[i].data_bytes), TAG,
                            "send command failed");
        vTaskDelay(pdMS_TO_TICKS(init_cmds[i].delay_ms));

        // Check if the current cmd is the "command set" cmd
        if ((init_cmds[i].cmd == SPD2010_CMD_SET) && (init_cmds[i].data_bytes > 2)) {
            is_user_set = (((uint8_t *)init_cmds[i].data)[2] == SPD2010_CMD_SET_USER);
        }
    }
    ESP_LOGD(TAG, "send init commands success");

    return ESP_OK;
}

static esp_err_t panel_spd2010_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    spd2010_panel_t *spd2010 = __containerof(panel, spd2010_panel_t, base);
    assert((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");
    esp_lcd_panel_io_handle_t io = spd2010->io;

    x_start += spd2010->x_gap;
    x_end += spd2010->x_gap;
    y_start += spd2010->y_gap;
    y_end += spd2010->y_gap;

    // define an area of frame memory where MCU can access
    ESP_RETURN_ON_ERROR(tx_param(spd2010, io, LCD_CMD_CASET, (uint8_t[]) {
        (x_start >> 8) & 0xFF,
        x_start & 0xFF,
        ((x_end - 1) >> 8) & 0xFF,
        (x_end - 1) & 0xFF,
    }, 4), TAG, "send command failed");
    ESP_RETURN_ON_ERROR(tx_param(spd2010, io, LCD_CMD_RASET, (uint8_t[]) {
        (y_start >> 8) & 0xFF,
        y_start & 0xFF,
        ((y_end - 1) >> 8) & 0xFF,
        (y_end - 1) & 0xFF,
    }, 4), TAG, "send command failed");
    // transfer frame buffer
    size_t len = (x_end - x_start) * (y_end - y_start) * spd2010->fb_bits_per_pixel / 8;
    ESP_RETURN_ON_ERROR(tx_color(spd2010, io, LCD_CMD_RAMWR, color_data, len), TAG, "send color failed");

    return ESP_OK;
}

static esp_err_t panel_spd2010_invert_color(esp_lcd_panel_t *panel, bool invert_color_data)
{
    spd2010_panel_t *spd2010 = __containerof(panel, spd2010_panel_t, base);
    esp_lcd_panel_io_handle_t io = spd2010->io;
    int command = 0;
    if (invert_color_data) {
        command = LCD_CMD_INVON;
    } else {
        command = LCD_CMD_INVOFF;
    }
    ESP_RETURN_ON_ERROR(tx_param(spd2010, io, command, NULL, 0), TAG, "send command failed");
    return ESP_OK;
}

static esp_err_t panel_spd2010_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    spd2010_panel_t *spd2010 = __containerof(panel, spd2010_panel_t, base);
    esp_lcd_panel_io_handle_t io = spd2010->io;
    if (mirror_x) {
        spd2010->madctl_val |= BIT(1);
    } else {
        spd2010->madctl_val &= ~BIT(1);
    }
    if (mirror_y) {
        spd2010->madctl_val |= BIT(0);
    } else {
        spd2010->madctl_val &= ~BIT(0);
    }
    ESP_RETURN_ON_ERROR(tx_param(spd2010, io, LCD_CMD_MADCTL, (uint8_t[]) {
        spd2010->madctl_val
    }, 1), TAG, "send command failed");
    return ESP_OK;
}

static esp_err_t panel_spd2010_swap_xy(esp_lcd_panel_t *panel, bool swap_axes)
{
    // ESP_LOGE(TAG, "swap_xy is not supported by this panel");
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t panel_spd2010_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    spd2010_panel_t *spd2010 = __containerof(panel, spd2010_panel_t, base);
    spd2010->x_gap = x_gap;
    spd2010->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_spd2010_disp_on_off(esp_lcd_panel_t *panel, bool on_off)
{
    spd2010_panel_t *spd2010 = __containerof(panel, spd2010_panel_t, base);
    esp_lcd_panel_io_handle_t io = spd2010->io;
    int command = 0;

    if (on_off) {
        command = LCD_CMD_DISPON;
    } else {
        command = LCD_CMD_DISPOFF;
    }
    ESP_RETURN_ON_ERROR(tx_param(spd2010, io, command, NULL, 0), TAG, "send command failed");
    return ESP_OK;
}

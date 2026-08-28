#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2s_std.h"
#include "mic_driver.h"

static const char *TAG = "mic";

static i2s_chan_handle_t s_rx_chan = NULL;
static RingbufHandle_t   s_ringbuf = NULL;

#define MIC_RINGBUF_BYTES   (8 * 1024)          /* 环形缓冲 ~250ms @16k/16bit */
#define MIC_CAPTURE_SAMPLES 480                 /* 每次读 480 个样本(30ms) */

/* ---------- 后台采音任务: I2S读32bit → 取高16bit → 16bit PCM → 环形缓冲 ----------
 * 咪头输出 24bit 于 32bit slot 的高位(MSB先出), 低位8bit为三态后下拉的0;
 * 取 `raw >> 16` 即 24bit 的高 16bit, 得到标准 16bit PCM。
 * 缓冲数组放堆上(任务栈只 4KB, 大数组放栈上会溢出破坏内存)。 */
static void mic_capture_task(void *arg)
{
    int32_t *raw = malloc(MIC_CAPTURE_SAMPLES * sizeof(int32_t));
    int16_t *pcm = malloc(MIC_CAPTURE_SAMPLES * sizeof(int16_t));
    if (!raw || !pcm) {
        ESP_LOGE(TAG, "capture buf alloc fail");
        vTaskDelete(NULL);
        return;
    }
    while (1) {
        size_t bytes_read = 0;
        esp_err_t ret = i2s_channel_read(s_rx_chan, raw, MIC_CAPTURE_SAMPLES * sizeof(int32_t), &bytes_read, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "i2s read err 0x%x", ret);
            vTaskDelay(pdMS_TO_TICKS(100));   /* 失败时降频, 避免错误刷屏 */
            continue;
        }
        size_t n = bytes_read / sizeof(int32_t);
        /* I2S1 mono 配置下每帧采 2 个样本(2x 速率), 取偶数下标 → 16kHz 单声道。
         * 注: 若实测发现取错路(音量仍小), 改 raw[i+1] 试另一路。 */
        size_t m = 0;
        for (size_t i = 0; i + 1 < n; i += 2) {
            pcm[m++] = (int16_t)(raw[i] >> 16);
        }
        /* 缓冲满则丢弃本次(实时音频, 保新不保全) */
        xRingbufferSend(s_ringbuf, pcm, m * sizeof(int16_t), 0);
    }
}

esp_err_t mic_init(void)
{
    /* 1. 新建 I2S1 RX 通道
     *    注意: i2s_new_channel 参数顺序是 (chan_cfg, tx_handle, rx_handle)!
     *    只采音 → tx 传 NULL, rx 传 &s_rx_chan */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(MIC_I2S_PORT, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = 6;
    chan_cfg.dma_frame_num = 240;
    ESP_RETURN_ON_ERROR(i2s_new_channel(&chan_cfg, NULL, &s_rx_chan), TAG, "new rx chan");

    /* 2. 标准 I2S(Philips): 32bit slot, 单声道左声道(L/R=GND), 16kHz
     *    咪头是从机, 时钟由 ESP32 提供(BCLK=64×fWS) */
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(MIC_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = MIC_PIN_SCK,
            .ws   = MIC_PIN_WS,
            .dout = I2S_GPIO_UNUSED,
            .din  = MIC_PIN_DIN,
        },
    };
    ESP_RETURN_ON_ERROR(i2s_channel_init_std_mode(s_rx_chan, &std_cfg), TAG, "init std");

    /* 3. 环形缓冲 */
    s_ringbuf = xRingbufferCreate(MIC_RINGBUF_BYTES, RINGBUF_TYPE_BYTEBUF);
    if (s_ringbuf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    /* 4. 使能 + 启动采音任务 */
    ESP_RETURN_ON_ERROR(i2s_channel_enable(s_rx_chan), TAG, "enable rx");
    if (xTaskCreate(mic_capture_task, "mic_cap", 4096, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "capture task create fail");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "init ok: I2S%d %dHz mono, SCK=%d WS=%d DIN=%d",
             MIC_I2S_PORT, MIC_SAMPLE_RATE, MIC_PIN_SCK, MIC_PIN_WS, MIC_PIN_DIN);
    return ESP_OK;
}

/* 从环形缓冲读取 samples 个 16bit 样本 */
esp_err_t mic_read_pcm(int16_t *buf, size_t samples, int timeout_ms)
{
    size_t got = 0;
    while (got < samples) {
        size_t item_size = 0;
        uint8_t *item = xRingbufferReceive(s_ringbuf, &item_size, pdMS_TO_TICKS(timeout_ms));
        if (item == NULL) {
            return ESP_ERR_TIMEOUT;
        }
        size_t n = item_size / sizeof(int16_t);
        if (n > samples - got) {
            n = samples - got;
        }
        memcpy(buf + got, item, n * sizeof(int16_t));
        got += n;
        vRingbufferReturnItem(s_ringbuf, item);
    }
    return ESP_OK;
}

/* ---------- 测试: 3 秒内每 100ms 打印 RMS(dBFS)/峰值/电平条 ---------- */
static void mic_test_task(void *arg)
{
    const int frames = MIC_SAMPLE_RATE / 10;    /* 100ms @16kHz = 1600 样本 */
    int16_t *buf = malloc(frames * sizeof(int16_t));   /* 3200B, 放堆上 */
    if (!buf) {
        ESP_LOGE(TAG, "test buf alloc fail");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "--- mic test start: 对着咪头说话, 安静时应接近 -inf ---");
    for (int t = 0; t < 30; t++) {
        if (mic_read_pcm(buf, frames, 500) != ESP_OK) {
            ESP_LOGW(TAG, "read timeout");
            continue;
        }
        int64_t sum = 0;
        int16_t peak = 0;
        for (int i = 0; i < frames; i++) {
            sum += (int64_t)buf[i] * buf[i];
            int16_t a = buf[i] < 0 ? (int16_t)-buf[i] : buf[i];
            if (a > peak) peak = a;
        }
        double rms = sqrt((double)sum / frames);
        double db  = 20.0 * log10((rms + 1e-9) / 32768.0);
        int bars = (int)((db + 60.0) / 60.0 * 20);   /* -60..0dB 映射到 0..20 格 */
        if (bars < 0) bars = 0;
        if (bars > 20) bars = 20;
        char bar[21];
        for (int i = 0; i < 20; i++) bar[i] = (i < bars) ? '#' : ' ';
        bar[20] = '\0';
        ESP_LOGI(TAG, "rms=%7.1fdB  peak=%5d |%s|", db, peak, bar);
    }
    ESP_LOGI(TAG, "--- mic test done ---");
    free(buf);
    vTaskDelete(NULL);
}

void mic_test(void)
{
    if (xTaskCreate(mic_test_task, "mic_test", 4096, NULL, 6, NULL) != pdPASS) {
        ESP_LOGE(TAG, "test task create fail");
    }
}

#include "math.h"
#include <stdlib.h>
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"

static i2s_chan_handle_t spk_chan;

#define SPK_BCLK_GPIO    GPIO_NUM_41   // 接 MAX98357 BCLK
#define SPK_LRCK_GPIO    GPIO_NUM_48   // 接 MAX98357 LRCLK
#define SPK_DOUT_GPIO    GPIO_NUM_42   // 接 MAX98357 DIN

void speaker_init(void)
{
    // ① I2S 通道配置（DMA）
    i2s_chan_config_t chan_cfg = {
        .id = I2S_NUM_0,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 6,
        .dma_frame_num = 240,
        .auto_clear = true,          // 静音时输出 0
    };
    i2s_new_channel(&chan_cfg, &spk_chan, NULL);   // 只用 TX

    // ② 标准 I2S（Philips）配置：44.1kHz / 16bit / 立体声
    //    （改成 44100：匹配网易云 MP3 的采样率，否则音调会偏高）
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
                        I2S_DATA_BIT_WIDTH_16BIT,
                        I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,   // MAX98357 不需要 MCLK
            .bclk = SPK_BCLK_GPIO,
            .ws   = SPK_LRCK_GPIO,
            .dout = SPK_DOUT_GPIO,
            .din  = I2S_GPIO_UNUSED,
        },
    };
    i2s_channel_init_std_mode(spk_chan, &std_cfg);
    i2s_channel_enable(spk_chan);
}

/* 一次性播放（测试音用）：播完 disable */
void speaker_play(const int16_t *pcm, size_t samples)
{
    i2s_channel_enable(spk_chan);
    size_t written = 0;
    size_t total = samples * sizeof(int16_t);
    const uint8_t *p = (const uint8_t *)pcm;
    while (written < total) {
        size_t n = 0;
        i2s_channel_write(spk_chan, p + written, total - written, &n, portMAX_DELAY);
        written += n;
    }
    i2s_channel_disable(spk_chan);
}

/* 流式播放（音乐用）：保持 I2S 持续输出，不 enable/disable */
void speaker_play_stream(const int16_t *pcm, size_t samples)
{
    size_t written = 0;
    size_t total = samples * sizeof(int16_t);
    const uint8_t *p = (const uint8_t *)pcm;
    while (written < total) {
        size_t n = 0;
        i2s_channel_write(spk_chan, p + written, total - written, &n, portMAX_DELAY);
        written += n;
    }
}

void speaker_play_tone(int freq_hz, int duration_ms, float amplitude)
{
    size_t samples = 44100 * duration_ms / 1000;
    int16_t *buf = malloc(samples * 2 * sizeof(int16_t));
    for (size_t i = 0; i < samples; i++) {
        int16_t v = (int16_t)(amplitude * 32767 * sinf(2 * M_PI * freq_hz * i / 44100));
        buf[2 * i]     = v;
        buf[2 * i + 1] = v;
    }
    speaker_play(buf, samples * 2);
    free(buf);
}
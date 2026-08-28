#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "i2s_stream.h"      /* i2s_stream_borrow_tx(): 借 ADF 的 I2S0 TX 通道 */
#include "speaker.h"
#include "mic_driver.h"

static const char *TAG = "spk";

#define SPK_SAMPLE_RATE      16000   /* 小智 TTS / 咪头 采样率(16kHz 单声道) */
#define SPK_STEREO_CHUNK     480     /* 单声道→立体声复制分块 */
#define SPK_PLAY_GAIN_DEFAULT 14     /* 回放默认增益(咪头回环用; TTS 用 spk_set_gain 调低) */

static i2s_chan_handle_t s_tx = NULL;
static i2s_std_config_t  s_saved_cfg;   /* 借 ADF 通道时的原配置(44.1k stereo), 播完恢复 */
static bool              s_borrowed = false;
static float             s_gain = SPK_PLAY_GAIN_DEFAULT;

void spk_set_gain(float gain)
{
    s_gain = gain;
    ESP_LOGI(TAG, "gain set to %.2f", (double)gain);
}

/* 借 ADF 的 I2S0 TX 通道, 临时切到 16k/16bit/mono(小智格式)。
 * 前提: 音乐未在播(半双工互斥), 否则 ADF 的写任务会和这里抢通道。 */
static esp_err_t spk_acquire(void)
{
    if (s_borrowed) {
        return ESP_OK;
    }
    i2s_std_config_t cur;
    i2s_chan_handle_t tx = i2s_stream_borrow_tx(I2S_NUM_0, &cur);
    if (!tx) {
        ESP_LOGE(TAG, "borrow ADF I2S0 tx failed (music 未初始化?)");
        return ESP_ERR_NOT_FOUND;
    }
    s_saved_cfg = cur;
    s_tx = tx;

    /* 只把采样率切到 16k, slot 保持立体声 16bit(和音乐同一格式, MAX98357 才认)。
     * 写入时把单声道复制成立体声(见 spk_write_pcm)。 */
    i2s_std_config_t play = cur;
    play.clk_cfg.sample_rate_hz = SPK_SAMPLE_RATE;

    i2s_channel_disable(tx);
    ESP_RETURN_ON_ERROR(i2s_channel_reconfig_std_clock(tx, &play.clk_cfg), TAG, "reconfig clk");
    i2s_channel_enable(tx);
    s_borrowed = true;
    ESP_LOGI(TAG, "borrowed I2S0, switched to %dHz stereo", SPK_SAMPLE_RATE);
    return ESP_OK;
}

/* 恢复 ADF 原配置(44.1k stereo), 归还 I2S0 */
static void spk_release(void)
{
    if (!s_borrowed) {
        return;
    }
    i2s_channel_disable(s_tx);
    i2s_channel_reconfig_std_slot(s_tx, &s_saved_cfg.slot_cfg);
    i2s_channel_reconfig_std_clock(s_tx, &s_saved_cfg.clk_cfg);
    i2s_channel_enable(s_tx);
    s_borrowed = false;
    ESP_LOGI(TAG, "released I2S0, restored %dHz", s_saved_cfg.clk_cfg.sample_rate_hz);
}

esp_err_t spk_open(void)
{
    return spk_acquire();
}

esp_err_t spk_write_pcm(const int16_t *pcm, size_t samples)
{
    if (!s_tx || !s_borrowed) {
        return ESP_ERR_INVALID_STATE;
    }
    /* 单声道 → 立体声复制, 分块写入避免大缓冲 */
    int16_t stereo[SPK_STEREO_CHUNK * 2];
    size_t done = 0;
    while (done < samples) {
        size_t chunk = (samples - done) > SPK_STEREO_CHUNK ? SPK_STEREO_CHUNK : (samples - done);
        for (size_t i = 0; i < chunk; i++) {
            int32_t v = (int32_t)((float)pcm[done + i] * s_gain);   /* 放大 + 限幅 */
            if (v > 32767) {
                v = 32767;
            } else if (v < -32768) {
                v = -32768;
            }
            stereo[2 * i]     = (int16_t)v;
            stereo[2 * i + 1] = (int16_t)v;
        }
        size_t total = chunk * 2 * sizeof(int16_t);
        const uint8_t *p = (const uint8_t *)stereo;
        size_t written = 0;
        while (written < total) {
            size_t n = 0;
            /* 有限超时: 之前 portMAX_DELAY 卡住会占死 CPU 触发 task_wdt, 卡住就丢这一块 */
            esp_err_t ret = i2s_channel_write(s_tx, p + written, total - written, &n, pdMS_TO_TICKS(100));
            if (ret != ESP_OK || n == 0) {
                break;
            }
            written += n;
        }
        done += chunk;
    }
    return ESP_OK;
}

void spk_close(void)
{
    spk_release();
}

/* 回环测试: 录 seconds 秒(咪头) → 播放(喇叭), 验证 16k/16bit 全链路 */
void spk_loopback_test(int seconds)
{
    size_t samples = SPK_SAMPLE_RATE * seconds;
    int16_t *buf = malloc(samples * sizeof(int16_t));
    if (!buf) {
        ESP_LOGE(TAG, "loopback buf alloc fail");
        return;
    }

    ESP_LOGI(TAG, "--- recording %ds, 说话 ...", seconds);
    if (mic_read_pcm(buf, samples, seconds * 1000 + 1000) != ESP_OK) {
        ESP_LOGW(TAG, "record timeout");
        free(buf);
        return;
    }

    /* 诊断: 打印录音 RMS */
    int64_t sum = 0;
    for (size_t i = 0; i < samples; i++) {
        sum += (int64_t)buf[i] * buf[i];
    }
    double rms = sqrt((double)sum / samples);
    ESP_LOGI(TAG, "recorded rms=%.0f", rms);

    ESP_LOGI(TAG, "--- playing back (gain x%.2f) ...", (double)s_gain);
    if (spk_open() == ESP_OK) {
        spk_write_pcm(buf, samples);
        spk_close();
    }
    ESP_LOGI(TAG, "--- loopback done ---");
    free(buf);
}

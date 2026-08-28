#ifndef __SPEAKER_H_
#define __SPEAKER_H_

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * 小智放音(16kHz/16bit/单声道 PCM → MAX98357A, I2S0)
 * 按需 open/close, 不长期占用 I2S0(与 ADF 音乐共存: 谁播谁用)
 * 引脚: BCLK=41  LRCK=48  DOUT=42
 * ============================================================ */

/* 创建并启用 I2S0 TX 通道(16k/16bit/mono); 需 I2S0 空闲(音乐未在播) */
esp_err_t spk_open(void);

/* 阻塞写 samples 个 16bit 单声道样本 */
esp_err_t spk_write_pcm(const int16_t *pcm, size_t samples);

/* 禁用并删除通道, 释放 I2S0 */
void spk_close(void);

/* 回环测试: 用 mic_read_pcm 录 seconds 秒 → 从喇叭放出来(Step3 验证放音链路) */
void spk_loopback_test(int seconds);

/* 设置回放软件增益(默认 SPK_PLAY_GAIN=14; TTS 是服务器生成的幅度大, 建议设 1~2) */
void spk_set_gain(float gain);

#ifdef __cplusplus
}
#endif

#endif /* __SPEAKER_H_ */

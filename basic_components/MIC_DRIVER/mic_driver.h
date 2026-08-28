#ifndef __MIC_DRIVER_H_
#define __MIC_DRIVER_H_

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * MSM261S4030H0R 咪头驱动
 * 接口: I2S 输出 (标准 I2S/Philips, 从机, 24bit 数据 / 32bit 字长)
 * 接线: WS=IO38  SCK=IO39  DIN(=SD)=IO40
 * 说明: 板上 CHIPEN/L/R 已固定 (CHIPEN=VDD 使能 / L/R=GND 左声道), 软件不控制
 * ============================================================ */

#define MIC_I2S_PORT       1           /* I2S_NUM_1 (与喇叭 I2S0 分开, 互不影响) */
#define MIC_PIN_SCK        39          /* 位时钟 BCLK */
#define MIC_PIN_WS         38          /* 字选择 LRCK */
#define MIC_PIN_DIN        40          /* 串行数据 SD */
#define MIC_SAMPLE_RATE    16000       /* 16kHz 单声道, 小智 OPUS 用此采样率 */

/* 初始化 I2S1 采音: 启动后台采集任务, 持续把 16bit PCM 填进环形缓冲 */
esp_err_t mic_init(void);

/* 从环形缓冲读取 samples 个 16bit 单声道样本 (16kHz); 超时返回 ESP_ERR_TIMEOUT */
esp_err_t mic_read_pcm(int16_t *buf, size_t samples, int timeout_ms);

/* 测试: 读 3 秒, 每 100ms 打印 RMS(dBFS)/峰值/电平条, 对着咪头说话看跳变 */
void mic_test(void);

#ifdef __cplusplus
}
#endif

#endif /* __MIC_DRIVER_H_ */

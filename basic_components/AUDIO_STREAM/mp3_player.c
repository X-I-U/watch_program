#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "audio_pipeline.h"
#include "audio_element.h"
#include "http_stream.h"
#include "mp3_decoder.h"
#include "i2s_stream.h"
#include "driver/gpio.h"

static const char *TAG = "audio_player";

/* 播放任务：ADF pipeline 内部自带任务，这里开一个独立任务调用，避免阻塞 main */
static void audio_play_task(void *arg)
{
    const char *url = (const char *)arg;
    ESP_LOGI(TAG, "start playing: %s", url);

    /* ① I2S 输出配置（MAX98357 引脚，按你实际接线改！）
       注意：ADF release/v2.x 在 IDF>=5.0 用的是新 I2S API（std_cfg/chan_cfg），
       不再是老 ADF 的 i2s_config/i2s_pin_config。引脚从 std_cfg.gpio_cfg 取（本工程补丁后 i2s_stream 直接用它）。 */
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_cfg.std_cfg.clk_cfg.sample_rate_hz = 44100;      /* 网易云 MP3 一般 44.1k */
    i2s_cfg.std_cfg.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_16BIT;
    i2s_cfg.std_cfg.slot_cfg.slot_mode = I2S_SLOT_MODE_STEREO;
    /* 引脚必须全部显式赋值（含不用的），否则会用 0 号引脚 */
    i2s_cfg.std_cfg.gpio_cfg = (i2s_std_gpio_config_t){
        .mclk = I2S_GPIO_UNUSED,
        .bclk = GPIO_NUM_41,     /* ← 改成你的 BCLK 引脚 */
        .ws   = GPIO_NUM_48,     /* ← 改成你的 LRCK 引脚 */
        .dout = GPIO_NUM_42,     /* ← 改成你的 DOUT 引脚 */
        .din  = I2S_GPIO_UNUSED,
    };
    i2s_cfg.use_alc = true;    /* 音量控制依赖 ALC（否则 i2s_alc_volume_set 无效） */
    audio_element_handle_t i2s = i2s_stream_init(&i2s_cfg);
    i2s_alc_volume_set(i2s, 1);   /* 0~100；调小可减少低音电流尖峰，缓解 USB 供电复位 */

    /* ② HTTP 流输入 + MP3 解码器 */
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.type = AUDIO_STREAM_READER;
    audio_element_handle_t http = http_stream_init(&http_cfg);
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    audio_element_handle_t mp3 = mp3_decoder_init(&mp3_cfg);

    /* ③ 组成 pipeline：http → mp3 → i2s
       音量在 i2s_stream 的 _i2s_process 里统一缩放（见 adf_components/audio_stream/i2s_stream_idf5.c 的 LOCAL PATCH，
       I2S_SOFTWARE_GAIN 常量），自定义增益元素会触发任务看门狗，已弃用。 */
    audio_pipeline_handle_t pipeline = audio_pipeline_init(NULL);
    audio_pipeline_register(pipeline, http, "http");
    audio_pipeline_register(pipeline, mp3, "mp3");
    audio_pipeline_register(pipeline, i2s, "i2s");
    audio_pipeline_link(pipeline, (const char *[]){"http", "mp3", "i2s"}, 3);

    /* ④ 设 URL 并播放 */
    audio_element_set_uri(http, url);
    audio_pipeline_run(pipeline);

    ESP_LOGI(TAG, "pipeline running");
    /* 阻塞等待播放结束（可按需） */
    vTaskDelete(NULL);
}

/* 对外接口：开独立任务播放，不阻塞调用者 */
void mp3_player_play(const char *url)
{
    xTaskCreate(audio_play_task, "audio_play", 8192, (void *)url, 5, NULL);
}

/* 初始化占位：当前实现把 I2S/pipeline 都放在播放任务里，无需独立 init。
   若想把 pipeline 建成一次、多次播放复用，把 audio_play_task 里的 ①②③ 移到此处。 */
void mp3_player_init(void)
{
    ESP_LOGI(TAG, "mp3_player_init: nothing to do (pipeline built per play)");
}
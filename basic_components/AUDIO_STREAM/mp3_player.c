#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "audio_pipeline.h"
#include "audio_element.h"
#include "audio_event_iface.h"
#include "http_stream.h"
#include "mp3_decoder.h"
#include "i2s_stream.h"
#include "driver/gpio.h"

#include "mp3_player.h"

static const char *TAG = "mp3_player";

/* 持久管线的句柄（init 建一次，全程复用） */
static audio_pipeline_handle_t s_pipeline;
static audio_element_handle_t  s_http;
static audio_element_handle_t  s_mp3;
static audio_element_handle_t  s_i2s;

/* 当前播放 URL 拷贝到静态缓冲，保证 set_uri 后依然有效 */
static char s_url[192];

/* 管线是否已 run 过：切歌前需要先 stop 干净（仅对已启动的管线 stop） */
static bool s_started = false;

/* 管线事件接口：让上层（音乐任务）消费，避免 external queue 塞满 */
static audio_event_iface_handle_t s_evt;

void mp3_player_init(void)
{
    if (s_pipeline) {
        return;   /* 已初始化 */
    }
    ESP_LOGI(TAG, "init: build persistent pipeline");

    /* ① I2S 输出配置（MAX98357 引脚，按你实际接线改！）
       注意：ADF release/v2.x 在 IDF>=5.0 用新 I2S API（std_cfg/chan_cfg），
       引脚从 std_cfg.gpio_cfg 取（本工程补丁后 i2s_stream 直接用它）。 */
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
    i2s_cfg.use_alc = true;    /* 音量控制依赖 ALC */
    s_i2s = i2s_stream_init(&i2s_cfg);
    i2s_alc_volume_set(s_i2s, 1);
    //adf_components/audio_stream/i2s_stream_idf5.c里面可调节音量
    
    /* ② HTTP 流输入 + MP3 解码器 */
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.type = AUDIO_STREAM_READER;
    s_http = http_stream_init(&http_cfg);
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    s_mp3 = mp3_decoder_init(&mp3_cfg);

    /* ③ 组成 pipeline：http → mp3 → i2s
       音量/单声道混音在 i2s_stream 的 _i2s_process 里统一处理（见 adf_components/audio_stream/i2s_stream_idf5.c 的 LOCAL PATCH）。 */
    s_pipeline = audio_pipeline_init(NULL);
    audio_pipeline_register(s_pipeline, s_http, "http");
    audio_pipeline_register(s_pipeline, s_mp3, "mp3");
    audio_pipeline_register(s_pipeline, s_i2s, "i2s");
    audio_pipeline_link(s_pipeline, (const char *[]){"http", "mp3", "i2s"}, 3);

    /* 设置管线事件接口并由上层消费，防止 external queue 塞满导致元素命令丢失 */
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    s_evt = audio_event_iface_init(&evt_cfg);
    audio_pipeline_set_listener(s_pipeline, s_evt);

    ESP_LOGI(TAG, "init done");
}

void mp3_player_play(const char *url)
{
    if (!s_pipeline) {
        ESP_LOGE(TAG, "not initialized, call mp3_player_init first");
        return;
    }
    /* 切歌前先 stop，并重置元素/环形缓冲/管线状态，干净切换新 URL。
       参考 ADF living_stream 例子的 restart 写法，否则重复 stop/run 元素会卡死（AEL_IO_ABORT）。 */
    if (s_started) {
        audio_pipeline_stop(s_pipeline);
        audio_pipeline_wait_for_stop_with_ticks(s_pipeline, pdMS_TO_TICKS(2000));
        audio_element_reset_state(s_http);
        audio_element_reset_state(s_mp3);
        audio_element_reset_state(s_i2s);
        audio_pipeline_reset_ringbuffer(s_pipeline);
        audio_pipeline_reset_items_state(s_pipeline);
    }

    strncpy(s_url, url, sizeof(s_url) - 1);
    s_url[sizeof(s_url) - 1] = '\0';

    ESP_LOGI(TAG, "play: %s", s_url);
    audio_element_set_uri(s_http, s_url);
    audio_pipeline_run(s_pipeline);
    s_started = true;
}

void mp3_player_pause(void)
{
    if (s_pipeline) {
        audio_pipeline_pause(s_pipeline);
    }
}

void mp3_player_resume(void)
{
    if (s_pipeline) {
        audio_pipeline_resume(s_pipeline);
    }
}

mp3_player_state_t mp3_player_get_state(void)
{
    if (!s_pipeline) {
        return MP3_PLAYER_STOPPED;
    }
    switch (audio_element_get_state(s_i2s)) {
    case AEL_STATE_RUNNING:  return MP3_PLAYER_PLAYING;
    case AEL_STATE_PAUSED:   return MP3_PLAYER_PAUSED;
    default:                 return MP3_PLAYER_STOPPED;
    }
}

/* 取进度信息：byte_pos/bps/duration 从 mp3 解码器取，total_bytes 从 http 元素取
   （http 有 Content-Length，mp3 元素不会自己带 total_bytes） */
static void player_get_info(audio_element_info_t *info)
{
    memset(info, 0, sizeof(*info));
    if (s_mp3) {
        audio_element_getinfo(s_mp3, info);
    }
    if (s_http) {
        audio_element_info_t http_info = { 0 };
        audio_element_getinfo(s_http, &http_info);
        if (http_info.total_bytes > 0) {
            info->total_bytes = http_info.total_bytes;
        }
    }
}

int mp3_player_get_progress_percent(void)
{
    audio_element_info_t info;
    player_get_info(&info);

    /* 进度调试日志（5s 一次），用于排查 total_bytes/duration 是否可得 */
    static int32_t s_last_log_ms = 0;
    int32_t now_ms = (int32_t)(esp_timer_get_time() / 1000);
    if (now_ms - s_last_log_ms > 5000) {
        s_last_log_ms = now_ms;
        ESP_LOGI(TAG, "prog info: pos=%lld total=%lld bps=%d dur=%d heap=%d iheap=%d",
                 (long long)info.byte_pos, (long long)info.total_bytes,
                 info.bps, info.duration,
                 (int)esp_get_free_heap_size(),
                 (int)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    }

    /* 优先用 total_bytes（HTTP 有 Content-Length 时）；否则用 duration（mp3 解码器可能给出） */
    if (info.total_bytes > 0) {
        int pct = (int)(info.byte_pos * 100 / info.total_bytes);
        if (pct < 0) pct = 0;
        if (pct > 100) pct = 100;
        return pct;
    }
    if (info.duration > 0 && info.bps > 0) {
        int pos_sec = (int)(info.byte_pos * 8 / info.bps);
        int pct = (int)(pos_sec * 100 / info.duration);
        if (pct < 0) pct = 0;
        if (pct > 100) pct = 100;
        return pct;
    }
    return 0;
}

int mp3_player_get_position_sec(void)
{
    audio_element_info_t info;
    player_get_info(&info);
    if (info.bps <= 0) {
        return 0;
    }
    return (int)(info.byte_pos * 8 / info.bps);
}

int mp3_player_get_duration_sec(void)
{
    audio_element_info_t info;
    player_get_info(&info);
    if (info.bps <= 0) {
        return 0;
    }
    return (int)(info.total_bytes * 8 / info.bps);
}

/* 暴露管线事件接口给上层（音乐任务）消费，防止 external queue 塞满 */
void *mp3_player_get_event_iface(void)
{
    return s_evt;
}

#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_event.h"
#include "esp_heap_caps.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "esp_xiaozhi_chat.h"
#include "esp_xiaozhi_info.h"
#include "esp_mcp_engine.h"
#include "mic_driver.h"
#include "speaker.h"
#include "wifista.h"
#include "xiaozhi.h"
#include "audio_element.h"
#include "esp_audio_simple_dec.h"
#include "esp_audio_dec_default.h"
#include "esp_opus_dec.h"
#include "esp_audio_enc.h"
#include "esp_opus_enc.h"

static const char *TAG = "xiaozhi";

static esp_xiaozhi_chat_handle_t s_chat = 0;

/* ---------- 会话状态(半双工: 听时不放, 放时不听) ---------- */
static volatile bool s_connected  = false;   /* 已连上服务器 */
static volatile bool s_session    = false;   /* 音频通道已开 */
static volatile bool s_listening  = false;   /* 正在采音上传 */
static volatile bool s_speaking   = false;   /* 正在播 TTS */

#define MIC_FRAME      960                   /* 采音一帧 60ms @16k(匹配 OPUS 帧长) */
/* 上行编码 + 下行解码都用官方 esp_audio_codec: mic 任务直接调 enc_process, audio_cb 直接调 dec_process */
static esp_audio_enc_handle_t    s_enc        = NULL;
static int                       s_enc_in_sz  = 1920;   /* 编码器输入帧大小(60ms@16k mono PCM) */
static int                       s_enc_out_sz = 512;    /* 编码器输出缓冲(OPUS) */
static ringbuf_handle_t          s_dec_in     = NULL;   /* [2B长度][OPUS帧], websocket任务写, 播放任务读 */
static esp_audio_simple_dec_handle_t s_simple_dec = NULL;
static uint32_t s_audio_rx_frames = 0;   /* [diag] 服务器音频帧计数 */
static uint32_t s_play_frames     = 0;   /* [diag] 播放取出字节计数 */

static void xiaozhi_conv_task(void *arg);

/* ---------- ESP 事件系统: 连接/音频通道状态 ---------- */
static void xiaozhi_esp_event_handler(void *arg, esp_event_base_t base, int32_t id, void *event_data)
{
    switch (id) {
    case ESP_XIAOZHI_CHAT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "===== connected to xiaozhi server =====");
        s_connected = true;
        break;
    case ESP_XIAOZHI_CHAT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "disconnected from server");
        s_connected = false;
        s_session = s_listening = s_speaking = false;
        break;
    case ESP_XIAOZHI_CHAT_EVENT_AUDIO_CHANNEL_OPENED:
        ESP_LOGI(TAG, "audio channel opened (start listening)");
        s_session   = true;
        s_listening = true;
        /* 告诉服务器"用户开始说话"(类似唤醒词触发), 否则服务器不启动 ASR */
        esp_err_t sl_ret = esp_xiaozhi_chat_send_start_listening(s_chat, ESP_XIAOZHI_CHAT_LISTENING_MODE_AUTO);
        ESP_LOGI(TAG, "send_start_listening ret=0x%x", sl_ret);
        break;
    case ESP_XIAOZHI_CHAT_EVENT_AUDIO_CHANNEL_CLOSED:
        ESP_LOGI(TAG, "audio channel closed");
        s_session   = false;
        s_listening = false;
        s_speaking  = false;
        break;
    default:
        break;
    }
}

/* ---------- audio_callback(websocket任务上下文, 必须轻): 只把 [2B长度][OPUS帧] 写进缓冲, 解码放播放任务 ---------- */
static void xiaozhi_audio_cb(const uint8_t *data, int len, void *ctx)
{
    if (s_dec_in && len > 0 && len <= 512) {
        uint8_t hdr[2] = { (uint8_t)(len >> 8), (uint8_t)(len & 0xFF) };
        rb_write(s_dec_in, (char *)hdr, 2, pdMS_TO_TICKS(50));
        rb_write(s_dec_in, (char *)data, len, pdMS_TO_TICKS(50));
        if ((++s_audio_rx_frames % 50) == 1) {
            ESP_LOGI(TAG, "[diag] audio rx frame len=%d (total %lu)", len, (unsigned long)s_audio_rx_frames);
        }
    }
}

/* ---------- 事件回调: 状态机 + 日志 ---------- */
static void xiaozhi_event_cb(esp_xiaozhi_chat_event_t event, void *event_data, void *ctx)
{
    switch (event) {
    case ESP_XIAOZHI_CHAT_EVENT_CHAT_TTS_STATE: {
        esp_xiaozhi_chat_tts_state_t *st = (esp_xiaozhi_chat_tts_state_t *)event_data;
        if (st->state == ESP_XIAOZHI_CHAT_TTS_STATE_START) {
            s_listening = false;      /* 停采音(半双工) */
            s_speaking  = true;
            spk_open();               /* 借 I2S0 切 16k */
            ESP_LOGI(TAG, "[tts] speaking start");
        } else if (st->state == ESP_XIAOZHI_CHAT_TTS_STATE_STOP) {
            s_speaking = false;
            spk_close();              /* 恢复 44.1k 还通道 */
            if (s_dec_in) {
                rb_reset(s_dec_in);   /* 清掉缓冲的旧帧, 防下轮播放残留 */
            }
            /* 连续对话: 说完恢复采音 + 重新告诉服务器"继续听" */
            if (s_session) {
                s_listening = true;
                esp_xiaozhi_chat_send_start_listening(s_chat, ESP_XIAOZHI_CHAT_LISTENING_MODE_AUTO);
            }
            ESP_LOGI(TAG, "[tts] speaking stop, listening again");
        }
        break;
    }
    case ESP_XIAOZHI_CHAT_EVENT_CHAT_TEXT: {
        esp_xiaozhi_chat_text_data_t *t = (esp_xiaozhi_chat_text_data_t *)event_data;
        ESP_LOGI(TAG, "[text] %s: %s",
                 (t->role == ESP_XIAOZHI_CHAT_TEXT_ROLE_USER) ? "user" : "ai",
                 t->text ? t->text : "");
        break;
    }
    case ESP_XIAOZHI_CHAT_EVENT_CHAT_ERROR: {
        esp_xiaozhi_chat_error_info_t *err = (esp_xiaozhi_chat_error_info_t *)event_data;
        ESP_LOGE(TAG, "chat error 0x%x (%s)", err->code, err->source ? err->source : "?");
        break;
    }
    case ESP_XIAOZHI_CHAT_EVENT_CHAT_SYSTEM_CMD:
        ESP_LOGW(TAG, "system cmd: %s", (const char *)event_data);
        break;
    default:
        break;   /* EMOJI 等暂不处理 */
    }
}

/* ---------- 采音任务: 听的状态下读 mic → OPUS 编码 → 传服务器 ---------- */
static void xiaozhi_mic_task(void *arg)
{
    int16_t buf[MIC_FRAME];
    uint8_t opus_out[512];   /* OPUS 60ms@24kbps mono <200B, 512 足够 */
    uint32_t frames = 0;
    while (1) {
        if (s_session && s_listening) {
            if (mic_read_pcm(buf, MIC_FRAME, 200) == ESP_OK && s_enc) {
                /* 官方编码器: 一帧 PCM 直接 process 出一帧 OPUS, 无元素任务 */
                esp_audio_enc_in_frame_t in = {
                    .buffer = (uint8_t *)buf, .len = (uint32_t)s_enc_in_sz,
                };
                esp_audio_enc_out_frame_t out = {
                    .buffer = opus_out, .len = sizeof(opus_out),
                };
                esp_audio_err_t ret = esp_audio_enc_process(s_enc, &in, &out);
                if (ret == ESP_AUDIO_ERR_OK && out.encoded_bytes > 0) {
                    esp_xiaozhi_chat_send_audio_data(s_chat, (char *)opus_out, out.encoded_bytes);
                    if ((++frames % 100) == 0) {   /* 每 ~6s 打一次 */
                        ESP_LOGI(TAG, "mic feeding... (%lu frames sent)", (unsigned long)frames);
                    }
                }
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}

/* ---------- 播放任务: 说的时候从缓冲读 [2B长度][OPUS帧] → 解码 → PCM 写喇叭(栈 PSRAM 32KB) ---------- */
static void xiaozhi_play_task(void *arg)
{
    uint8_t opus_buf[512];
    int16_t pcm[2048];   /* 60ms@16k mono=1920B, 2048 够 */
#ifdef CONFIG_ESP_TASK_WDT_EN
    esp_task_wdt_add(NULL);   /* 解码可能超过5s, 主动喂狗防 WDT */
#endif
    while (1) {
#ifdef CONFIG_ESP_TASK_WDT_EN
        esp_task_wdt_reset();
#endif
        if (s_speaking && s_dec_in && s_simple_dec) {
            uint8_t hdr[2];
            if (rb_read(s_dec_in, (char *)hdr, 2, pdMS_TO_TICKS(100)) != 2) {
                continue;
            }
            uint16_t flen = (uint16_t)((hdr[0] << 8) | hdr[1]);
            if (flen == 0 || flen > sizeof(opus_buf)) {
                continue;   /* 非法长度, 丢帧 */
            }
            if (rb_read(s_dec_in, (char *)opus_buf, flen, pdMS_TO_TICKS(100)) != flen) {
                continue;
            }
            esp_audio_simple_dec_raw_t raw = {
                .buffer = opus_buf, .len = flen, .eos = false,
            };
            esp_audio_simple_dec_out_t frame = {
                .buffer = (uint8_t *)pcm, .len = sizeof(pcm),
            };
            if (esp_audio_simple_dec_process(s_simple_dec, &raw, &frame) == ESP_AUDIO_ERR_OK
                && frame.decoded_size > 0) {
                if ((s_play_frames++ % 50) == 0) {
                    ESP_LOGI(TAG, "[diag] play pcm bytes=%u", (unsigned)frame.decoded_size);
                }
                spk_write_pcm(pcm, frame.decoded_size / sizeof(int16_t));
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

esp_err_t xiaozhi_init(void)
{
    /* 1. 等 WiFi 连上(最多 20s) */
    if (!wifi_wait_connected(20000)) {
        ESP_LOGE(TAG, "WiFi not connected, abort");
        return ESP_ERR_NOT_FOUND;
    }
    ESP_LOGI(TAG, "WiFi connected, server: %s", CONFIG_XIAOZHI_OTA_URL);

    /* 2. get_info: 拿激活码 / 绑定状态 / 传输配置。
     *    TLS/网络偶发失败(如 -0x0050 X509_INVALID_NAME), 重试几次。 */
    esp_xiaozhi_chat_info_t info = { 0 };
    esp_err_t ret = ESP_FAIL;
    for (int attempt = 1; attempt <= 5; attempt++) {
        esp_xiaozhi_chat_free_info(&info);   /* 清上次残留, 防泄漏 */
        memset(&info, 0, sizeof(info));
        ret = esp_xiaozhi_chat_get_info(&info);
        if (ret == ESP_OK) {
            break;
        }
        ESP_LOGW(TAG, "get_info failed (0x%x), retry %d/5 ...", ret, attempt);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    ESP_RETURN_ON_ERROR(ret, TAG, "get_info failed after retries");

    if (info.has_activation_code) {
        /* 未绑定: 打印 6 位激活码, 去网页控制台添加设备 */
        ESP_LOGW(TAG, "========================================");
        ESP_LOGW(TAG, "设备未绑定! 6 位激活码 = [ %s ]", info.activation_code);
        ESP_LOGW(TAG, "打开 xiaozhi.me 控制台 → 添加设备 → 输入此码");
        ESP_LOGW(TAG, "========================================");
        esp_xiaozhi_chat_free_info(&info);
        return ESP_OK;   /* 绑定后再 connect */
    }

    if (info.has_serial_number) {
        ESP_LOGI(TAG, "device already bound (sn=%s)", info.serial_number);
    }

    ESP_LOGI(TAG, "free mem: internal=%zu  total(8bit incl psram)=%zu",
             heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
             heap_caps_get_free_size(MALLOC_CAP_8BIT));

    /* 3. 已绑定: 注册事件 + 建 MCP engine + chat_init + start */
    ESP_RETURN_ON_ERROR(esp_event_handler_register(ESP_XIAOZHI_CHAT_EVENTS, ESP_EVENT_ANY_ID,
                                                   xiaozhi_esp_event_handler, NULL),
                        TAG, "reg event handler");

    esp_mcp_t *mcp = NULL;
    ESP_RETURN_ON_ERROR(esp_mcp_create(&mcp), TAG, "mcp create failed");

    ESP_LOGI(TAG, "server transport options: mqtt=%d websocket=%d",
             info.has_mqtt_config, info.has_websocket_config);

    esp_xiaozhi_chat_config_t cfg = ESP_XIAOZHI_CHAT_DEFAULT_CONFIG();
    cfg.event_callback       = xiaozhi_event_cb;
    cfg.audio_callback       = xiaozhi_audio_cb;
    cfg.audio_callback_ctx   = NULL;
    cfg.has_mqtt_config      = false;  /* 强制 WebSocket: MQTT 需 UDP 音频, 网络环境易断 */
    cfg.has_websocket_config = info.has_websocket_config;
    cfg.mcp_engine           = mcp;
    cfg.owns_mcp_engine      = true;   /* chat deinit 时一并销毁 mcp */
    esp_xiaozhi_chat_free_info(&info);

    ESP_RETURN_ON_ERROR(esp_xiaozhi_chat_init(&cfg, &s_chat), TAG, "chat init failed");

    /* 4. 上行 OPUS 编码: 官方 esp_audio_enc, mic 任务直接调 process, 无元素任务不卡死 */
    esp_opus_enc_register();   /* 只注册 OPUS 编码器 */
    esp_opus_enc_config_t ocfg = ESP_OPUS_ENC_CONFIG_DEFAULT();
    ocfg.sample_rate       = 16000;
    ocfg.channel           = ESP_AUDIO_MONO;
    ocfg.bits_per_sample   = ESP_AUDIO_BIT16;
    ocfg.bitrate           = 24000;   /* 小智默认 24kbps */
    ocfg.frame_duration    = ESP_OPUS_ENC_FRAME_DURATION_60_MS;
    ocfg.application_mode  = ESP_OPUS_ENC_APPLICATION_VOIP;
    ocfg.enable_vbr        = false;
    esp_audio_enc_config_t enc_cfg = {
        .type   = ESP_AUDIO_TYPE_OPUS,
        .cfg    = &ocfg,
        .cfg_sz = sizeof(ocfg),
    };
    if (esp_audio_enc_open(&enc_cfg, &s_enc) == ESP_AUDIO_ERR_OK) {
        esp_audio_enc_get_frame_size(s_enc, &s_enc_in_sz, &s_enc_out_sz);
        ESP_LOGI(TAG, "simple opus encoder ready (in=%d out=%d)", s_enc_in_sz, s_enc_out_sz);
    } else {
        ESP_LOGE(TAG, "simple opus encoder open failed");
    }

    /* 4b. 下行 OPUS 解码: 官方 esp_audio_simple_dec, 每个 websocket 音频消息=一帧, 直接解 */
    esp_opus_dec_register();   /* 只注册 OPUS(不能用 register_default: 会拉全量解码器, 和 esp_codec 链接冲突) */
    esp_opus_dec_cfg_t opus_cfg = {
        .sample_rate    = 16000,
        .channel        = ESP_AUDIO_MONO,
        .frame_duration = ESP_OPUS_DEC_FRAME_DURATION_60_MS,
        .self_delimited = false,
    };
    esp_audio_simple_dec_cfg_t sdec_cfg = {
        .dec_type      = ESP_AUDIO_SIMPLE_DEC_TYPE_RAW_OPUS,
        .dec_cfg       = &opus_cfg,
        .cfg_size      = sizeof(opus_cfg),
        .use_frame_dec = true,   /* 输入是完整一帧, 直接解不用解析 */
    };
    if (esp_audio_simple_dec_open(&sdec_cfg, &s_simple_dec) == ESP_AUDIO_ERR_OK) {
        s_dec_in = rb_create(1024, 4);   /* OPUS 帧缓冲 4KB(带2B长度前缀) */
        ESP_LOGI(TAG, "simple opus decoder ready");
    } else {
        ESP_LOGE(TAG, "simple opus decoder open failed");
    }

    /* 5. 采音/播放任务(栈放 PSRAM, 不占内部 RAM) */
    xTaskCreatePinnedToCoreWithCaps(xiaozhi_mic_task, "xz_mic", 40 * 1024, NULL, 5, NULL, tskNO_AFFINITY,
                                    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);   /* OPUS编码吃栈, ADF 元素也是40KB */
    xTaskCreatePinnedToCoreWithCaps(xiaozhi_play_task, "xz_play", 32 * 1024, NULL, 6, NULL, tskNO_AFFINITY,
                                    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);   /* OPUS解码吃栈(参考30KB) */
    xTaskCreatePinnedToCoreWithCaps(xiaozhi_conv_task, "xz_conv", 8192, NULL, 3, NULL, tskNO_AFFINITY,
                                    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);   /* 连续对话保活: 通道关了自动重开 */
    spk_set_gain(0.35f);   /* TTS 满幅+MAX98357高增益→电流大→欠压重启, 压到 1/4 幅度 */

    ESP_RETURN_ON_ERROR(esp_xiaozhi_chat_start(s_chat), TAG, "chat start failed");
    ESP_LOGI(TAG, "chat started (waiting for events)");
    return ESP_OK;
}

esp_err_t xiaozhi_talk(void)
{
    if (!s_chat) {
        ESP_LOGE(TAG, "chat not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    if (!s_connected) {
        ESP_LOGW(TAG, "not connected yet");
        return ESP_ERR_INVALID_STATE;
    }
    if (s_session) {
        ESP_LOGW(TAG, "session already active");
        return ESP_OK;
    }
    /* 用 OPUS(服务器标准格式): 采音编码成 OPUS 上传, TTS 收 OPUS 解码播放 */
    esp_xiaozhi_chat_audio_t audio = {
        .format         = "opus",
        .sample_rate    = 16000,
        .channels       = 1,
        .frame_duration = 60,
    };
    ESP_RETURN_ON_ERROR(esp_xiaozhi_chat_open_audio_channel(s_chat, &audio, NULL, 0),
                        TAG, "open audio channel");
    ESP_LOGI(TAG, "talk: audio channel opening (opus 16k mono)");
    return ESP_OK;
}

/* 连续对话保活: 连上且无会话时周期重开音频通道(首次连接 / 服务器关通道后) */
static void xiaozhi_conv_task(void *arg)
{
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (s_connected && !s_session && !s_speaking) {
            xiaozhi_talk();
        }
    }
}

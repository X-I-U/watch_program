#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "audio_event_iface.h"
#include "mp3_player.h"
#include "music_core.h"

static const char *TAG = "music_core";

/* 歌单：加歌在这里加一行 { 歌名, 播放URL }。
   UI(播放页列表) 会按这个表动态生成条目，数量不受限制。 */
static const music_song_t s_songs[] = {
    { "晴天", "http://music.163.com/song/media/outer/url?id=2652820720.mp3" },
    { "花海", "http://music.163.com/song/media/outer/url?id=2659569861.mp3" },
    { "红色高跟鞋", "http://music.163.com/song/media/outer/url?id=2046829393.mp3"},
    { "Take Me Hand", "http://music.163.com/song/media/outer/url?id=2071452224.mp3"},
    { "Love is gone", "http://music.163.com/song/media/outer/url?id=2724994876.mp3"}
};
#define MUSIC_SONG_COUNT (sizeof(s_songs) / sizeof(s_songs[0]))

/* 命令队列：LVGL 层只投递命令（快速返回），真正的音频控制（stop/run 等深调用）在专用音乐任务里执行。
   原因：LVGL 任务栈只有 4KB，直接调 audio 的 stop/run（HTTP 断开/TLS/I2S 清理）会栈溢出崩溃。 */
typedef enum {
    MUSIC_CMD_PLAY = 1,   /* 播当前选中（停止时从头播，暂停时恢复） */
    MUSIC_CMD_PLAY_IDX,   /* 播指定 index */
    MUSIC_CMD_PAUSE,
    MUSIC_CMD_RESUME,
    MUSIC_CMD_STOP,
    MUSIC_CMD_NEXT,
    MUSIC_CMD_PREV,
} music_cmd_t;

typedef struct {
    music_cmd_t cmd;
    int         index;
} music_msg_t;

static QueueHandle_t s_cmd_queue;
static int  s_index   = 0;
static bool s_playing = false;   /* 意图标记（音乐任务写入，LVGL 只读，单字节访问安全） */

/* ---------- 以下在音乐任务上下文中执行（栈充足） ---------- */
static void do_play_index(int index)
{
    if (index < 0 || index >= (int)MUSIC_SONG_COUNT) {
        ESP_LOGW(TAG, "bad index %d", index);
        return;
    }
    s_index   = index;
    s_playing = true;
    /* URL 直接存在歌单表里（HTTP，会 302 到 CDN，http_stream 自动跟随） */
    const char *url = s_songs[index].url;
    ESP_LOGI(TAG, "play #%d %s (%s) heap=%d", index, s_songs[index].title, url,
             (int)esp_get_free_heap_size());
    mp3_player_play(url);
}

static void do_resume(void)
{
    if (mp3_player_get_state() == MP3_PLAYER_STOPPED) {
        /* 已停（而不是暂停）→ 重播当前曲目 */
        do_play_index(s_index);
        return;
    }
    s_playing = true;
    mp3_player_resume();
}

static void music_task(void *arg)
{
    music_msg_t msg;
    audio_event_iface_handle_t evt = (audio_event_iface_handle_t)mp3_player_get_event_iface();
    while (1) {
        /* 命令优先（非阻塞） */
        if (xQueueReceive(s_cmd_queue, &msg, 0) == pdTRUE) {
            switch (msg.cmd) {
            case MUSIC_CMD_PLAY:
                do_play_index(s_index);
                break;
            case MUSIC_CMD_PLAY_IDX:
                do_play_index(msg.index);
                break;
            case MUSIC_CMD_PAUSE:
                s_playing = false;
                mp3_player_pause();
                break;
            case MUSIC_CMD_RESUME:
                do_resume();
                break;
            case MUSIC_CMD_NEXT:
                do_play_index((s_index + 1) % (int)MUSIC_SONG_COUNT);
                break;
            case MUSIC_CMD_PREV:
                do_play_index((s_index - 1 + (int)MUSIC_SONG_COUNT) % (int)MUSIC_SONG_COUNT);
                break;
            case MUSIC_CMD_STOP:
                s_playing = false;
                mp3_player_stop();   /* 彻底停(释放内部 RAM), 给小智等让内存 */
                break;
            default:
                break;
            }
        }
        /* 消费管线事件（短超时，防止 external queue 塞满，顺带让出 CPU） */
        if (evt) {
            audio_event_iface_msg_t evt_msg;
            audio_event_iface_listen(evt, &evt_msg, pdMS_TO_TICKS(20));
        }
    }
}

static void post_cmd(music_cmd_t cmd, int index)
{
    if (!s_cmd_queue) {
        return;
    }
    music_msg_t msg = { .cmd = cmd, .index = index };
    xQueueSend(s_cmd_queue, &msg, 0);   /* 非阻塞投递，失败即丢弃（队列满时忽略） */
}

/* ---------- 对外 API（LVGL 调用，只投递命令，快速返回） ---------- */
void music_core_init(void)
{
    mp3_player_init();
    s_index   = 0;
    s_playing = false;
    s_cmd_queue = xQueueCreate(8, sizeof(music_msg_t));
    xTaskCreate(music_task, "music", 8192, NULL, 5, NULL);
    ESP_LOGI(TAG, "init, %d songs", (int)MUSIC_SONG_COUNT);
}

void music_core_play_index(int index) { post_cmd(MUSIC_CMD_PLAY_IDX, index); }
void music_core_pause(void)           { post_cmd(MUSIC_CMD_PAUSE, 0); }
void music_core_resume(void)          { post_cmd(MUSIC_CMD_RESUME, 0); }
void music_core_stop(void)            { post_cmd(MUSIC_CMD_STOP, 0); }
void music_core_next(void)            { post_cmd(MUSIC_CMD_NEXT, 0); }
void music_core_prev(void)            { post_cmd(MUSIC_CMD_PREV, 0); }

music_state_t music_core_get_state(void)
{
    switch (mp3_player_get_state()) {
    case MP3_PLAYER_PLAYING: return MUSIC_STATE_PLAYING;
    case MP3_PLAYER_PAUSED:  return MUSIC_STATE_PAUSED;
    default:                 return MUSIC_STATE_STOPPED;
    }
}

bool music_core_is_playing(void)
{
    return s_playing;
}

int music_core_get_index(void)
{
    return s_index;
}

const char *music_core_get_title(void)
{
    if (s_index < 0 || s_index >= (int)MUSIC_SONG_COUNT) {
        return "";
    }
    return s_songs[s_index].title;
}

int music_core_get_song_count(void)
{
    return (int)MUSIC_SONG_COUNT;
}

const char *music_core_get_song_title(int index)
{
    if (index < 0 || index >= (int)MUSIC_SONG_COUNT) {
        return "";
    }
    return s_songs[index].title;
}

const char *music_core_get_song_url(int index)
{
    if (index < 0 || index >= (int)MUSIC_SONG_COUNT) {
        return "";
    }
    return s_songs[index].url;
}

int music_core_get_progress_percent(void)
{
    return mp3_player_get_progress_percent();
}

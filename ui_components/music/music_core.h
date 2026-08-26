#ifndef __MUSIC_CORE_H_
#define __MUSIC_CORE_H_

#include <stdbool.h>

#define MUSIC_SONG_COUNT 2

typedef enum {
    MUSIC_STATE_STOPPED = 0,   /* 未在播（含一首播完） */
    MUSIC_STATE_PLAYING,
    MUSIC_STATE_PAUSED,
} music_state_t;

/* 音乐业务核心：歌单 + 播放状态机。
   只依赖 basic_components 的 mp3_player 引擎，不依赖 LVGL —— 与 UI 解耦。 */
void music_core_init(void);
void music_core_play_index(int index);   /* 播指定歌（0 起） */
void music_core_pause(void);
void music_core_resume(void);
void music_core_next(void);
void music_core_prev(void);
music_state_t music_core_get_state(void);
bool music_core_is_playing(void);        /* 是否"意图在播"（用于自动连播判断） */
int  music_core_get_index(void);
const char *music_core_get_title(void);
int  music_core_get_progress_percent(void);

#endif

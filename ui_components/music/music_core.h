#ifndef __MUSIC_CORE_H_
#define __MUSIC_CORE_H_

#include <stdbool.h>

/* 单首歌：歌名 + 可直接播放的 URL（mp3）。
   加新歌 = 在 music_core.c 的 s_songs 表里加一行即可，
   播放页 UI 会自动按歌单生成一个可点击的列表条目（歌名/URL 都在这绑定）。
   网易云外链完整地址格式：http://music.163.com/song/media/outer/url?id=<歌曲id>.mp3 */
typedef struct {
    const char *title;   /* 歌名。任意中文都能显示(UI 用 GB2312 全字集字体，同小智对话) */
    const char *url;     /* 播放地址(mp3/http, 会 302 自动跟随) */
} music_song_t;

int  music_core_get_song_count(void);   /* 歌单歌曲数(= s_songs 表行数) */
const char *music_core_get_song_title(int index);
const char *music_core_get_song_url(int index);

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
void music_core_stop(void);              /* 彻底停止(停解码/停流), 给其它功能腾内部 RAM; 之后可用 play/resume 重播 */
void music_core_next(void);
void music_core_prev(void);
music_state_t music_core_get_state(void);
bool music_core_is_playing(void);        /* 是否"意图在播"（用于自动连播判断） */
int  music_core_get_index(void);
const char *music_core_get_title(void);
int  music_core_get_progress_percent(void);

#endif

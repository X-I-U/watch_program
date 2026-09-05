#ifndef __MP3_PLAYER_H_
#define __MP3_PLAYER_H_

#include <stdbool.h>

/* 播放器对外状态 */
typedef enum {
    MP3_PLAYER_STOPPED = 0,   /* 未在播（含播放结束） */
    MP3_PLAYER_PLAYING,
    MP3_PLAYER_PAUSED,
} mp3_player_state_t;

/* 持久管线：init 建一次，play/pause/resume 控制同一管线，支持切歌/暂停/进度查询 */
void mp3_player_init(void);              /* 初始化并建立持久管线（只调一次） */
void mp3_player_play(const char *url);   /* 切到该 URL 并开始播放（自动停止当前） */
void mp3_player_pause(void);
void mp3_player_resume(void);
void mp3_player_stop(void);              /* 彻底停止: 停解码/停流并重置状态(释放内部占用), 之后可再 play */
mp3_player_state_t mp3_player_get_state(void);
int  mp3_player_get_progress_percent(void);  /* 0~100 */
int  mp3_player_get_position_sec(void);
int  mp3_player_get_duration_sec(void);
void *mp3_player_get_event_iface(void);      /* 管线事件接口，供上层消费防队列塞满 */

#endif

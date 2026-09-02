#ifndef __MUSIC_UI_H_
#define __MUSIC_UI_H_

#include "gui_guider.h"

//在event_init.c中添加#include "music_ui.h"
//在函数events_init_screen_6()中调用music_ui_screen_created(ui);  (重导出后要重加)
void music_ui_screen_created(lv_ui *ui);
// 歌单列表条目是运行时按 music_core 歌单动态生成的：
// 加新歌只需在 music_core.c 的 s_songs 表加一行 { 歌名, URL }，
// 进 screen6 会自动多出对应可点条目，中文歌名用 GB2312 全字集字体显示。

#endif

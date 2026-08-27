#ifndef __MUSIC_UI_H_
#define __MUSIC_UI_H_

#include "gui_guider.h"

//在event_init.c中添加#include "music_ui.h"
//在函数events_init_screen_6()中调用music_ui_screen_created(ui);
void music_ui_screen_created(lv_ui *ui);

#endif

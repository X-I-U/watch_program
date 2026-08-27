#include <string.h>
#include <stdint.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "music_core.h"
#include "music_ui.h"

static lv_obj_t   *s_title_label;   /* screen_6_label_1 */
static lv_obj_t   *s_slider;        /* screen_6_slider_1 */
static lv_obj_t   *s_play_btn;      /* screen_6_img_5 播放钮（按用户要求，img_5 对应播放） */
static lv_obj_t   *s_pause_btn;     /* screen_6_img_7 暂停钮（img_7 对应暂停） */
static lv_timer_t *s_refresh_timer;

/* 上一轮刷新看到的状态，用于"播放中→停止"跳变检测（判断一首自然放完，而不是播放出错） */
static music_state_t s_prev_state = MUSIC_STATE_STOPPED;

/* ---------- 图标状态：播放中 → 显示暂停钮(img_7)，未播放 → 显示播放钮(img_5) ---------- */
static void music_ui_set_playing_icon(bool playing)
{
    if (!s_play_btn || !s_pause_btn) {
        return;
    }
    if (playing) {
        lv_obj_clear_flag(s_pause_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_play_btn, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(s_play_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_pause_btn, LV_OBJ_FLAG_HIDDEN);
    }
}

/* ---------- 事件回调 ---------- */
static void music_play_btn_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    if (music_core_get_state() == MUSIC_STATE_PAUSED) {
        music_core_resume();
    } else {
        music_core_play_index(music_core_get_index());
    }
    music_ui_set_playing_icon(true);
}

static void music_pause_btn_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    music_core_pause();
    music_ui_set_playing_icon(false);
}

static void music_next_btn_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    music_core_next();
    music_ui_set_playing_icon(true);
}

static void music_prev_btn_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    music_core_prev();
    music_ui_set_playing_icon(true);
}

/* 歌单容器点击：cont_1=晴天(0)、cont_2=花海(1) */
static void music_playlist_item_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_SHORT_CLICKED) {
        return;
    }
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    music_core_play_index(idx);
    music_ui_set_playing_icon(true);
}

/* ---------- 定时刷新：歌名 / 进度条 / 自动连播 ---------- */
static void music_refresh_cb(lv_timer_t *timer)
{
    /* screen6 已销毁或已不是当前屏（离开/切换中）→ 删定时器，避免访问悬空控件指针（曾导致 strcmp 崩溃） */
    if (!lv_obj_is_valid(guider_ui.screen_6) || lv_scr_act() != guider_ui.screen_6) {
        lv_timer_del(timer);
        s_refresh_timer = NULL;
        return;
    }
    /* 歌名（直接设置；不用 strcmp 比较，避免悬空 label 指针导致崩溃） */
    const char *title = music_core_get_title();
    if (s_title_label && title) {
        lv_label_set_text(s_title_label, title);
    }
    /* 进度条 */
    if (s_slider) {
        lv_slider_set_value(s_slider, music_core_get_progress_percent(), LV_ANIM_OFF);
    }
    /* 图标始终按实际播放状态显示（播放中→停止钮，否则→播放钮），避免事件回调设置的偏差 */
    music_ui_set_playing_icon(music_core_get_state() == MUSIC_STATE_PLAYING);
    /* 自动连播：状态从"播放中"跳到"停止"（一首自然放完）→ 下一首。
       用跳变判断而非单纯的 STOPPED，避免播放出错（从未进入 PLAYING）时 500ms 空转切歌。 */
    music_state_t st = music_core_get_state();
    if (music_core_is_playing() && st == MUSIC_STATE_STOPPED && s_prev_state == MUSIC_STATE_PLAYING) {
        music_core_next();
    }
    s_prev_state = st;
}

/* 控件/屏幕被删除时删除刷新定时器并清空控件指针，避免悬空访问（LoadProhibited 崩溃）。
   【关键】GUI Guider 的 ui_load_scr_animation 对旧屏会先 lv_obj_clean()（立即释放子控件），
   而 screen_6 自己的 LV_EVENT_DELETE 要等动画结束 auto_del 才触发——刷新定时器若不提前停掉，
   会在 200ms 延时窗口里访问已释放的控件。所以这个回调同时挂在 4 个控件上：任一被删即停定时器。 */
static void music_screen_delete_cb(lv_event_t *e)
{
    if (s_refresh_timer) {
        lv_timer_del(s_refresh_timer);
        s_refresh_timer = NULL;
    }
    s_title_label = NULL;
    s_slider      = NULL;
    s_play_btn    = NULL;
    s_pause_btn   = NULL;
}

void music_ui_screen_created(lv_ui *ui)
{
    s_title_label = ui->screen_6_label_1;
    s_slider      = ui->screen_6_slider_1;
    s_play_btn    = ui->screen_6_img_5;   /* img_5 = 播放 */
    s_pause_btn   = ui->screen_6_img_7;   /* img_7 = 暂停 */

    /* 歌名 label 的 16px 字体是中文字集（只含"晴天"等 62 字符），缺"花/海"；
       改用 20px 完整中文字集，否则切到花海时歌名显示不出来 */
    lv_obj_set_style_text_font(s_title_label, &lv_font_SourceHanSansSC_Regular_20,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 图标与当前播放状态同步（进入页面时） */
    music_ui_set_playing_icon(music_core_get_state() == MUSIC_STATE_PLAYING);
    /* 状态跳变基准：进入页面时同步，避免残留旧值误触发自动连播 */
    s_prev_state = music_core_get_state();

    /* 绑定控制按钮 */
    lv_obj_add_event_cb(s_play_btn,  music_play_btn_cb,  LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_pause_btn, music_pause_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui->screen_6_img_3, music_next_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui->screen_6_img_4, music_prev_btn_cb, LV_EVENT_CLICKED, NULL);

    /* 歌单：cont_1=晴天(0)、cont_2=花海(1)（其余容器暂未接歌） */
    lv_obj_add_event_cb(ui->screen_6_cont_1, music_playlist_item_cb, LV_EVENT_ALL, (void *)(intptr_t)0);
    lv_obj_add_event_cb(ui->screen_6_cont_2, music_playlist_item_cb, LV_EVENT_ALL, (void *)(intptr_t)1);

    /* 控件级删除监听：lv_obj_clean(screen_6) 释放子控件的瞬间就停刷新定时器，
       早于 screen_6 的 LV_EVENT_DELETE（后者要等切屏动画 auto_del 才触发） */
    lv_obj_add_event_cb(s_title_label, music_screen_delete_cb, LV_EVENT_DELETE, NULL);
    lv_obj_add_event_cb(s_slider,      music_screen_delete_cb, LV_EVENT_DELETE, NULL);
    lv_obj_add_event_cb(s_play_btn,    music_screen_delete_cb, LV_EVENT_DELETE, NULL);
    lv_obj_add_event_cb(s_pause_btn,   music_screen_delete_cb, LV_EVENT_DELETE, NULL);
    /* screen 自身销毁时兜底清理定时器（正常 lv_obj_del 路径） */
    lv_obj_add_event_cb(ui->screen_6, music_screen_delete_cb, LV_EVENT_DELETE, NULL);

    /* 刷新定时器（500ms） */
    s_refresh_timer = lv_timer_create(music_refresh_cb, 500, NULL);

    /* 显示当前歌名 */
    if (s_title_label) {
        lv_label_set_text(s_title_label, music_core_get_title());
    }
}

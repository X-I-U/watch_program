#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "xiaozhi.h"
#include "fonts.h"
#include "music_core.h"
#include "xiaozhi_ui.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/idf_additions.h"
#include "esp_heap_caps.h"

/* ============================================================
 * 小智AI UI 绑定层
 * 只碰 LVGL; 状态/对话来自服务层 (xiaozhi.h)
 * 三态: 连接中(spinner+label_2) / 聆听中(img_3+label_5) / 回复中(img_2+label_6)
 * 对话: tile 内纵向滚动容器 + 日志 label
 * ============================================================ */

/* ---------- screen9 控件引用(每次进入重建) ---------- */
static lv_obj_t *s_spinner;       /* screen_9_spinner_1 连接中转圈 */
static lv_obj_t *s_label_conn;    /* screen_9_label_2 "连接中..." */
static lv_obj_t *s_icon_listen;   /* screen_9_img_3 输入图标 */
static lv_obj_t *s_label_listen;  /* screen_9_label_5 "聆听中..." */
static lv_obj_t *s_icon_reply;    /* screen_9_img_2 回答图标 */
static lv_obj_t *s_label_reply;   /* screen_9_label_6 "回复中..." */
static lv_obj_t *s_log_cont;      /* 对话滚动容器 */
static lv_obj_t *s_log_label;     /* 对话日志 label */
static lv_timer_t *s_timer;       /* 刷新定时器 */
static xiaozhi_state_t s_last_state = XIAOZHI_STATE_IDLE;
static uint8_t s_leave_ticks = 0; /* 连续"不在页面"计数(防进页淡入误判离开) */

/* ---------- 对话文本: ws 任务 → 队列 → LVGL 定时器 ---------- */
typedef struct {
    char role[6];
    char text[192];
} xz_msg_t;

static QueueHandle_t s_msg_q = NULL;
static char s_log[2048];          /* 日志缓冲(只被 LVGL 任务写) */
static bool s_music_was_playing = false;   /* 进页前音乐在播, 离开时恢复 */

static void xiaozhi_ui_cleanup(void);

/* ============================================================
 * 音乐互斥: TTS 和音乐都走 I2S0/MAX98357, 进页暂停、离开恢复
 * ============================================================ */
static void pause_music_if_playing(void)
{
    if (music_core_is_playing()) {
        music_core_pause();
        s_music_was_playing = true;
    }
}

static void resume_music_if_was_playing(void)
{
    if (s_music_was_playing) {
        music_core_resume();
        s_music_was_playing = false;
    }
}

/* 三态统一摆到 logo 下方居中(设计稿位置分散, 运行时拉齐, 只显当前状态) */
static void layout_status_center(void)
{
    lv_obj_set_pos(s_spinner,      38, 100);
    lv_obj_set_pos(s_icon_listen,  38, 104);
    lv_obj_set_pos(s_icon_reply,   38, 104);

    lv_obj_set_pos(s_label_conn,   72, 102);
    lv_obj_set_size(s_label_conn,  130, 24);
    lv_obj_set_pos(s_label_listen, 72, 106);
    lv_obj_set_size(s_label_listen, 130, 24);
    lv_obj_set_pos(s_label_reply,  72, 106);
    lv_obj_set_size(s_label_reply, 130, 24);
}

/* ============================================================
 * 日志: 追加一行, 空间不足时丢弃最旧的几行
 * ============================================================ */
static void log_append(const char *role, const char *text)
{
    char line[200];
    int len = snprintf(line, sizeof(line), "%s: %s\n",
                       (role && role[0] == 'u') ? "user" : "小智",
                       text ? text : "");
    if (len < 0) return;
    /* 循环丢旧行直到能放下 */
    while ((int)strlen(s_log) + len + 1 > (int)sizeof(s_log)) {
        char *nl = strchr(s_log, '\n');
        if (!nl) { s_log[0] = '\0'; break; }
        memmove(s_log, nl + 1, strlen(nl + 1) + 1);
    }
    strcat(s_log, line);
}

/* ============================================================
 * 文本回调(websocket 任务上下文, 不能碰 LVGL, 只入队)
 * ============================================================ */
static void xiaozhi_ui_text_cb(const char *role, const char *text)
{
    if (!s_msg_q) return;
    xz_msg_t m;
    memset(&m, 0, sizeof(m));
    snprintf(m.role, sizeof(m.role), "%s", role ? role : "ai");
    snprintf(m.text, sizeof(m.text), "%s", text ? text : "");
    xQueueSend(s_msg_q, &m, 0);   /* 满则丢, 不阻塞 ws 任务 */
}

/* ============================================================
 * 三态显隐
 * ============================================================ */
static void set_visible(lv_obj_t *obj, bool vis)
{
    if (!obj) return;
    if (vis) lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    else     lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static void update_status(xiaozhi_state_t st)
{
    bool show_conn   = (st == XIAOZHI_STATE_CONNECTING);
    bool show_bind   = (st == XIAOZHI_STATE_NEED_BIND);
    bool show_listen = (st == XIAOZHI_STATE_CONNECTED || st == XIAOZHI_STATE_LISTENING);
    bool show_reply  = (st == XIAOZHI_STATE_SPEAKING);

    set_visible(s_spinner,      show_conn);              /* 转圈只在连接中 */
    set_visible(s_label_conn,   show_conn || show_bind);
    set_visible(s_icon_listen,  show_listen);
    set_visible(s_label_listen, show_listen);
    set_visible(s_icon_reply,   show_reply);
    set_visible(s_label_reply,  show_reply);

    if (show_conn) {
        lv_label_set_text(s_label_conn, "连接中...");
    } else if (show_bind) {
        const char *code = xiaozhi_get_activation_code();
        lv_label_set_text_fmt(s_label_conn, "未绑定:%s", code ? code : "??????");
    }
}

/* ============================================================
 * 后台连接任务(阻塞的 xiaozhi_init 不能跑在 LVGL 任务里)
 * ============================================================ */
static void xiaozhi_ui_connect_task(void *arg)
{
    xiaozhi_init();
    vTaskDelete(NULL);
}

static void spawn_connect(void)
{
    /* 栈必须用内部 RAM: get_info 内部会写 NVS(存配置), 写 flash 时禁用 cache,
       PSRAM 栈会触发 esp_task_stack_is_sane_cache_disabled 断言崩溃。
       get_info 之前跑在 3.5KB 的 app_main 栈上, 这里给 8KB 足够(TLS+cJSON)。 */
    xTaskCreatePinnedToCoreWithCaps(xiaozhi_ui_connect_task, "xz_ui_conn", 8 * 1024, NULL,
                                    4, NULL, tskNO_AFFINITY,
                                    MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
}

/* ============================================================
 * 对话日志: tile 内建纵向滚动容器 + 日志 label
 * ============================================================ */
static void create_log(lv_obj_t *tile)
{
    s_log_cont = lv_obj_create(tile);
    lv_obj_set_size(s_log_cont, 230, 126);
    lv_obj_set_pos(s_log_cont, 1, 1);
    lv_obj_set_scroll_dir(s_log_cont, LV_DIR_TOP | LV_DIR_BOTTOM);  /* 仅纵向: 横滑留给返回手势 */
    lv_obj_set_scrollbar_mode(s_log_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_log_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(s_log_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(s_log_cont, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    s_log_label = lv_label_create(s_log_cont);
    lv_label_set_long_mode(s_log_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(s_log_label, 222);
    lv_obj_set_style_text_font(s_log_label, &lv_font_sourcesans_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(s_log_label, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(s_log_label, "");
}

/* ============================================================
 * 刷新定时器: 状态→三态, 文本→日志, 离开→清理
 * ============================================================ */
static void xiaozhi_ui_timer_cb(lv_timer_t *timer)
{
    /* 离开判定: lv_scr_act() 在淡入动画开始前仍是旧屏, 连续 3 次(约600ms)
       不在 screen9 才算真离开, 避免进页淡入时误判。正常离开由
       screen_9 的 LV_EVENT_DELETE 回调负责清理。 */
    if (!lv_obj_is_valid(guider_ui.screen_9) || lv_scr_act() != guider_ui.screen_9) {
        if (++s_leave_ticks >= 3) {
            xiaozhi_ui_cleanup();
        }
        return;
    }
    s_leave_ticks = 0;

    /* 状态 → 三态显隐(只在变化时刷新) */
    xiaozhi_state_t st = xiaozhi_get_state();
    if (st != s_last_state) {
        update_status(st);
        s_last_state = st;
    }

    /* 文本 → 日志 */
    xz_msg_t m;
    bool changed = false;
    while (xQueueReceive(s_msg_q, &m, 0) == pdTRUE) {
        log_append(m.role, m.text);
        changed = true;
    }
    if (changed && s_log_label) {
        lv_label_set_text(s_log_label, s_log);
        lv_obj_update_layout(s_log_cont);
        lv_obj_scroll_to_y(s_log_cont, LV_COORD_MAX, LV_ANIM_OFF);   /* 滚到底: 始终显示最新 */
    }
}

/* ============================================================
 * 清理: 离开页时停定时器 + 关通道停听
 * ============================================================ */
static void xiaozhi_ui_cleanup(void)
{
    if (s_timer) { lv_timer_del(s_timer); s_timer = NULL; }
    xiaozhi_set_active(false);          /* 关音频通道, 停止聆听(保连接, 重进秒回) */
    resume_music_if_was_playing();      /* 进页前在播则恢复音乐 */
    s_spinner = s_label_conn = s_icon_listen = s_label_listen = s_icon_reply = s_label_reply = NULL;
    s_log_cont = s_log_label = NULL;
    s_log[0] = '\0';
}

static void xiaozhi_ui_delete_cb(lv_event_t *e)
{
    xiaozhi_ui_cleanup();
}

/* ============================================================
 * 入口: 进 screen9 时由 events_init_screen_9 调用
 * ============================================================ */
void xiaozhi_ui_screen_created(lv_ui *ui)
{
    /* 1. 抓控件 */
    s_spinner      = ui->screen_9_spinner_1;
    s_label_conn   = ui->screen_9_label_2;
    s_icon_listen  = ui->screen_9_img_3;
    s_label_listen = ui->screen_9_label_5;
    s_icon_reply   = ui->screen_9_img_2;
    s_label_reply  = ui->screen_9_label_6;

    /* 1b. 音乐互斥(进页暂停, 避免 TTS 抢 I2S0) + 三态统一居中 */
    pause_music_if_playing();
    layout_status_center();

    /* 2. 中文 label 统一覆盖 sourcesans_16(对话是任意汉字, GUI Guider 子集不够) */
    lv_obj_set_style_text_font(s_label_conn,   &lv_font_sourcesans_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(s_label_listen, &lv_font_sourcesans_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(s_label_reply,  &lv_font_sourcesans_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 3. 隐藏 GUI Guider 的前缀 label(user:/xiao zhi:), 由日志 label 接管 */
    lv_obj_add_flag(ui->screen_9_label_3, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_9_label_4, LV_OBJ_FLAG_HIDDEN);

    /* 4. 对话日志 */
    create_log(ui->screen_9_tileview_1_tile);

    /* 5. 文本回调 + 队列(清掉上次残留) */
    if (!s_msg_q) {
        s_msg_q = xQueueCreate(12, sizeof(xz_msg_t));
    }
    xQueueReset(s_msg_q);
    xiaozhi_set_text_cb(xiaozhi_ui_text_cb);
    s_log[0] = '\0';

    /* 6. 激活页面(允许 xz_conv 自动开通道) + 未连则后台连接 */
    xiaozhi_set_active(true);
    if (xiaozhi_get_state() == XIAOZHI_STATE_IDLE) {
        spawn_connect();
    }

    /* 7. 刷新定时器(200ms) */
    s_last_state  = XIAOZHI_STATE_IDLE;
    s_leave_ticks = 0;
    s_timer = lv_timer_create(xiaozhi_ui_timer_cb, 200, NULL);

    /* 8. 页面销毁时清理(离开页关通道停听) */
    lv_obj_add_event_cb(ui->screen_9, xiaozhi_ui_delete_cb, LV_EVENT_DELETE, NULL);
}

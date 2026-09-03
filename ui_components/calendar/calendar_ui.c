#include "calendar_ui.h"
#include "lvgl.h"
#include "lv_carousel.h"
#include "calendar_core.h"
#include "rtc_service.h"
#include <stdio.h>

/* 网格布局：carousel 240 宽 × 200 高，7 列（周一~周日，与表头 M T W T F S S 对齐） */
#define CAL_PAGES     3
#define CAL_CELL_W    34
#define CAL_CELL_H    30
#define CAL_GRID_LEFT 1
#define CAL_GRID_TOP  4

LV_FONT_DECLARE(lv_font_montserratMedium_16);

/* 每页 = 一个月的日期网格 */
typedef struct {
    int year;
    int month;
} cal_page_t;

static lv_ui      *s_ui;
static cal_page_t  s_pages[CAL_PAGES];      /* [0]=上月 [1]=当月 [2]=下月 */
static lv_obj_t   *s_elements[CAL_PAGES];   /* carousel 元素（列 0/1/2） */
static int s_today_y, s_today_m, s_today_d;
static int  s_base_y, s_base_m;             /* 窗口焦点月（中间元素 = 当前焦点） */
static bool s_shifting;                     /* 正在 snap 回中间，抑制重入 */

/* 相对今天的比较：<0 过去, 0 今天, >0 未来 */
static int date_cmp(int y1, int m1, int d1, int y2, int m2, int d2)
{
    if (y1 != y2) return (y1 < y2) ? -1 : 1;
    if (m1 != m2) return (m1 < m2) ? -1 : 1;
    if (d1 != d2) return (d1 < d2) ? -1 : 1;
    return 0;
}

/* 在元素里建一个月的日期网格（今天画圈、过去的置灰） */
static void calendar_build_page(lv_obj_t *element, const cal_page_t *p)
{
    calendar_month_t cm;
    calendar_core_calc_month(p->year, p->month, &cm);

    for (int day = 1; day <= cm.days_in_month; day++) {
        int col = calendar_core_cell_col(&cm, day);
        int row = calendar_core_cell_row(&cm, day);
        int x = CAL_GRID_LEFT + col * CAL_CELL_W + (CAL_CELL_W - 30) / 2;
        int y = CAL_GRID_TOP  + row * CAL_CELL_H + (CAL_CELL_H - 30) / 2;

        lv_obj_t *lbl = lv_label_create(element);
        char buf[12];
        snprintf(buf, sizeof(buf), "%d", day);
        lv_label_set_text(lbl, buf);
        lv_obj_set_style_text_font(lbl, &lv_font_montserratMedium_16, 0);
        lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(lbl, lv_color_hex(0x000000), 0);
        lv_obj_set_size(lbl, 30, 30);
        lv_obj_set_pos(lbl, x, y);

        int c = date_cmp(p->year, p->month, day, s_today_y, s_today_m, s_today_d);
        if (c == 0) {
            /* 今天：圆形高亮 + 白字。整体上移 3px + pad_top 2 让数字在圆里居中并与左右日期对齐。
               坐标直接算，不要用 lv_obj_get_x/get_y 重读（布局未刷新时会读到错值，控件会跑到左上角） */
            lv_obj_set_style_bg_color(lbl, lv_color_hex(0x2195f6), 0);
            lv_obj_set_style_bg_opa(lbl, 255, 0);
            lv_obj_set_style_radius(lbl, LV_RADIUS_CIRCLE, 0);
            lv_obj_set_style_text_color(lbl, lv_color_hex(0xffffff), 0);
            lv_obj_set_style_pad_top(lbl, 2, 0);
            lv_obj_set_pos(lbl, x, y - 3);
        } else if (c < 0) {
            /* 过去的日期：置灰（表盘亮度高，比 LV_OPA_40 深一点，避免太浅看不清） */
            lv_obj_set_style_text_opa(lbl, LV_OPA_60, 0);
        }
    }
}

/* 翻到第几页 → 更新右上角年份/月份标签 */
static void calendar_update_title(int page_idx)
{
    if (!s_ui) return;
    const cal_page_t *p = &s_pages[page_idx];
    char buf[16];
    snprintf(buf, sizeof(buf), "%d年", p->year);
    lv_label_set_text(s_ui->screen_4_label_40, buf);
    lv_label_set_text(s_ui->screen_4_label_39, calendar_core_month_name_cn(p->month));
}

/* 根据焦点月(s_base)填 s_pages[] = [base-1, base, base+1] */
static void calendar_set_window(void)
{
    int by = s_base_y, bm = s_base_m;
    int py = by, pm = bm - 1; if (pm < 1) { pm = 12; py--; }
    int ny = by, nm = bm + 1; if (nm > 12) { nm = 1; ny++; }
    s_pages[0] = (cal_page_t){ py, pm };
    s_pages[1] = (cal_page_t){ by, bm };
    s_pages[2] = (cal_page_t){ ny, nm };
}

/* 重建全部 3 个元素的日期网格（先清掉旧 label） */
static void calendar_build_all(void)
{
    for (int i = 0; i < CAL_PAGES; i++) {
        lv_obj_clean(s_elements[i]);
        calendar_build_page(s_elements[i], &s_pages[i]);
    }
}

/* 窗口平移一格后回中间元素。dir=+1 往后翻(下月)，-1 往前翻(上月)。
   关键：停到边缘页时用户正盯着的那列(列0/列2)就是新焦点月；回中前先把中间列刷成同一个月，
   再【无动画瞬时】跳回中间 —— 因中间与当前显示内容相同，跳转画面不变、不会从反方向倒滑进来。
   跳完后再刷两侧元素(此时已在屏幕外)。这样翻页方向永远与滑动方向一致，可无限翻。 */
static void calendar_shift_window(int dir)
{
    int by = s_base_y, bm = s_base_m;
    if (dir > 0) { bm++; if (bm > 12) { bm = 1; by++; } }
    else         { bm--; if (bm < 1)  { bm = 12; by--; } }
    s_base_y = by; s_base_m = bm;

    calendar_set_window();   /* s_pages = [焦点-1, 焦点, 焦点+1] */

    /* ① 先只刷中间列(新焦点月)。此刻用户看到的边缘列内容也是这个月；
         中间列在屏幕外(偏左/偏右)，刷它不可见，也不会改变当前显示。 */
    lv_obj_clean(s_elements[1]);
    calendar_build_page(s_elements[1], &s_pages[1]);

    /* ② 瞬时跳回中间列(scroll_x=240)：中间==刚看到的月，画面无变化。
        LV_ANIM_OFF 是关键 —— 不再往回滚动画，避免"下月从反方向滑进来"。 */
    s_shifting = true;
    lv_obj_set_scroll_dir(s_ui->screen_4_carousel_1, LV_DIR_HOR);
    lv_obj_update_layout(s_ui->screen_4_carousel_1);
    lv_obj_scroll_to(s_ui->screen_4_carousel_1, 240, 0, LV_ANIM_OFF);
    s_shifting = false;

    /* ③ 已回到中间，左右两列在屏幕外，此时再刷它们(为下一次翻页备用)，不会闪。 */
    lv_obj_clean(s_elements[0]);
    calendar_build_page(s_elements[0], &s_pages[0]);
    lv_obj_clean(s_elements[2]);
    calendar_build_page(s_elements[2], &s_pages[2]);

    calendar_update_title(1);
}

/* 延迟执行窗口平移：必须等外层 carousel_event_cb 完全结束（它会在我们回调后覆盖 scroll_dir），
   这里再重置 scroll_dir=HOR 才不会被覆盖，否则翻页后 carousel 被锁成单方向，只能在前两个月间来回。 */
static void calendar_shift_delayed(lv_timer_t *timer)
{
    int dir = (int)(intptr_t)timer->user_data;
    lv_timer_del(timer);   /* one-shot */

    if (!s_ui || !lv_obj_is_valid(s_ui->screen_4_carousel_1)) return;   /* 已离开日历页 */

    calendar_shift_window(dir);
    lv_obj_set_scroll_dir(s_ui->screen_4_carousel_1, LV_DIR_HOR);
}

/* 翻页完成：更新标题；若滑到左右边缘元素，延迟到下一帧做窗口平移（避免与外层 scroll_dir 冲突） */
static void carousel_page_cb(lv_event_t *e)
{
    if (s_shifting) return;   /* snap 回中间触发的 VALUE_CHANGED，忽略 */

    lv_obj_t *carousel = lv_event_get_target(e);
    lv_obj_t *act = lv_carousel_get_element_act(carousel);
    int idx = -1;
    for (int i = 0; i < CAL_PAGES; i++) {
        if (act == s_elements[i]) { idx = i; break; }
    }
    if (idx < 0) return;

    calendar_update_title(idx);
    if (idx == 2) {
        lv_timer_create(calendar_shift_delayed, 30, (void *)(intptr_t)1);   /* 到右边缘(下月)：窗口后移 */
    } else if (idx == 0) {
        lv_timer_create(calendar_shift_delayed, 30, (void *)(intptr_t)(-1)); /* 到左边缘(上月)：窗口前移 */
    }
}

void calendar_ui_screen_created(lv_ui *ui)
{
    s_ui = ui;

    /* 今天（从 RTC 读；读不到就兜底） */
    pcf8563_time_t now = {0};
    if (rtc_service_get_time(&now) == ESP_OK) {
        s_today_y = now.year; s_today_m = now.month; s_today_d = now.day;
    } else {
        s_today_y = 2026; s_today_m = 8; s_today_d = 26;
    }

    /* 窗口焦点 = 当月 */
    s_base_y = s_today_y; s_base_m = s_today_m;
    calendar_set_window();

    /* 元素：GUI Guider 已建 element_1(列0)/element_2(列1)，补第 3 个(列2)。
       布局 [上月, 当月, 下月]：手指左滑→右(下月)，右滑→左(上月)。 */
    s_elements[0] = ui->screen_4_carousel_1_element_1;
    s_elements[1] = ui->screen_4_carousel_1_element_2;
    s_elements[2] = lv_carousel_add_element(ui->screen_4_carousel_1, 2);

    /* 第 3 个元素去掉默认边框/背景，与前两个视觉一致 */
    lv_obj_set_style_bg_opa(s_elements[2], 0, 0);
    lv_obj_set_style_border_width(s_elements[2], 0, 0);
    lv_obj_set_style_outline_width(s_elements[2], 0, 0);
    lv_obj_set_style_shadow_width(s_elements[2], 0, 0);

    /* 每个元素建日期网格 */
    calendar_build_all();

    /* 翻页回调 + 初始标题 + 初始停到当月（列 1） */
    lv_obj_add_event_cb(ui->screen_4_carousel_1, carousel_page_cb, LV_EVENT_VALUE_CHANGED, NULL);
    calendar_update_title(1);
    s_shifting = true;
    lv_obj_update_layout(ui->screen_4_carousel_1);
    lv_obj_set_element_id(ui->screen_4_carousel_1, 1, LV_ANIM_OFF);
    s_shifting = false;
}

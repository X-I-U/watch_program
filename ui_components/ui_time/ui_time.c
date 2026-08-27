#include "ui_time.h"
#include "ui.h"
#include "gui_guider.h"
#include "lvgl.h"

static const char *weekday_cn(int wd)
{
    static const char *names[7] = {
        "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"
    };
    return (wd >= 0 && wd <= 6) ? names[wd] : "";
}

void ui_time_refresh(const pcf8563_time_t *t)
{
    if (!t) return;

    /* 时间：24 小时制（GUI Guider 普通 Label：screen_2_cont_8 里的 screen_2_label_2） */
    if (lv_obj_is_valid(guider_ui.screen_2_label_2)) {
        lv_label_set_text_fmt(guider_ui.screen_2_label_2, "%02d:%02d:%02d",
                              t->hour, t->min, t->sec);
    }
    /* 日期 */
    if (lv_obj_is_valid(guider_ui.screen_2_datetext_1)) {
        lv_label_set_text_fmt(guider_ui.screen_2_datetext_1, "%d/%02d/%02d",
                              t->year, t->month, t->day);
    }
    /* 星期 */
    if (lv_obj_is_valid(guider_ui.screen_2_label_1)) {
        lv_label_set_text(guider_ui.screen_2_label_1, weekday_cn(t->weekday));
    }
}

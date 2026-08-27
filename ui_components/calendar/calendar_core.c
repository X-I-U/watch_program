#include "calendar_core.h"
#include <time.h>

static bool is_leap_year(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

void calendar_core_calc_month(int year, int month, calendar_month_t *out)
{
    if (!out || month < 1 || month > 12) return;

    static const int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int d = days[month - 1];
    if (month == 2 && is_leap_year(year)) d = 29;

    /* 当月 1 号是星期几：用 libc mktime 归一化（0=周日），再转成 0=周一 */
    struct tm t = {0};
    t.tm_year = year - 1900;
    t.tm_mon  = month - 1;
    t.tm_mday = 1;
    t.tm_hour = 12;
    mktime(&t);
    int w = (t.tm_wday + 6) % 7;   /* 周一→0, 周日→6 */

    out->year = year;
    out->month = month;
    out->days_in_month = d;
    out->first_wday = w;
}

int calendar_core_cell_col(const calendar_month_t *m, int day)
{
    return (m->first_wday + (day - 1)) % 7;
}

int calendar_core_cell_row(const calendar_month_t *m, int day)
{
    return (m->first_wday + (day - 1)) / 7;
}

const char *calendar_core_month_name_cn(int month)
{
    static const char *names[12] = {
        "一月", "二月", "三月", "四月", "五月", "六月",
        "七月", "八月", "九月", "十月", "十一月", "十二月"
    };
    return (month >= 1 && month <= 12) ? names[month - 1] : "";
}

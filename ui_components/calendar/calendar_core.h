#ifndef __CALENDAR_CORE_H_
#define __CALENDAR_CORE_H_

#include <stdbool.h>

/* 日历业务核心（纯 C，无 LVGL）。
 * 只负责"某年某月怎么排"的日期计算，UI 层拿这些数据去画。
 * 星期约定：0=周一 ... 6=周日（与 screen4 表头 M T W T F S S 一致）。
 */

typedef struct {
    int year;           /* 4 位年份，如 2026 */
    int month;          /* 1-12 */
    int days_in_month;  /* 28-31 */
    int first_wday;     /* 当月 1 号是星期几，0=周一 */
} calendar_month_t;

/* 计算某年某月的基本信息 */
void calendar_core_calc_month(int year, int month, calendar_month_t *out);

/* 某月某日在 7 列网格中的列(0-6)和行(0-5)。day 取值 1..days_in_month */
int calendar_core_cell_col(const calendar_month_t *m, int day);
int calendar_core_cell_row(const calendar_month_t *m, int day);

/* 月份中文名："一月"~"十二月"（返回静态字符串） */
const char *calendar_core_month_name_cn(int month);

#endif /* __CALENDAR_CORE_H_ */

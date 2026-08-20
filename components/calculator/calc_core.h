/**
 * calc_core.h —— 计算器核心逻辑（纯 C，零 LVGL 依赖）
 *
 * 本模块只做"输入按键 -> 更新内部状态 -> 给出要显示的内容"，
 * 不认识 lv_obj_t、不认识任何驱动，可单独在 PC 上编译测试。
 */
#ifndef CALC_CORE_H
#define CALC_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/* 退格键（GUI 里那个删除图片键） */
#define CALC_KEY_BACKSPACE 0x08

/* 显示缓冲区大小（含 '\0'） */
#define CALC_DISPLAY_MAX 24

typedef struct {
    char   display[CALC_DISPLAY_MAX];  /* 当前应显示的内容，calc_ui 直接读它刷新 */
    char   expr[CALC_DISPLAY_MAX];     /* 正在输入的表达式（如 "1+6"），按 = 后清空，显示结果 */
    char   buffer[CALC_DISPLAY_MAX];   /* 正在输入的数字串 */
    int    len;                        /* buffer 当前长度 */
    double acc;                        /* 累积值（上一个运算符之前的结果） */
    char   op;                         /* 待执行运算符：'+' '-' '*' '/' '%'，'\0' 表示无 */
    bool   fresh;                      /* 刚按过运算符/等号，下一个数字应开启新输入 */
    bool   error;                      /* 出错（除零/溢出）：除 'C' 外忽略所有键 */
} calc_t;

/** 复位到 "0" */
void calc_init(calc_t *c);

/**
 * 输入一个按键。
 * key: '0'-'9' | '.' | '+' '-' '*' '/' '%' | '=' | 'C' | CALC_KEY_BACKSPACE
 * 每次调用后 c->display 已更新为新的显示内容。
 */
void calc_input(calc_t *c, char key);

#ifdef __cplusplus
}
#endif

#endif /* CALC_CORE_H */

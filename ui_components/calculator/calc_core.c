/**
 * calc_core.c —— 计算器核心实现
 *
 * 运算用"立即执行"模型（同普通桌面计算器）：按运算符的瞬间就把
 *   累积值 OP 当前输入 算掉，结果作为新的累积值。
 * 例：5 + 3 * 2 =  ->  5+3=8，8*2=16
 *
 * 显示：输入时把按键原样串成表达式（expr）显示，运算符保留，
 *   如输入 "1+6" 显示 "1+6"；按 = 后算出结果替换显示（"7"），
 *   同时清空 expr，之后的输入从结果继续。
 */
#include "calc_core.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CALC_ERR_STR   "Err"
#define CALC_INPUT_MAX (CALC_DISPLAY_MAX - 2)   /* 输入上限（给 '\0' 留位） */

static void set_display(calc_t *c, const char *s)
{
    strncpy(c->display, s, CALC_DISPLAY_MAX - 1);
    c->display[CALC_DISPLAY_MAX - 1] = '\0';
}

/* ---- 表达式显示：输入时把按键原样串成公式（如 "1+6"），按 = 后清空改显结果 ---- */

static void expr_clear(calc_t *c)
{
    c->expr[0] = '\0';
}

static void expr_append(calc_t *c, char ch)
{
    size_t n = strlen(c->expr);
    if (n + 1 >= CALC_DISPLAY_MAX) return;   /* 满了就丢弃（显示长度上限） */
    c->expr[n] = ch;
    c->expr[n + 1] = '\0';
}

/* 追加一个运算符：末尾已是运算符（连续按运算符）则替换；
 * 表达式为空（刚按过 =，显示的是结果）则以当前结果为起点接运算符，如 "8" -> "8+" */
static void expr_push_operator(calc_t *c, char op)
{
    size_t n = strlen(c->expr);
    if (n == 0) {
        strncpy(c->expr, c->display, CALC_DISPLAY_MAX - 1);
        c->expr[CALC_DISPLAY_MAX - 1] = '\0';
        expr_append(c, op);
        return;
    }
    char last = c->expr[n - 1];
    if (last == '+' || last == '-' || last == '*' || last == '/' || last == '%')
        c->expr[n - 1] = op;
    else
        expr_append(c, op);
}

/* 进入错误状态：除 'C' 外忽略所有键 */
static void set_error(calc_t *c)
{
    c->error = true;
    expr_clear(c);
    set_display(c, CALC_ERR_STR);
}

/* 数值 -> 显示：最多 10 位有效数字，整数不带小数点（%g 自动去尾零） */
static void show_number(calc_t *c, double v)
{
    char tmp[40];

    if (isnan(v) || isinf(v)) { set_error(c); return; }

    snprintf(tmp, sizeof(tmp), "%.10g", v);
    if (strlen(tmp) >= CALC_DISPLAY_MAX) { set_error(c); return; }   /* 科学计数法过长 */
    set_display(c, tmp);
}

static double buf_value(calc_t *c)
{
    return (c->len > 0) ? atof(c->buffer) : 0.0;
}

/* 当前操作数：fresh（刚按过运算符/等号）时取累积值，否则取正在输入的数 */
static double current_value(calc_t *c)
{
    if (c->fresh) return c->acc;
    return (c->len > 0) ? buf_value(c) : c->acc;
}

static double apply_op(char op, double a, double b, bool *err)
{
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/':
        if (b == 0.0) { *err = true; return 0.0; }
        return a / b;
    case '%':
        if (b == 0.0) { *err = true; return 0.0; }
        return fmod(a, b);
    default:  return b;
    }
}

void calc_init(calc_t *c)
{
    memset(c, 0, sizeof(*c));
    c->fresh = true;                /* 初始状态按 '.' 应从 "0." 开始 */
    expr_clear(c);
    set_display(c, "0");
}

void calc_input(calc_t *c, char key)
{
    if (c->error) {
        if (key == 'C') calc_init(c);           /* 出错后除 C 外全部忽略 */
        return;
    }

    /* ---- 数字 ---- */
    if (key >= '0' && key <= '9') {
        bool was_fresh = c->fresh;
        if (c->fresh) {                         /* 结果/运算符之后，开新输入 */
            c->buffer[0] = '\0';
            c->len = 0;
            c->fresh = false;
        }
        if (c->len >= CALC_INPUT_MAX) return;   /* 输入长度上限 */
        c->buffer[c->len++] = key;
        c->buffer[c->len] = '\0';
        if (was_fresh && c->op == '\0')
            { expr_clear(c); expr_append(c, key); }  /* 刚按过 = ：开始新表达式 */
        else
            expr_append(c, key);                     /* 续上当前表达式 */
        set_display(c, c->expr);
        return;
    }

    /* ---- 小数点 ---- */
    if (key == '.') {
        bool was_fresh = c->fresh;
        if (c->fresh) {
            strcpy(c->buffer, "0");
            c->len = 1;
            c->fresh = false;
        }
        if (!strchr(c->buffer, '.') && c->len < CALC_INPUT_MAX) {
            c->buffer[c->len++] = key;
            c->buffer[c->len] = '\0';
            if (was_fresh && c->op == '\0') {
                strcpy(c->expr, "0.");
            } else if (was_fresh) {
                expr_append(c, '0');   /* 运算符之后：5+ -> 5+0. */
                expr_append(c, '.');
            } else {
                expr_append(c, '.');
            }
        }
        set_display(c, c->expr);
        return;
    }

    /* ---- 退格 ---- */
    if (key == CALC_KEY_BACKSPACE) {
        if (c->fresh) { calc_init(c); return; } /* 结果/运算符后按退格 = 清屏 */
        if (c->len > 0) {
            c->buffer[--c->len] = '\0';
            if (c->len == 0) { c->buffer[0] = '0'; c->buffer[1] = '\0'; c->len = 1; }
        }
        /* 表达式同步删尾；整串删空时回落到 buffer（"0"） */
        size_t n = strlen(c->expr);
        if (n > 0) c->expr[n - 1] = '\0';
        if (c->expr[0] == '\0') set_display(c, c->buffer);
        else                    set_display(c, c->expr);
        return;
    }

    /* ---- 清零 ---- */
    if (key == 'C') { calc_init(c); return; }

    /* ---- 运算符 + - * / % ---- */
    if (key == '+' || key == '-' || key == '*' || key == '/' || key == '%') {
        double val = current_value(c);
        bool   err = false;

        if (c->op != '\0') {
            if (!c->fresh) c->acc = apply_op(c->op, c->acc, val, &err);
            /* fresh 表示连续按运算符：只换运算符，不重复计算 */
        } else {
            c->acc = val;                       /* 第一个运算符：把当前输入存进累积值 */
        }
        if (err) { set_error(c); return; }

        c->op = key;
        c->fresh = true;
        expr_push_operator(c, key);             /* 运算符保留在表达式里：1+6 -> 1+6 */
        set_display(c, c->expr);
        return;
    }

    /* ---- 等号 ---- */
    if (key == '=') {
        if (c->op != '\0') {
            double val = current_value(c);
            bool   err = false;
            c->acc = apply_op(c->op, c->acc, val, &err);
            if (err) { set_error(c); return; }
            c->op = '\0';
        } else {
            c->acc = current_value(c);          /* 无待执行运算符：当前输入即结果 */
        }
        c->fresh = true;
        expr_clear(c);                          /* 结果替换表达式显示 */
        show_number(c, c->acc);
        return;
    }
}

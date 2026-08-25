/**
 * game2048_core.c —— 2048 核心实现
 *
 * 方向滑动统一转成"向左滑动"处理：
 *   左 = 逐行 slide_left；右 = 行反转后 slide_left；
 *   上 = 逐列（抽成行）slide_left；下 = 列反转后 slide_left。
 */
#include "game2048_core.h"

#include <stdlib.h>
#include <string.h>

/* ---- 单行/单列向左滑动合并 ----
 * line 为 4 元素数组，原地改写。每格每步最多合并一次（used 标记）。
 * 合并：out[w-1] == v 且未被用过  ->  指数 +1，分数加上合并后的值 1<<exp。
 */
static bool slide_left(uint8_t *line, uint32_t *score)
{
    uint8_t out[4] = {0, 0, 0, 0};
    bool    used[4] = {false, false, false, false};
    int     w = 0;
    bool    changed = false;

    for (int r = 0; r < 4; r++) {
        uint8_t v = line[r];
        if (v == 0) continue;

        if (w > 0 && out[w - 1] == v && !used[w - 1] && v < 30) {
            out[w - 1] = (uint8_t)(v + 1);      /* 合并 = 指数 +1 */
            used[w - 1] = true;                 /* 禁止再合并 */
            *score += (uint32_t)1 << out[w - 1];
            changed = true;
        }
        else {
            out[w++] = v;
        }
    }

    for (int r = 0; r < 4; r++) {
        if (out[r] != line[r]) changed = true;
        line[r] = out[r];
    }
    return changed;
}

/* 每一行都做一次 slide_left；reverse 为 true 时先把行反转（= 向右滑） */
static bool slide_rows(g2048_t *g, bool reverse)
{
    bool changed = false;

    for (int r = 0; r < 4; r++) {
        uint8_t line[4];
        for (int c = 0; c < 4; c++)
            line[c] = g->grid[r][reverse ? (3 - c) : c];
        if (slide_left(line, &g->score)) changed = true;
        for (int c = 0; c < 4; c++)
            g->grid[r][reverse ? (3 - c) : c] = line[c];
    }
    return changed;
}

/* 每一列都做一次 slide_left（把列抽成行再处理）；reverse 为 true = 向下滑 */
static bool slide_cols(g2048_t *g, bool reverse)
{
    bool changed = false;

    for (int c = 0; c < 4; c++) {
        uint8_t line[4];
        for (int r = 0; r < 4; r++)
            line[r] = g->grid[reverse ? (3 - r) : r][c];
        if (slide_left(line, &g->score)) changed = true;
        for (int r = 0; r < 4; r++)
            g->grid[reverse ? (3 - r) : r][c] = line[r];
    }
    return changed;
}

/* 合成出 2048（指数 >= 11）即标记获胜 */
static void check_won(g2048_t *g)
{
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            if (g->grid[r][c] >= G2048_WIN_EXP) { g->won = true; return; }
}

void g2048_spawn(g2048_t *g)
{
    int empty[16];
    int n = 0;

    for (int i = 0; i < 16; i++)
        if (g->grid[i / 4][i % 4] == 0) empty[n++] = i;
    if (n == 0) return;

    int idx = empty[rand() % n];
    g->grid[idx / 4][idx % 4] = (rand() % 10 == 0) ? 2 : 1;   /* 10% 出 4，90% 出 2 */
}

bool g2048_can_move(const g2048_t *g)
{
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (g->grid[r][c] == 0) return true;
            if (c < 3 && g->grid[r][c] == g->grid[r][c + 1]) return true;
            if (r < 3 && g->grid[r][c] == g->grid[r + 1][c]) return true;
        }
    }
    return false;
}

bool g2048_move(g2048_t *g, g2048_dir_t dir)
{
    bool changed;

    switch (dir) {
    case G2048_DIR_LEFT:  changed = slide_rows(g, false); break;
    case G2048_DIR_RIGHT: changed = slide_rows(g, true);  break;
    case G2048_DIR_UP:    changed = slide_cols(g, false); break;
    case G2048_DIR_DOWN:  changed = slide_cols(g, true);  break;
    default:              return false;
    }

    if (changed) {
        check_won(g);
        g2048_spawn(g);
        g->over = !g2048_can_move(g);
    }
    return changed;
}

void g2048_init(g2048_t *g)
{
    memset(g->grid, 0, sizeof(g->grid));
    g->score = 0;
    g->won   = false;
    g->over  = false;

    g2048_spawn(g);
    g2048_spawn(g);
}

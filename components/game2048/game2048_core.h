/**
 * game2048_core.h —— 2048 核心逻辑（纯 C，零 LVGL / 驱动依赖）
 *
 * 本模块只做"方向滑动 -> 更新棋盘状态 -> 返回是否变化"，
 * 不认识 lv_obj_t、不认识任何驱动，可单独在 PC 上编译测试：
 *     gcc game2048_core.c test.c
 *
 * 棋盘用"指数"存储：grid[r][c] = 0 表示空格，n 表示数值 2^n。
 * 合并相邻同值格 = 指数 +1（如两个 2 → 一个 4：指数 1 → 2），
 * 显示值就是 1 << n。这样核心逻辑极小，也天然避免大数运算。
 */
#ifndef GAME2048_CORE_H
#define GAME2048_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define G2048_WIN_EXP 11        /* 2^11 = 2048，合成即获胜（之后可继续玩） */

typedef enum {
    G2048_DIR_UP = 0,
    G2048_DIR_DOWN,
    G2048_DIR_LEFT,
    G2048_DIR_RIGHT,
} g2048_dir_t;

typedef struct {
    uint8_t  grid[4][4];        /* 指数：0=空，n=2^n */
    uint32_t score;
    bool     won;               /* 是否已合成过 2048 */
    bool     over;              /* 是否已无法移动（游戏结束） */
} g2048_t;

/** 新开一局：清空棋盘，随机生成两个初始块 */
void g2048_init(g2048_t *g);

/**
 * 完整走一步：按 dir 滑动合并；若棋盘发生变化则随机生成一个新块，
 * 并更新 won / over 状态。返回棋盘是否变化（UI 层据此决定是否刷新）。
 * 随机数来自 rand()，调用方需自行 srand 播种。
 */
bool g2048_move(g2048_t *g, g2048_dir_t dir);

/** 在随机空格子里生成一个新块：90% 为 2（指数1），10% 为 4（指数2） */
void g2048_spawn(g2048_t *g);

/** 是否还能移动（存在空格，或存在可合并的相邻同值块） */
bool g2048_can_move(const g2048_t *g);

/* ---- 只读访问 ---- */

static inline int g2048_cell(const g2048_t *g, int row, int col)
{
    return g->grid[row][col];   /* 返回指数，0 = 空 */
}

static inline uint32_t g2048_score(const g2048_t *g) { return g->score; }
static inline bool g2048_won(const g2048_t *g)       { return g->won; }
static inline bool g2048_over(const g2048_t *g)      { return g->over; }

#ifdef __cplusplus
}
#endif

#endif /* GAME2048_CORE_H */

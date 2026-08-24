/**
 * game2048_ui.c —— 2048 UI 适配层
 *
 * 格子识别：不硬编码 screen_3_cont_N 序号，遍历 screen_3_cont_1 的子对象，
 * 按 (x,y) 位置反推 4x4 行列（间距 51px）。这样 GUI Guider 重导出改命名/顺序也不容易错。
 *
 * 数字块：每个格子一个 label，位置 = 格子位置 + 2，尺寸 43x43
 * （与 GUI Guider 里样例 label 的排法一致），背景色按数值区分，
 * 文字颜色/字号随数值自适应。
 *
 * 手势：LVGL 的 LV_EVENT_GESTURE 从按下对象沿父链上冒，需要沿途对象带
 * LV_OBJ_FLAG_GESTURE_BUBBLE，处理器挂在 screen_3 上。
 *
 * 线程安全：本文件的静态状态只在 LVGL 任务上下文（事件回调）里访问，天然单线程，
 * 不需要额外加锁（同 calc_ui）。
 */
#include "game2048_ui.h"

#include "lvgl.h"
#include "game2048_core.h"

/* GUI Guider 生成的头里只声明了 Acme 系列，montserratMedium_16 需要自己声明 */
LV_FONT_DECLARE(lv_font_montserratMedium_16);

/* ---- 4x4 格子布局参数（与 GUI Guider 里的排布一致）---- */
#define CELL_PITCH  51          /* 相邻格子中心间距 */
#define CELL_X0     5           /* 第一列 x */
#define CELL_Y0     7           /* 第一行 y */
#define TILE_OFF    2           /* 数字块相对格子的偏移 */
#define TILE_SIZE   43          /* 数字块尺寸 = 47 - 2*2 */

static g2048_t    s_game;
static lv_obj_t  *s_tiles[4][4];    /* 16 个数字块 label */
static lv_obj_t  *s_score_label;    /* 分数 label */
static lv_obj_t  *s_overlay;        /* 游戏结束弹窗遮罩（默认隐藏） */

/* 2048 经典配色。存十六进制原值、运行时再转 lv_color_t：
   因为这个移植版里 lv_color_hex() 展开成复合字面量，不能用作静态数组的
   常量初始化元素。下标 = 指数（0 空 / 1=2 / 2=4 / 3=8 ...） */
static const uint32_t s_tile_colors[] = {
    0xffffff, /* 0   空（与格子底色一致） */
    0xeee4da, /* 2  */
    0xede0c8, /* 4  */
    0xf2b179, /* 8  */
    0xf59563, /* 16 */
    0xf67c5f, /* 32 */
    0xf65e3b, /* 64 */
    0xedcf72, /* 128 */
    0xedcc61, /* 256 */
    0xedc850, /* 512 */
    0xedc53f, /* 1024 */
    0xedc22e, /* 2048 */
    0x3c3a32, /* 4096+ */
};
#define TILE_COLOR_MAX ((int)(sizeof(s_tile_colors) / sizeof(s_tile_colors[0])) - 1)

static lv_color_t tile_color(int exp)
{
    if (exp < 0)    exp = 0;
    if (exp > TILE_COLOR_MAX) exp = TILE_COLOR_MAX;
    return lv_color_hex(s_tile_colors[exp]);
}

/* 文字颜色：2/4 用深色，8 起用白色，保证对比度 */
static lv_color_t tile_text_color(int exp)
{
    return (exp <= 2) ? lv_color_hex(0x776e65) : lv_color_hex(0xf9f6f2);
}

/* 按位数选字号：位数越多字越小，保证不超出 43px 的块 */
static const lv_font_t *tile_font(int exp)
{
    int digits = 1;
    for (int v = 1 << exp; v >= 10; v /= 10) digits++;

    if (digits <= 1) return &lv_font_Acme_Regular_40;
    if (digits == 2) return &lv_font_Acme_Regular_35;
    if (digits == 3) return &lv_font_Acme_Regular_25;
    return &lv_font_montserratMedium_16;
}

/* ---- 建立 16 个数字块，并识别格子映射 ---- */
static void build_tiles(lv_ui *ui)
{
    lv_obj_t *board = ui->screen_3_cont_1;

    /* 删掉 GUI Guider 摆放的样例数字 label（"2"/"4"/"2" 占位），
       运行时重新建 16 个，删除时注意索引前移 */
    uint32_t i = 0;
    while (i < lv_obj_get_child_cnt(board)) {
        lv_obj_t *child = lv_obj_get_child(board, i);
        if (lv_obj_check_type(child, &lv_label_class)) lv_obj_del(child);
        else                                          i++;
    }

    /* 剩下的子对象就是 16 个格子容器；先固定数量再建 label，避免把
       新建的 label 也当成格子处理 */
    uint32_t cell_cnt = lv_obj_get_child_cnt(board);
    for (uint32_t k = 0; k < cell_cnt; k++) {
        lv_obj_t *cell = lv_obj_get_child(board, k);
        int x = lv_obj_get_x(cell);
        int y = lv_obj_get_y(cell);
        int col = (x - CELL_X0 + CELL_PITCH / 2) / CELL_PITCH;
        int row = (y - CELL_Y0 + CELL_PITCH / 2) / CELL_PITCH;
        if (col < 0) col = 0;
        if (col > 3) col = 3;
        if (row < 0) row = 0;
        if (row > 3) row = 3;

        lv_obj_add_flag(cell, LV_OBJ_FLAG_GESTURE_BUBBLE);

        lv_obj_t *tile = lv_label_create(board);
        lv_obj_set_pos(tile, x + TILE_OFF, y + TILE_OFF);
        lv_obj_set_size(tile, TILE_SIZE, TILE_SIZE);
        lv_obj_set_style_text_align(tile, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(tile, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(tile, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(tile, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_long_mode(tile, LV_LABEL_LONG_CLIP);

        s_tiles[row][col] = tile;
    }

    /* 让手势能从格子容器冒泡到 screen_3 */
    lv_obj_add_flag(board, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

/* ---- 分数 label：填满分数框，分数变大后按框宽截断，不换行错位 ---- */
static void setup_score(lv_ui *ui)
{
    lv_obj_t *score = ui->screen_3_label_4;
    lv_obj_t *box   = ui->screen_3_cont_18;

    lv_obj_set_pos(score, lv_obj_get_x(box) + 6, lv_obj_get_y(box) + 8);
    lv_obj_set_size(score, lv_obj_get_width(box) - 12, lv_obj_get_height(box) - 16);
    lv_label_set_long_mode(score, LV_LABEL_LONG_CLIP);

    s_score_label = score;
}

/* ---- 把 core 棋盘状态刷到 16 个数字块 + 分数 ---- */
static void refresh_all(void)
{
    char buf[16];

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            lv_obj_t *tile = s_tiles[r][c];
            if (tile == NULL) continue;

            int exp = g2048_cell(&s_game, r, c);
            lv_obj_set_style_bg_opa(tile, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(tile, tile_color(exp), LV_PART_MAIN | LV_STATE_DEFAULT);

            if (exp == 0) {
                lv_label_set_text(tile, "");
            }
            else {
                lv_snprintf(buf, sizeof(buf), "%d", 1 << exp);
                lv_label_set_text(tile, buf);
                lv_obj_set_style_text_color(tile, tile_text_color(exp), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_font(tile, tile_font(exp), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }
    }

    if (s_score_label != NULL) {
        lv_snprintf(buf, sizeof(buf), "score:%u", (unsigned)s_game.score);
        lv_label_set_text(s_score_label, buf);
    }
}

/* ---- 游戏结束弹窗：遮罩 + "GAME OVER" + Try Again 按钮 ----
 * 全部运行时创建（不依赖 GUI Guider），默认隐藏，游戏结束才显示。
 * 遮罩铺满全屏且没有 GESTURE_BUBBLE，弹窗期间会挡住下面的棋盘手势。 */
static void try_again_cb(lv_event_t *e)
{
    lv_obj_add_flag(s_overlay, LV_OBJ_FLAG_HIDDEN);   /* 藏弹窗 */
    g2048_init(&s_game);                              /* 新开一局（分数归零、生成两块） */
    refresh_all();
}

static void build_gameover_modal(lv_ui *ui)
{
    lv_obj_t *overlay = lv_obj_create(ui->screen_3);
    lv_obj_set_size(overlay, lv_obj_get_width(ui->screen_3), lv_obj_get_height(ui->screen_3));
    lv_obj_set_pos(overlay, 0, 0);
    lv_obj_set_style_bg_color(overlay, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(overlay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);     /* 默认隐藏，游戏结束才显示 */

    lv_obj_t *panel = lv_obj_create(overlay);
    lv_obj_set_size(panel, 200, 120);
    lv_obj_center(panel);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0xf2e995), LV_PART_MAIN | LV_STATE_DEFAULT);  /* 分数框同款浅黄 */
    lv_obj_set_style_radius(panel, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(panel, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *title = lv_label_create(panel);
    lv_label_set_text(title, "GAME OVER");
    lv_label_set_long_mode(title, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(title, &lv_font_Acme_Regular_35, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(title, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(title, 190, 40);              /* 高度给足 35px 字体，避免裁切 */
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 6);  /* 标题靠上 */

    lv_obj_t *btn = lv_btn_create(panel);
    lv_obj_set_size(btn, 120, 42);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xbbada0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0); /* 按钮靠下 */
    lv_obj_add_event_cb(btn, try_again_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Try Again");
    lv_obj_set_style_text_font(btn_label, &lv_font_Acme_Regular_30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn_label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(btn_label);

    s_overlay = overlay;
}

/* ---- 手势：方向 -> core.move，棋盘变了才刷新 ---- */
static void game_gesture_cb(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    g2048_dir_t gdir;

    switch (dir) {
    case LV_DIR_LEFT:   gdir = G2048_DIR_LEFT;   break;
    case LV_DIR_RIGHT:  gdir = G2048_DIR_RIGHT;  break;
    case LV_DIR_TOP:    gdir = G2048_DIR_UP;     break;
    case LV_DIR_BOTTOM: gdir = G2048_DIR_DOWN;   break;
    default:            return;
    }

    lv_indev_wait_release(lv_indev_get_act());   /* 防止手势结束后附带触发 click */

    if (!s_game.over && g2048_move(&s_game, gdir)) {
        refresh_all();
        if (g2048_over(&s_game)) {
            lv_obj_clear_flag(s_overlay, LV_OBJ_FLAG_HIDDEN);   /* 游戏结束：弹出提示 */
        }
    }
}

void game2048_ui_screen_created(lv_ui *ui)
{
    srand((unsigned)lv_tick_get());   /* 每次进入 screen_3 播种，保证开局随机 */

    build_tiles(ui);
    setup_score(ui);

    lv_obj_add_event_cb(ui->screen_3, game_gesture_cb, LV_EVENT_GESTURE, ui);

    build_gameover_modal(ui);   /* 最后创建弹窗，保证盖在最上层 */

    g2048_init(&s_game);
    refresh_all();
}

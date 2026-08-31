/*
 * fonts.h — 中文字体声明(ui_components/fonts/)
 * 用 lv_font_conv 从 SourceHanSansSC-Regular.otf 生成的 GB2312 一级字子集。
 * 用法: 给中文 label 设字体 lv_obj_set_style_text_font(label, &lv_font_sourcesans_16, ...)
 */
#ifndef __FONTS_H_
#define __FONTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

LV_FONT_DECLARE(lv_font_sourcesans_16);
LV_FONT_DECLARE(lv_font_sourcesans_20);

#ifdef __cplusplus
}
#endif

#endif /* __FONTS_H_ */

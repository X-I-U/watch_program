/* GUI Guider 1.0 生成代码 与 工程 LVGL 8.4 的兼容补丁。
 *
 * 通过 components/CMakeLists.txt 里的 -include 打进 components 组件所有源文件，
 * 因此 GUI Guider 重新导出覆盖 generated/ 后依然生效，无需每次手动改生成代码。
 *
 * 作用：
 *  1) 先引入真实 lvgl.h，再引入 GUI Guider 数字时钟扩展控件 lv_dclock.h
 *  2) GUI Guider 的 lv_animimg_set_src 多一个第4参 reverse，LVGL 8.4 没有；
 *     用宏把 4 参调用折叠成 3 参（reverse=false 与 LVGL 默认正向播放一致）。
 */
#ifndef GUIDER_COMPAT_H
#define GUIDER_COMPAT_H

#include "lvgl.h"
#include "lv_dclock.h"

#define lv_animimg_set_src(img, dsc, num, reverse) lv_animimg_set_src(img, dsc, num)

#endif /* GUIDER_COMPAT_H */

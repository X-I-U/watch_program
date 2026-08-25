/* GUI Guider 生成的图片/字体文件里用 #include "lvgl/lvgl.h"（带 lvgl/ 前缀），
   而 ESP-IDF 里 LVGL 的主头文件在 include 路径根下叫 lvgl.h。
   这个 shim 让两者对齐；放在 components/ui/lvgl/ 下，重新导出覆盖 generated/ 时不会被冲掉。
   注意必须用尖括号 <lvgl.h>：双引号会先搜本文件所在目录，导致包含到 shim 自己，无限递归。 */
#include <lvgl.h>

#include <stdbool.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "wifista.h"
#include "ui_wifi.h"

/* 未连上 WiFi 时的图标透明度(拉低→灰淡表示无网); 连上恢复满显 */
#define WIFI_ICON_OPA_OFF   LV_OPA_40
#define WIFI_ICON_OPA_ON    255

static lv_obj_t *s_icon       = NULL;
static int       s_last_opacity = -1;   /* -1 = 强制首刷 */

void ui_wifi_refresh(void)
{
    /* 控件句柄每次从 guider_ui 现取, 并做指针变化检测:
       screen2 离开再回来会被 GUI Guider 重建, 需强制重写一次,
       否则变化检测以为没变就漏刷(同 ui_battery 的处理)。 */
    lv_obj_t *icon = guider_ui.screen_2_img_7;
    if (icon != s_icon) {
        s_icon = icon;
        s_last_opacity = -1;
    }
    if (!icon) {
        return;
    }

    int opa = wifista_is_connected() ? WIFI_ICON_OPA_ON : WIFI_ICON_OPA_OFF;
    if (opa != s_last_opacity) {
        s_last_opacity = opa;
        if (lv_obj_is_valid(icon)) {
            lv_obj_set_style_img_opa(icon, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

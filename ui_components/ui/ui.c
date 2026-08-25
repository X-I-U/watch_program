#include "gui_guider.h"
#include "custom.h"
#include "events_init.h"

/* gui_guider.h 里只有 extern 声明，这里补上全局定义 */
lv_ui guider_ui;

void guider_ui_init(void)
{
    setup_ui(&guider_ui);      /* 创建 screen + 控件 + 加载 */
    events_init(&guider_ui);   /* 挂载事件（在 events_init.c 里写逻辑） */
    custom_init(&guider_ui);   /* 自定义逻辑（在 custom.c 里写，重新生成不丢失） */
}

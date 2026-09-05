# Watch Program

一款基于 ESP32-S3 的智能手表固件：LVGL 驱动 240×300 触摸彩屏，内置时钟、日历、音乐、计步、秒表、计算器、2048 等常用功能，并接入 Xiaozhi（小智）AI 语音助手。

> 开发框架：ESP-IDF 5.5.5 + LVGL 8.x（GUI Guider）+ ESP-ADF + esp_xiaozhi
> 主开发分支：`feat/watch`

## 实现的功能

- **桌面主屏**：大字时钟（NTP 自动校时 + RTC 走时）、电量、WiFi 状态图标、应用宫格
- **日历**：月视图，左右滑动无限翻月，今天高亮
- **音乐播放**：在线 mp3，歌单为「歌名+URL」表驱动，动态生成列表，支持播放/暂停/切歌/进度
- **小智 AI 语音**：免提语音问答（TTS），说话时暂停音乐、离开页面不再占用喇叭
- **记步**：BMI270 计步 + 7 天历史柱状图，掉电不丢
- **计时器/秒表**：倒计时到点全局弹窗
- **计算器 / 2048**：纯 C 逻辑核心
- **设置**：WiFi 列表连接、NTP 时间校准

## 代码结构

```
main/                入口与启动编排
basic_components/    驱动/服务层（不碰 LVGL）：屏幕/触摸/电源/RTC/WiFi/咪头/喇叭/小智/记步/计时
ui_components/       UI 层
  ├ ui/generated/    GUI Guider 生成区（重导出会覆盖）
  ├ fonts/           GB2312 全字库（中文显示）
  └ …每屏绑定层       calculator/calendar/game2048/music/xiaozhi_ui/timer_ui/settings_ui/ui_step/ui_time/ui_battery/ui_wifi
adf_components/      从 ESP-ADF 精简拷入的音频组件
managed_components/  组件管理器安装的第三方组件（LVGL、esp_xiaozhi 等）
```

**分层思路**：UI 绑定层只负责翻译用户操作、刷新显示；耗时逻辑（音频/网络）放到后台任务，经命令队列执行，避免阻塞 LVGL。

> 注意：GUI Guider 重导出后，`ui_components/ui/generated/events_init.c` 里各屏的绑定调用需重新加回。

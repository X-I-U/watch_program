set(src_dirs
            calculator
            game2048
            music
            ui
            ui/generated
            ui/generated/guider_fonts
            ui/generated/images
            ui/generated/guider_customer_fonts
            ui/custom
            ui/dclock
            ui/carousel
            ui_time
            calendar
            fonts
            xiaozhi_ui
            timer_ui
            ui_battery
            ui_wifi
            ui_step
            settings_ui)

set(include_dirs
            calculator
            game2048
            music
            ui
            ui/generated
            ui/generated/guider_fonts
            ui/generated/images
            ui/generated/guider_customer_fonts
            ui/custom
            ui/dclock
            ui/carousel
            ui_time
            calendar
            fonts
            xiaozhi_ui
            timer_ui
            ui_battery
            ui_wifi
            ui_step
            settings_ui)

set(requires
            lvgl__lvgl
            basic_components)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)

# GUI Guider 兼容层（components/ui/guider_compat.h）：数字时钟控件 + animimg 4参->3参 折叠
target_compile_definitions(${COMPONENT_LIB} PUBLIC "LV_USE_DCLOCK=1")
target_compile_definitions(${COMPONENT_LIB} PUBLIC "LV_USE_CAROUSEL=1")
# -include 打进所有源文件：无需手动改生成代码，GUI Guider 重新导出覆盖 generated/ 后依然生效
target_compile_options(${COMPONENT_LIB} PRIVATE "-include" "guider_compat.h")
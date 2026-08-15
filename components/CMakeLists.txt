set(src_dirs
            LED
            KEY
            LCD
            MYNTC
            WIFISTA
            espressif__esp_lcd_tk018
            ui
            ui/generated
            ui/generated/guider_fonts
            ui/generated/images
            ui/generated/guider_customer_fonts
            ui/custom
            ui/dclock)

set(include_dirs
            LED
            KEY
            LCD
            MYNTC
            WIFISTA
            espressif__esp_lcd_tk018/include
            ui
            ui/generated
            ui/generated/guider_fonts
            ui/generated/images
            ui/generated/guider_customer_fonts
            ui/custom
            ui/dclock)

set(requires
            driver
            esp_wifi
            lwip
            esp_timer
            esp_lcd
            esp_lcd_touch_cst816s
            esp_lcd_touch_ft5x06
            lvgl__lvgl)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)

# GUI Guider 兼容层（components/ui/guider_compat.h）：数字时钟控件 + animimg 4参->3参 折叠
target_compile_definitions(${COMPONENT_LIB} PUBLIC "LV_USE_DCLOCK=1")
# -include 打进所有源文件：无需手动改生成代码，GUI Guider 重新导出覆盖 generated/ 后依然生效
target_compile_options(${COMPONENT_LIB} PRIVATE "-include" "guider_compat.h")
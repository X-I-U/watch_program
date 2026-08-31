set(src_dirs
            LCD
            MYNTC
            WIFISTA
            espressif__esp_lcd_tk018
            SPEAKER
            AXP2101
            AUDIO_STREAM
            BASIC_TIMER
            MIC_DRIVER
            XIAOZHI
            TIMER_SVC)

set(include_dirs
            LCD
            MYNTC
            WIFISTA
            espressif__esp_lcd_tk018/include
            SPEAKER
            AXP2101
            AUDIO_STREAM
            BASIC_TIMER
            MIC_DRIVER
            XIAOZHI
            TIMER_SVC)

set(requires
            driver
            esp_wifi
            lwip
            esp_timer
            esp_lcd
            esp_lcd_touch_cst816s
            esp_lcd_touch_ft5x06
            lvgl__lvgl
            audio_pipeline
            audio_stream
            esp-adf-libs
            esp_audio_codec
            espressif__esp_xiaozhi)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

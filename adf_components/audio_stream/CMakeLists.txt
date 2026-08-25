set(COMPONENT_SRCS "fatfs_stream.c"
                    "http_stream.c"
                    "http_playlist.c"
                    "raw_stream.c"
                    "spiffs_stream.c"
                    "tcp_client_stream.c"
                    "embed_flash_stream.c"
                    "pwm_stream.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_PRIV_INCLUDEDIRS "lib/hls/include" "lib/gzip/include")
list(APPEND COMPONENT_SRCS  "lib/hls/hls_parse.c"
                            "lib/hls/hls_playlist.c"
                            "lib/hls/line_reader.c"
                            "lib/hls/join_path.c")

if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_LESS "5.0")
    list(APPEND COMPONENT_SRCS  "i2s_stream.c")
else()
    list(APPEND COMPONENT_SRCS  "i2s_stream_idf5.c")
endif()


list(APPEND COMPONENT_SRCS  "lib/gzip/gzip_miniz.c")

# [LOCAL PATCH 2026-08-25] 移除 audio_board / esp_dispatcher / esp_actions / tone_partition：
# 它们会把 esp_peripherals/display_service/audio_hal 等一批在 IDF 5.1 上编译有问题的老组件拖进构建
# （audio_board->esp_peripherals，tone_partition->esp_actions->display_service->audio_board）。
# 本项目用 i2s_stream 时自己提供引脚（见 i2s_stream_idf5.c 的 LOCAL PATCH），不需要板级模型，
# 也不用 tone_stream（蜂鸣音），故一并摘除。
set(COMPONENT_REQUIRES audio_pipeline driver audio_sal esp_http_client tcp_transport spiffs esp-adf-libs bootloader_support mbedtls)

# [LOCAL PATCH 2026-08-25] esp32s3 从 AEC/TTS/算法流分支移除（这些源文件依赖 esp-sr 语音库）。
# 本项目仅用 http/i2s/mp3 播放网络音频，用不到 AEC/TTS/算法流，故跳过以避免拉取 375MB 的 esp-sr。
# 如需恢复：把 esp32s3 加回两个条件，并从 github 拉取 esp-sr 子模块。
if((${IDF_TARGET} STREQUAL "esp32") OR (${IDF_TARGET} STREQUAL "esp32p4")  OR (${IDF_TARGET} STREQUAL "esp32c5"))
list(APPEND COMPONENT_SRCS "aec_stream.c")
list(APPEND COMPONENT_REQUIRES esp-sr)
if((${IDF_TARGET} STREQUAL "esp32") OR (${IDF_TARGET} STREQUAL "esp32p4"))
    list(APPEND COMPONENT_SRCS "algorithm_stream.c" "tts_stream.c")
endif()
endif()

register_component()

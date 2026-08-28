# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# 项目自定义组件目录：原 components 拆分为两个聚合组件。
# 必须写在 project() 之前，否则新组件不会被发现。
set(EXTRA_COMPONENT_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/ui_components
    ${CMAKE_CURRENT_LIST_DIR}/basic_components
    ${CMAKE_CURRENT_LIST_DIR}/adf_components
    ${CMAKE_CURRENT_LIST_DIR}/adf_components/esp-adf-libs/esp_audio_codec)

# [LOCAL PATCH 2026-08-25] adf_components/ 只含 audio_pipeline/audio_sal/audio_stream/esp-adf-libs
# （从 D:\Espressif\esp-adf\components 拷贝）。不能直接指 $ENV{ADF_PATH}/components——那会把 ADF 全部
# ~40 个组件都拉进构建，其中 audio_board/audio_hal/display_service/esp_dispatcher/esp_peripherals 等
# 老组件在 esp32s3 + IDF5.1 上编译不过（audio_board 默认 lyrat_v4_3 是 ESP32 板）。ADF 更新后重新拷贝即可。

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(hello_world)
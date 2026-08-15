idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")
# main 里要 include "lv_dclock.h" 并调用 lv_dclock_set_text_fmt（该头文件内容用 #if LV_USE_DCLOCK 守卫）
target_compile_definitions(${COMPONENT_LIB} PUBLIC "LV_USE_DCLOCK=1")
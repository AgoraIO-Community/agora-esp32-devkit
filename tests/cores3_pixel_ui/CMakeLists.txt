# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.16)
project(cores3_pixel_renderer_host C CXX)
get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(LVGL_ROOT "${REPO_ROOT}/components/lvgl")
file(GLOB_RECURSE LVGL_HOST_SOURCES "${LVGL_ROOT}/src/*.c")
add_library(lvgl_host STATIC ${LVGL_HOST_SOURCES})
target_include_directories(lvgl_host PUBLIC "${LVGL_ROOT}" "${LVGL_ROOT}/src")
target_compile_definitions(lvgl_host PUBLIC LV_CONF_INCLUDE_SIMPLE LV_KCONFIG_IGNORE)
set(RENDERER_ROOT "${REPO_ROOT}/components/mybot_platform/src/drivers/display/renderers/lvgl")
add_executable(test_renderer test_renderer.cc
    "${RENDERER_ROOT}/pixel_expressions.c" "${RENDERER_ROOT}/pixel_fonts.c"
    "${RENDERER_ROOT}/lvgl_fonts.c")
target_compile_features(test_renderer PRIVATE cxx_std_17)
target_compile_options(test_renderer PRIVATE -Wall -Wextra -Werror)
target_compile_definitions(test_renderer PRIVATE CONFIG_MYBOT_LVGL_UI_ANIMATIONS=1)
target_include_directories(test_renderer PRIVATE "mocks/renderer"
    "${REPO_ROOT}/components/mybot_platform/include"
    "${REPO_ROOT}/components/mybot_platform/src/internal"
    "${REPO_ROOT}/components/mybot_stack/mybot_sdk/mybot/include")
target_link_libraries(test_renderer PRIVATE lvgl_host m)
if(CORES3_PIXEL_UI_SANITIZERS)
    foreach(target lvgl_host test_renderer)
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    endforeach()
    target_link_options(test_renderer PRIVATE -fsanitize=address,undefined)
endif()

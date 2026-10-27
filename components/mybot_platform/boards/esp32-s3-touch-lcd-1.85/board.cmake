# SPDX-License-Identifier: Apache-2.0

set(MYBOT_BOARD_TARGET "esp32s3")
set(MYBOT_BOARD_SDKCONFIG_DEFAULTS "${CMAKE_CURRENT_LIST_DIR}/sdkconfig.defaults")
set(MYBOT_BOARD_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(MYBOT_BOARD_REQUIRED_CONFIGS
    CONFIG_PARTITION_TABLE_CUSTOM
    CONFIG_ESPTOOLPY_FLASHSIZE_16MB
    CONFIG_ESPTOOLPY_FLASHMODE_QIO
    CONFIG_SPIRAM
    CONFIG_SPIRAM_MODE_OCT
    CONFIG_SPIRAM_SPEED_80M
)
set(MYBOT_BOARD_PARTITION_TABLE "partitions/v2/16m.csv")

get_filename_component(MYBOT_PLATFORM_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
include("${MYBOT_PLATFORM_ROOT}/src/drivers/display/display.cmake")
set(MYBOT_BOARD_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/board.c"
    "${CMAKE_CURRENT_LIST_DIR}/lcd185_hardware.c"
    "${MYBOT_PLATFORM_ROOT}/src/drivers/io_expander/tca9554.c"
    "${MYBOT_PLATFORM_ROOT}/src/drivers/input/cst816s_boot_input.c"
    "${MYBOT_PLATFORM_ROOT}/src/drivers/audio/raw_i2s_audio.c"
)
mybot_display_add_panel(MYBOT_BOARD_SOURCES "st77916/st77916_lcd185_panel.c")
mybot_display_add_lvgl_sources(MYBOT_BOARD_SOURCES "shared_lvgl_adapter.cc")
set(MYBOT_BOARD_REQUIRES
    button
    esp_driver_gpio
    esp_driver_i2c
    esp_driver_i2s
    esp_driver_ledc
    esp_driver_spi
    esp_lcd
    esp_lcd_st77916
    esp_lcd_touch
    esp_lcd_touch_cst816s
    esp_lvgl_port
    lvgl
)

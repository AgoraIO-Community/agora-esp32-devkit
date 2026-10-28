# SPDX-License-Identifier: Apache-2.0

set(MYBOT_BOARD_TARGET "esp32s3")
get_filename_component(MYBOT_GENERIC_BOARD_DIR "${MYBOT_BOARD_PROFILE}" DIRECTORY)
get_filename_component(MYBOT_GENERIC_COMMON_DIR
    "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
set(MYBOT_BOARD_SDKCONFIG_DEFAULTS "${MYBOT_GENERIC_BOARD_DIR}/sdkconfig.defaults")
if(NOT MYBOT_GENERIC_DISPLAY)
    set(MYBOT_BOARD_HEADLESS TRUE)
endif()
set(MYBOT_BOARD_INCLUDE_DIR
    "${MYBOT_GENERIC_BOARD_DIR}"
    "${MYBOT_GENERIC_COMMON_DIR}"
    "${MYBOT_GENERIC_COMMON_DIR}/../esp32-s3-touch-amoled-1.75-common"
)
set(MYBOT_BOARD_PARTITION_TABLE "${MYBOT_GENERIC_PARTITION_TABLE}")
set(MYBOT_BOARD_REQUIRED_CONFIGS ${MYBOT_GENERIC_REQUIRED_CONFIGS})
set(MYBOT_BOARD_FORBIDDEN_CONFIGS ${MYBOT_GENERIC_FORBIDDEN_CONFIGS})

get_filename_component(MYBOT_PLATFORM_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(MYBOT_BOARD_SOURCES
    "${MYBOT_GENERIC_BOARD_DIR}/board.c"
    "${MYBOT_GENERIC_COMMON_DIR}/generic_board.c"
    "${MYBOT_GENERIC_COMMON_DIR}/generic_hardware.c"
    ${MYBOT_GENERIC_EXTRA_SOURCES}
)

if(MYBOT_GENERIC_DISPLAY)
    include("${MYBOT_PLATFORM_ROOT}/src/drivers/display/display.cmake")
    mybot_display_add_panel(MYBOT_BOARD_SOURCES "${MYBOT_GENERIC_DISPLAY}")
    mybot_display_add_lvgl_sources(MYBOT_BOARD_SOURCES "shared_lvgl_adapter.cc")
endif()

if(MYBOT_GENERIC_AUDIO STREQUAL "es8311")
    list(APPEND MYBOT_BOARD_SOURCES "${MYBOT_PLATFORM_ROOT}/src/drivers/audio/es8311_codec_audio.c")
elseif(MYBOT_GENERIC_AUDIO STREQUAL "raw")
    list(APPEND MYBOT_BOARD_SOURCES "${MYBOT_PLATFORM_ROOT}/src/drivers/audio/raw_i2s_audio.c")
elseif(MYBOT_GENERIC_AUDIO STREQUAL "dual")
    list(APPEND MYBOT_BOARD_SOURCES
        "${MYBOT_PLATFORM_ROOT}/src/drivers/audio/amoled175_codec_audio.c")
elseif(MYBOT_GENERIC_AUDIO STREQUAL "es8374")
    list(APPEND MYBOT_BOARD_SOURCES
        "${MYBOT_PLATFORM_ROOT}/src/drivers/audio/es8311_codec_audio.c"
        "${MYBOT_PLATFORM_ROOT}/src/drivers/audio/es8374_codec.c")
endif()

if(MYBOT_GENERIC_TOUCH)
    list(APPEND MYBOT_BOARD_SOURCES
        "${MYBOT_PLATFORM_ROOT}/src/drivers/input/cst816s_boot_input.c")
    if(NOT MYBOT_GENERIC_TOUCH_DRIVER OR MYBOT_GENERIC_TOUCH_DRIVER STREQUAL "cst816s")
        list(APPEND MYBOT_GENERIC_EXTRA_REQUIRES
            esp_lcd_touch
            esp_lcd_touch_cst816s)
    elseif(MYBOT_GENERIC_TOUCH_DRIVER STREQUAL "ft5x06")
        list(APPEND MYBOT_GENERIC_EXTRA_REQUIRES
            esp_lcd_touch
            esp_lcd_touch_ft5x06)
    elseif(MYBOT_GENERIC_TOUCH_DRIVER STREQUAL "gt911")
        list(APPEND MYBOT_GENERIC_EXTRA_REQUIRES
            esp_lcd_touch
            esp_lcd_touch_gt911)
    else()
        message(FATAL_ERROR "Unknown MYBOT_GENERIC_TOUCH_DRIVER")
    endif()
else()
    list(APPEND MYBOT_BOARD_SOURCES
        "${MYBOT_PLATFORM_ROOT}/src/drivers/input/gpio_buttons.c")
endif()

if(MYBOT_GENERIC_PCA9557)
    list(APPEND MYBOT_BOARD_SOURCES
        "${MYBOT_PLATFORM_ROOT}/src/drivers/io_expander/pca9557.c")
endif()

if(MYBOT_GENERIC_TCA9554)
    list(APPEND MYBOT_BOARD_SOURCES
        "${MYBOT_PLATFORM_ROOT}/src/drivers/io_expander/tca9554.c")
endif()

if(MYBOT_GENERIC_AW9523B)
    list(APPEND MYBOT_BOARD_SOURCES
        "${MYBOT_PLATFORM_ROOT}/src/drivers/io_expander/aw9523b.c")
endif()

set(MYBOT_BOARD_REQUIRES
    button
    esp_codec_dev
    esp_driver_gpio
    esp_driver_i2c
    esp_driver_i2s
    esp_driver_ledc
    esp_driver_spi
    esp_lcd
    ${MYBOT_GENERIC_EXTRA_REQUIRES}
)

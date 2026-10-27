# SPDX-License-Identifier: Apache-2.0
idf_component_register(
    SRCS "esp_lcd_nv3023.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_lcd
    PRIV_REQUIRES driver
)
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    ESP_LCD_NV3023_VER_MAJOR=1
    ESP_LCD_NV3023_VER_MINOR=0
    ESP_LCD_NV3023_VER_PATCH=1
)

# SPDX-License-Identifier: Apache-2.0
idf_component_register(
    SRCS "esp_lcd_sh8601.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_lcd
    PRIV_REQUIRES driver
)
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    ESP_LCD_SH8601_VER_MAJOR=1
    ESP_LCD_SH8601_VER_MINOR=0
    ESP_LCD_SH8601_VER_PATCH=2
)

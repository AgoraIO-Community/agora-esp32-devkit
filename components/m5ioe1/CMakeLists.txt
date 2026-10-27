# SPDX-License-Identifier: MIT
idf_component_register(
    SRCS "src/M5IOE1.cpp"
    INCLUDE_DIRS "src"
    REQUIRES driver esp_driver_gpio esp_driver_i2c esp_rom
)

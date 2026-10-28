/* SPDX-License-Identifier: Apache-2.0 */
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"

#include <stdint.h>

#define TAG "atk_hardware"
#define ATK_EXPANDER_ADDRESS 0x20
#define ATK_EXPANDER_TIMEOUT_MS 100

static esp_err_t write_register(i2c_master_dev_handle_t device, uint8_t reg, uint8_t value) {
    const uint8_t command[] = {reg, value};
    return i2c_master_transmit(device, command, sizeof(command), ATK_EXPANDER_TIMEOUT_MS);
}

static esp_err_t read_register(i2c_master_dev_handle_t device, uint8_t reg, uint8_t *value) {
    esp_err_t result = i2c_master_transmit(device, &reg, sizeof(reg), ATK_EXPANDER_TIMEOUT_MS);
    if (result == ESP_OK) {
        result = i2c_master_receive(device, value, 1, ATK_EXPANDER_TIMEOUT_MS);
    }
    return result;
}

int mybot_generic_board_extra_init(i2c_master_bus_handle_t bus) {
    const i2c_device_config_t config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ATK_EXPANDER_ADDRESS,
        .scl_speed_hz = 400000,
    };
    i2c_master_dev_handle_t device = NULL;
    esp_err_t result = i2c_master_bus_add_device(bus, &config, &device);
    uint8_t output_low = 0;
    uint8_t output_high = 0;

    if (result == ESP_OK) {
        result = write_register(device, 0x06, 0x03);
    }
    if (result == ESP_OK) {
        result = write_register(device, 0x07, 0xF0);
    }
    if (result == ESP_OK) {
        result = read_register(device, 0x02, &output_low);
    }
    if (result == ESP_OK) {
        result = write_register(device, 0x02, output_low & (uint8_t) ~(1U << 2));
    }
    if (result == ESP_OK) {
        result = read_register(device, 0x03, &output_high);
    }
    if (result == ESP_OK) {
        result = write_register(device, 0x03, output_high | (uint8_t)(1U << 0));
    }
    if (device) {
        const esp_err_t remove_result = i2c_master_bus_rm_device(device);
        if (result == ESP_OK) {
            result = remove_result;
        }
    }
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "event=lcd_power action=initialize result=error code=%s",
                 esp_err_to_name(result));
        return -1;
    }
    return 0;
}

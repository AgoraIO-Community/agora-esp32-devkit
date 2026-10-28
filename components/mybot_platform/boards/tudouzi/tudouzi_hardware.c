/* SPDX-License-Identifier: Apache-2.0 */
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"

#include <stddef.h>
#include <stdint.h>

#define TAG "tudouzi_hardware"
#define AXP2101_ADDRESS 0x34
#define AXP2101_TIMEOUT_MS 100

typedef struct {
    uint8_t reg;
    uint8_t value;
} register_value_t;

static esp_err_t write_register(i2c_master_dev_handle_t device, uint8_t reg, uint8_t value) {
    const uint8_t command[] = {reg, value};
    return i2c_master_transmit(device, command, sizeof(command), AXP2101_TIMEOUT_MS);
}

static esp_err_t read_register(i2c_master_dev_handle_t device, uint8_t reg, uint8_t *value) {
    esp_err_t result = i2c_master_transmit(device, &reg, sizeof(reg), AXP2101_TIMEOUT_MS);
    if (result == ESP_OK) {
        result = i2c_master_receive(device, value, 1, AXP2101_TIMEOUT_MS);
    }
    return result;
}

int mybot_generic_board_extra_init(i2c_master_bus_handle_t bus) {
    static const register_value_t registers[] = {
        {0x22, 0x06}, {0x27, 0x10}, {0x93, 0x1C}, {0x64, 0x03}, {0x61, 0x05}, {0x62, 0x0A},
        {0x63, 0x15}, {0x14, 0x00}, {0x15, 0x00}, {0x16, 0x05}, {0x24, 0x01}, {0x50, 0x14},
    };
    const i2c_device_config_t config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = AXP2101_ADDRESS,
        .scl_speed_hz = 400000,
    };
    i2c_master_dev_handle_t device = NULL;
    esp_err_t result = i2c_master_bus_add_device(bus, &config, &device);
    for (size_t index = 0; result == ESP_OK && index < sizeof(registers) / sizeof(registers[0]);
         ++index) {
        result = write_register(device, registers[index].reg, registers[index].value);
    }
    uint8_t power_state = 0;
    if (result == ESP_OK) {
        result = read_register(device, 0x90, &power_state);
    }
    if (result == ESP_OK) {
        result = write_register(device, 0x90, power_state | 0x02);
    }
    if (device) {
        const esp_err_t remove_result = i2c_master_bus_rm_device(device);
        if (result == ESP_OK) {
            result = remove_result;
        }
    }
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "event=power action=initialize pmic=axp2101 result=error code=%s",
                 esp_err_to_name(result));
        return -1;
    }
    return 0;
}

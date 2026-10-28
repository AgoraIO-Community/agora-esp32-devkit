/* SPDX-License-Identifier: Apache-2.0 */
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"

#include <stdint.h>

#define TAG "lilygo_tdisplay_hardware"
#define SY6970_ADDRESS 0x6A
#define SY6970_TIMEOUT_MS 100

static esp_err_t write_register(i2c_master_dev_handle_t device, uint8_t reg, uint8_t value) {
    const uint8_t command[] = {reg, value};
    return i2c_master_transmit(device, command, sizeof(command), SY6970_TIMEOUT_MS);
}

int mybot_generic_board_extra_init(i2c_master_bus_handle_t bus) {
    const i2c_device_config_t config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SY6970_ADDRESS,
        .scl_speed_hz = 400000,
    };
    i2c_master_dev_handle_t device = NULL;
    esp_err_t result = i2c_master_bus_add_device(bus, &config, &device);
    if (result == ESP_OK) {
        result = write_register(device, 0x00, 0x08);
    }
    if (result == ESP_OK) {
        result = write_register(device, 0x02, 0xDD);
    }
    if (result == ESP_OK) {
        result = write_register(device, 0x07, 0x8D);
    }
    if (device) {
        const esp_err_t remove_result = i2c_master_bus_rm_device(device);
        if (result == ESP_OK) {
            result = remove_result;
        }
    }
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "event=power action=initialize pmic=sy6970 result=error code=%s",
                 esp_err_to_name(result));
        return -1;
    }
    return 0;
}

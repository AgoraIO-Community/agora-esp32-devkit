/* SPDX-License-Identifier: Apache-2.0 */
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"

#include <stdint.h>

#define TAG "labplus_hardware"
#define LABPLUS_STM8_ADDRESS 0x11
#define LABPLUS_STM8_RESET_REGISTER 0x04
#define LABPLUS_I2C_TIMEOUT_MS 100

int mybot_generic_board_extra_init(i2c_master_bus_handle_t bus) {
    const i2c_device_config_t config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LABPLUS_STM8_ADDRESS,
        .scl_speed_hz = 400000,
    };
    i2c_master_dev_handle_t device = NULL;
    esp_err_t result = i2c_master_bus_add_device(bus, &config, &device);
    if (result == ESP_OK) {
        const uint8_t command = LABPLUS_STM8_RESET_REGISTER;
        result = i2c_master_transmit(device, &command, sizeof(command), LABPLUS_I2C_TIMEOUT_MS);
    }
    if (device) {
        const esp_err_t remove_result = i2c_master_bus_rm_device(device);
        if (result == ESP_OK) {
            result = remove_result;
        }
    }
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "event=panel_reset action=initialize result=error code=%s",
                 esp_err_to_name(result));
        return -1;
    }
    return 0;
}

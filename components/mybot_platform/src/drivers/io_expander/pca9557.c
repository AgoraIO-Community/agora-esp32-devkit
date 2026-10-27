/* SPDX-License-Identifier: Apache-2.0 */
#include "io_expander/pca9557.h"

#include "esp_err.h"
#include "esp_log.h"

#define TAG "pca9557"
#define PCA9557_I2C_TIMEOUT_MS 100
#define PCA9557_REG_OUTPUT 0x01
#define PCA9557_REG_CONFIG 0x03

static int write_register(mybot_pca9557_t *expander, uint8_t reg, uint8_t value) {
    const uint8_t command[] = {reg, value};
    const esp_err_t result =
        i2c_master_transmit(expander->device, command, sizeof(command), PCA9557_I2C_TIMEOUT_MS);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "event=io_expander action=write register=0x%02x result=error code=%s", reg,
                 esp_err_to_name(result));
        return -1;
    }
    return 0;
}

int mybot_pca9557_init(mybot_pca9557_t *expander, i2c_master_bus_handle_t bus, uint8_t address,
                       uint32_t speed_hz) {
    if (!expander || !bus || expander->device) {
        return -1;
    }
    *expander = (mybot_pca9557_t){.bus = bus};
    const i2c_device_config_t config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = address,
        .scl_speed_hz = speed_hz,
    };
    const esp_err_t result = i2c_master_bus_add_device(bus, &config, &expander->device);
    if (result != ESP_OK) {
        expander->device = NULL;
        ESP_LOGE(TAG, "event=io_expander action=initialize address=0x%02x result=error code=%s",
                 address, esp_err_to_name(result));
        return -1;
    }
    return 0;
}

int mybot_pca9557_deinit(mybot_pca9557_t *expander) {
    if (!expander) {
        return -1;
    }
    if (expander->device && i2c_master_bus_rm_device(expander->device) != ESP_OK) {
        return -1;
    }
    expander->device = NULL;
    expander->bus = NULL;
    expander->output_state = 0;
    return 0;
}

int mybot_pca9557_configure_outputs(mybot_pca9557_t *expander, uint8_t output_mask,
                                    uint8_t high_mask) {
    if (!expander || !expander->device) {
        return -1;
    }
    const uint8_t state = high_mask & output_mask;
    if (write_register(expander, PCA9557_REG_OUTPUT, state) < 0 ||
        write_register(expander, PCA9557_REG_CONFIG, (uint8_t)~output_mask) < 0) {
        return -1;
    }
    expander->output_state = state;
    return 0;
}

int mybot_pca9557_set_levels(mybot_pca9557_t *expander, uint8_t mask, uint8_t high_mask) {
    if (!expander || !expander->device) {
        return -1;
    }
    const uint8_t state = (uint8_t)((expander->output_state & ~mask) | (high_mask & mask));
    if (write_register(expander, PCA9557_REG_OUTPUT, state) < 0) {
        return -1;
    }
    expander->output_state = state;
    return 0;
}

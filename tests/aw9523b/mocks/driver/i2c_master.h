/* SPDX-License-Identifier: MIT */
#ifndef AW9523B_I2C_MASTER_MOCK_H_
#define AW9523B_I2C_MASTER_MOCK_H_

#include <stddef.h>
#include <stdint.h>

typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define I2C_ADDR_BIT_LEN_7 0

typedef void *i2c_master_bus_handle_t;
typedef void *i2c_master_dev_handle_t;

typedef struct {
    int dev_addr_length;
    uint16_t device_address;
    uint32_t scl_speed_hz;
} i2c_device_config_t;

esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus, const i2c_device_config_t *config,
                                    i2c_master_dev_handle_t *out_device);
esp_err_t i2c_master_bus_rm_device(i2c_master_dev_handle_t device);
esp_err_t i2c_master_transmit(i2c_master_dev_handle_t device, const uint8_t *data, size_t size,
                              int timeout_ms);
const char *esp_err_to_name(esp_err_t error);

#endif /* AW9523B_I2C_MASTER_MOCK_H_ */

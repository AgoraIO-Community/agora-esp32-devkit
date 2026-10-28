/* SPDX-License-Identifier: MIT */
#ifndef BOARD_POWER_I2C_MASTER_H_
#define BOARD_POWER_I2C_MASTER_H_

#include <stddef.h>
#include <stdint.h>

typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1

typedef struct mock_i2c_bus *i2c_master_bus_handle_t;
typedef struct mock_i2c_device *i2c_master_dev_handle_t;

typedef enum {
    I2C_ADDR_BIT_LEN_7 = 0,
} i2c_addr_bit_len_t;

typedef struct {
    i2c_addr_bit_len_t dev_addr_length;
    uint16_t device_address;
    uint32_t scl_speed_hz;
} i2c_device_config_t;

esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus, const i2c_device_config_t *config,
                                    i2c_master_dev_handle_t *out_device);
esp_err_t i2c_master_bus_rm_device(i2c_master_dev_handle_t device);
esp_err_t i2c_master_transmit(i2c_master_dev_handle_t device, const uint8_t *data, size_t size,
                              int timeout_ms);
esp_err_t i2c_master_receive(i2c_master_dev_handle_t device, uint8_t *data, size_t size,
                             int timeout_ms);

#endif /* BOARD_POWER_I2C_MASTER_H_ */

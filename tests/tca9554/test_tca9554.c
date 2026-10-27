/* SPDX-License-Identifier: MIT */
#include "io_expander/tca9554.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);                   \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)

struct mock_i2c_bus {
    int unused;
};
struct mock_i2c_device {
    i2c_master_bus_handle_t bus;
    uint16_t address;
    uint32_t speed_hz;
    bool open;
};

static struct mock_i2c_bus g_bus;
static struct mock_i2c_device g_device;
static uint8_t g_register;
static uint8_t g_value;
static bool g_fail_add;
static bool g_fail_remove;
static bool g_fail_write;
static unsigned int g_writes;

const char *esp_err_to_name(esp_err_t error) {
    return error == ESP_OK ? "ESP_OK" : "ESP_FAIL";
}

esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus, const i2c_device_config_t *config,
                                    i2c_master_dev_handle_t *out_device) {
    CHECK(bus == &g_bus && !g_device.open);
    CHECK(config->dev_addr_length == I2C_ADDR_BIT_LEN_7);
    CHECK(config->device_address == 0x20 && config->scl_speed_hz == 400000);
    if (g_fail_add) {
        g_fail_add = false;
        return ESP_FAIL;
    }
    g_device = (struct mock_i2c_device){
        .bus = bus,
        .address = config->device_address,
        .speed_hz = config->scl_speed_hz,
        .open = true,
    };
    *out_device = &g_device;
    return ESP_OK;
}

esp_err_t i2c_master_bus_rm_device(i2c_master_dev_handle_t device) {
    CHECK(device == &g_device && g_device.open);
    if (g_fail_remove) {
        g_fail_remove = false;
        return ESP_FAIL;
    }
    g_device.open = false;
    return ESP_OK;
}

esp_err_t i2c_master_transmit(i2c_master_dev_handle_t device, const uint8_t *data, size_t size,
                              int timeout_ms) {
    CHECK(device == &g_device && device->open);
    CHECK(data && size == 2 && timeout_ms == 100);
    if (g_fail_write) {
        g_fail_write = false;
        return ESP_FAIL;
    }
    g_register = data[0];
    g_value = data[1];
    ++g_writes;
    return ESP_OK;
}

int main(void) {
    mybot_tca9554_t expander = {0};
    CHECK(mybot_tca9554_init(NULL, &g_bus, 0x20, 400000) < 0);
    CHECK(mybot_tca9554_init(&expander, NULL, 0x20, 400000) < 0);
    g_fail_add = true;
    CHECK(mybot_tca9554_init(&expander, &g_bus, 0x20, 400000) < 0);
    CHECK(!expander.device);

    CHECK(mybot_tca9554_init(&expander, &g_bus, 0x20, 400000) == 0);
    CHECK(expander.device == &g_device && expander.output_state == 0);
    CHECK(mybot_tca9554_init(&expander, &g_bus, 0x20, 400000) < 0);

    CHECK(mybot_tca9554_configure_outputs(&expander, 0x07, 0x02) == 0);
    CHECK(g_register == 0x03 && g_value == 0xf8);
    CHECK(g_writes == 2 && expander.output_state == 0x02);
    CHECK(mybot_tca9554_set_levels(&expander, 0x01, 0x01) == 0);
    CHECK(g_register == 0x01 && g_value == 0x03);
    CHECK(mybot_tca9554_set_levels(&expander, 0x02, 0x02) == 0);
    CHECK(g_value == 0x03);
    CHECK(mybot_tca9554_set_levels(&expander, 0x03, 0x00) == 0);
    CHECK(g_value == 0x00 && expander.output_state == 0);

    g_fail_write = true;
    CHECK(mybot_tca9554_set_levels(&expander, 0x01, 0x01) < 0);
    CHECK(expander.output_state == 0);
    CHECK(mybot_tca9554_set_levels(&expander, 0x01, 0x01) == 0);
    CHECK(expander.output_state == 0x01);

    g_fail_remove = true;
    CHECK(mybot_tca9554_deinit(&expander) < 0);
    CHECK(expander.device == &g_device);
    CHECK(mybot_tca9554_deinit(&expander) == 0);
    CHECK(!expander.device && !expander.bus && expander.output_state == 0);
    CHECK(mybot_tca9554_deinit(&expander) == 0);
    puts("TCA9554 masked update host tests passed");
    return 0;
}

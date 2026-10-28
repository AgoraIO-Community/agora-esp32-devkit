/* SPDX-License-Identifier: MIT */
#include "driver/i2c_master.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int mybot_generic_board_extra_init(i2c_master_bus_handle_t bus);

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
static bool g_fail_add;
static bool g_fail_transmit;
static bool g_fail_remove;
static unsigned int g_adds;
static unsigned int g_removes;
static unsigned int g_transmits;
static uint8_t g_command;
static size_t g_command_size;
static int g_timeout_ms;

static void reset_mock(void) {
    g_device = (struct mock_i2c_device){0};
    g_fail_add = false;
    g_fail_transmit = false;
    g_fail_remove = false;
    g_adds = 0;
    g_removes = 0;
    g_transmits = 0;
    g_command = 0;
    g_command_size = 0;
    g_timeout_ms = 0;
}

const char *esp_err_to_name(esp_err_t error) {
    return error == ESP_OK ? "ESP_OK" : "ESP_FAIL";
}

esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus, const i2c_device_config_t *config,
                                    i2c_master_dev_handle_t *out_device) {
    CHECK(bus == &g_bus && config && out_device && !g_device.open);
    CHECK(config->dev_addr_length == I2C_ADDR_BIT_LEN_7);
    CHECK(config->device_address == 0x11 && config->scl_speed_hz == 400000);
    ++g_adds;
    if (g_fail_add) {
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
    CHECK(device == &g_device && device->open);
    ++g_removes;
    g_device.open = false;
    return g_fail_remove ? ESP_FAIL : ESP_OK;
}

esp_err_t i2c_master_transmit(i2c_master_dev_handle_t device, const uint8_t *data, size_t size,
                              int timeout_ms) {
    CHECK(device == &g_device && device->open && data);
    ++g_transmits;
    g_command = data[0];
    g_command_size = size;
    g_timeout_ms = timeout_ms;
    return g_fail_transmit ? ESP_FAIL : ESP_OK;
}

static void expect_command(void) {
    CHECK(g_adds == 1 && g_transmits == 1 && g_removes == 1);
    CHECK(g_command == 0x04 && g_command_size == 1 && g_timeout_ms == 100);
    CHECK(!g_device.open);
}

int main(void) {
    reset_mock();
    CHECK(mybot_generic_board_extra_init(&g_bus) == 0);
    expect_command();

    reset_mock();
    g_fail_transmit = true;
    CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
    expect_command();

    reset_mock();
    g_fail_remove = true;
    CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
    expect_command();

    reset_mock();
    g_fail_add = true;
    CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
    CHECK(g_adds == 1 && g_transmits == 0 && g_removes == 0);

    puts("LabPlus STM8 reset host tests passed");
    return 0;
}

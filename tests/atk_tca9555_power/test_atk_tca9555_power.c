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
    bool open;
};
typedef struct {
    uint8_t reg;
    uint8_t value;
} write_record_t;

static struct mock_i2c_bus g_bus;
static struct mock_i2c_device g_device;
static write_record_t g_writes[8];
static uint8_t g_read_registers[8];
static unsigned int g_write_count;
static unsigned int g_read_count;
static unsigned int g_add_count;
static unsigned int g_remove_count;
static uint8_t g_output_low;
static uint8_t g_output_high;
static bool g_fail_add;
static bool g_fail_write;
static bool g_fail_read;
static bool g_fail_remove;

static void reset_mock(void) {
    g_device = (struct mock_i2c_device){0};
    g_write_count = 0;
    g_read_count = 0;
    g_add_count = 0;
    g_remove_count = 0;
    g_output_low = 0x04;
    g_output_high = 0x00;
    g_fail_add = false;
    g_fail_write = false;
    g_fail_read = false;
    g_fail_remove = false;
}

const char *esp_err_to_name(esp_err_t error) {
    return error == ESP_OK ? "ESP_OK" : "ESP_FAIL";
}

esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus, const i2c_device_config_t *config,
                                    i2c_master_dev_handle_t *out_device) {
    CHECK(bus == &g_bus && config && out_device && !g_device.open);
    CHECK(config->dev_addr_length == I2C_ADDR_BIT_LEN_7);
    CHECK(config->device_address == 0x20 && config->scl_speed_hz == 400000);
    ++g_add_count;
    if (g_fail_add) {
        return ESP_FAIL;
    }
    g_device.open = true;
    *out_device = &g_device;
    return ESP_OK;
}

esp_err_t i2c_master_bus_rm_device(i2c_master_dev_handle_t device) {
    CHECK(device == &g_device && device->open);
    ++g_remove_count;
    g_device.open = false;
    return g_fail_remove ? ESP_FAIL : ESP_OK;
}

esp_err_t i2c_master_transmit(i2c_master_dev_handle_t device, const uint8_t *data, size_t size,
                              int timeout_ms) {
    CHECK(device == &g_device && device->open && data && timeout_ms == 100);
    if (g_fail_write) {
        return ESP_FAIL;
    }
    if (size == 2) {
        CHECK(g_write_count < sizeof(g_writes) / sizeof(g_writes[0]));
        g_writes[g_write_count++] = (write_record_t){.reg = data[0], .value = data[1]};
    } else {
        CHECK(size == 1);
        g_read_registers[g_read_count++] = data[0];
    }
    return ESP_OK;
}

esp_err_t i2c_master_receive(i2c_master_dev_handle_t device, uint8_t *data, size_t size,
                             int timeout_ms) {
    CHECK(device == &g_device && device->open && data && size == 1 && timeout_ms == 100);
    if (g_fail_read) {
        return ESP_FAIL;
    }
    const uint8_t reg = g_read_registers[g_read_count - 1];
    CHECK((reg == 0x02 || reg == 0x03));
    *data = reg == 0x02 ? g_output_low : g_output_high;
    return ESP_OK;
}

static void expect_success(void) {
    CHECK(g_add_count == 1 && g_remove_count == 1 && g_write_count == 4);
    CHECK(g_writes[0].reg == 0x06 && g_writes[0].value == 0x03);
    CHECK(g_writes[1].reg == 0x07 && g_writes[1].value == 0xF0);
    CHECK(g_writes[2].reg == 0x02 && g_writes[2].value == 0x00);
    CHECK(g_writes[3].reg == 0x03 && g_writes[3].value == 0x01);
    CHECK(g_read_count == 2 && !g_device.open);
}

int main(void) {
    reset_mock();
    CHECK(mybot_generic_board_extra_init(&g_bus) == 0);
    expect_success();

    reset_mock();
    g_fail_write = true;
    CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
    CHECK(g_add_count == 1 && g_remove_count == 1 && !g_device.open);

    reset_mock();
    g_fail_read = true;
    CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
    CHECK(g_add_count == 1 && g_remove_count == 1 && !g_device.open);

    reset_mock();
    g_fail_remove = true;
    CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
    CHECK(g_add_count == 1 && g_remove_count == 1 && !g_device.open);

    reset_mock();
    g_fail_add = true;
    CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
    CHECK(g_add_count == 1 && g_remove_count == 0 && g_write_count == 0);

    puts("ATK TCA9555 display-power host tests passed");
    return 0;
}

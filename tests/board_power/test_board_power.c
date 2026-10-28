/* SPDX-License-Identifier: MIT */
#include "driver/i2c_master.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(TEST_SY6970) + defined(TEST_ATK_BOX) + defined(TEST_TUDOUZI) != 1
#error "Select exactly one board power profile"
#endif

int mybot_generic_board_extra_init(i2c_master_bus_handle_t bus);

#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);                   \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)

typedef struct {
    uint8_t reg;
    uint8_t value;
} write_record_t;

struct mock_i2c_bus {
    int unused;
};

struct mock_i2c_device {
    bool open;
};

static struct mock_i2c_bus g_bus;
static struct mock_i2c_device g_device;
static write_record_t g_writes[16];
static uint8_t g_read_registers[4];
static unsigned int g_write_count;
static unsigned int g_read_count;
static unsigned int g_add_count;
static unsigned int g_remove_count;
static unsigned int g_write_attempts;
static unsigned int g_fail_write_at;
static bool g_fail_add;
static bool g_fail_read;
static bool g_fail_remove;

#ifdef TEST_SY6970
#define EXPECTED_ADDRESS 0x6A
#define EXPECTED_WRITES 3
#define EXPECTED_READS 0
#define EXPECTED_OUTPUT_READ 0
static const write_record_t s_expected_writes[] = {
    {0x00, 0x08},
    {0x02, 0xDD},
    {0x07, 0x8D},
};
#elif defined(TEST_ATK_BOX)
#define EXPECTED_ADDRESS 0x20
#define EXPECTED_WRITES 5
#define EXPECTED_READS 1
#define EXPECTED_OUTPUT_READ 0x02
static const write_record_t s_expected_writes[] = {
    {0x06, 0x3B}, {0x07, 0xFE}, {0x06, 0x1B}, {0x07, 0xFE}, {0x02, 0xA4},
};
#else
#define EXPECTED_ADDRESS 0x34
#define EXPECTED_WRITES 13
#define EXPECTED_READS 1
#define EXPECTED_OUTPUT_READ 0x90
static const write_record_t s_expected_writes[] = {
    {0x22, 0x06}, {0x27, 0x10}, {0x93, 0x1C}, {0x64, 0x03}, {0x61, 0x05},
    {0x62, 0x0A}, {0x63, 0x15}, {0x14, 0x00}, {0x15, 0x00}, {0x16, 0x05},
    {0x24, 0x01}, {0x50, 0x14}, {0x90, 0x16},
};
#endif

static void reset_mock(void) {
    g_device = (struct mock_i2c_device){0};
    g_write_count = 0;
    g_read_count = 0;
    g_add_count = 0;
    g_remove_count = 0;
    g_write_attempts = 0;
    g_fail_write_at = 0;
    g_fail_add = false;
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
    CHECK(config->device_address == EXPECTED_ADDRESS && config->scl_speed_hz == 400000);
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
    if (g_fail_remove) {
        return ESP_FAIL;
    }
    g_device.open = false;
    return ESP_OK;
}

esp_err_t i2c_master_transmit(i2c_master_dev_handle_t device, const uint8_t *data, size_t size,
                              int timeout_ms) {
    CHECK(device == &g_device && device->open && data && timeout_ms == 100);
    if (size == 1) {
        CHECK(g_read_count < sizeof(g_read_registers));
        g_read_registers[g_read_count++] = data[0];
        return ESP_OK;
    }
    CHECK(size == 2 && g_write_count < sizeof(g_writes) / sizeof(g_writes[0]));
    ++g_write_attempts;
    if (g_fail_write_at && g_write_attempts == g_fail_write_at) {
        return ESP_FAIL;
    }
    g_writes[g_write_count++] = (write_record_t){.reg = data[0], .value = data[1]};
    return ESP_OK;
}

esp_err_t i2c_master_receive(i2c_master_dev_handle_t device, uint8_t *data, size_t size,
                             int timeout_ms) {
    CHECK(device == &g_device && device->open && data && size == 1 && timeout_ms == 100);
    CHECK(g_read_count > 0 && g_read_registers[g_read_count - 1] == EXPECTED_OUTPUT_READ);
    if (g_fail_read) {
        return ESP_FAIL;
    }
#ifdef TEST_ATK_BOX
    *data = 0x04;
#elif defined(TEST_TUDOUZI)
    *data = 0x14;
#else
    *data = 0;
#endif
    return ESP_OK;
}

static void expect_success(void) {
    CHECK(g_add_count == 1 && g_remove_count == 1 && !g_device.open);
    CHECK(g_read_count == EXPECTED_READS);
    CHECK(g_write_count == EXPECTED_WRITES);
    for (unsigned int index = 0; index < EXPECTED_WRITES; ++index) {
        CHECK(g_writes[index].reg == s_expected_writes[index].reg);
        CHECK(g_writes[index].value == s_expected_writes[index].value);
    }
}

int main(void) {
    reset_mock();
    CHECK(mybot_generic_board_extra_init(&g_bus) == 0);
    expect_success();

    reset_mock();
    g_fail_add = true;
    CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
    CHECK(g_add_count == 1 && g_remove_count == 0 && g_write_count == 0 && !g_device.open);

    for (unsigned int failure = 1; failure <= EXPECTED_WRITES; ++failure) {
        reset_mock();
        g_fail_write_at = failure;
        CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
        CHECK(g_add_count == 1 && g_remove_count == 1 && !g_device.open);
    }

    if (EXPECTED_READS) {
        reset_mock();
        g_fail_read = true;
        CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
        CHECK(g_add_count == 1 && g_remove_count == 1 && !g_device.open);
    }

    reset_mock();
    g_fail_remove = true;
    CHECK(mybot_generic_board_extra_init(&g_bus) < 0);
    CHECK(g_add_count == 1 && g_remove_count == 1 && g_device.open);

    reset_mock();
    CHECK(mybot_generic_board_extra_init(&g_bus) == 0);
    expect_success();
    puts("Board power and expander sequencing host tests passed");
    return 0;
}

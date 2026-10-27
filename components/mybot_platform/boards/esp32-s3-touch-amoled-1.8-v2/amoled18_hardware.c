/* SPDX-License-Identifier: Apache-2.0 */
#include "amoled18_hardware.h"

#include "board_config.h"
#include "io_expander/tca9554.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "amoled18_hw"
#define I2C_TIMEOUT_MS 100

typedef struct {
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t axp2101;
    mybot_tca9554_t expander;
    bool pa_ready;
    bool pa_enabled;
    bool initialized;
} amoled18_hardware_t;

static amoled18_hardware_t s_hardware;

static esp_err_t write_axp_register(uint8_t reg, uint8_t value) {
    const uint8_t command[] = {reg, value};
    return i2c_master_transmit(s_hardware.axp2101, command, sizeof(command), I2C_TIMEOUT_MS);
}

static esp_err_t initialize_axp2101(void) {
    static const uint8_t registers[][2] = {
        {0x22, 0x06}, {0x27, 0x10}, {0x80, 0x01}, {0x90, 0x00}, {0x91, 0x00}, {0x82, 0x12},
        {0x92, 0x1c}, {0x90, 0x01}, {0x64, 0x02}, {0x61, 0x02}, {0x62, 0x08}, {0x63, 0x01},
    };
    for (size_t index = 0; index < sizeof(registers) / sizeof(registers[0]); ++index) {
        esp_err_t result = write_axp_register(registers[index][0], registers[index][1]);
        if (result != ESP_OK) {
            return result;
        }
    }
    return ESP_OK;
}

static esp_err_t initialize_pa(void) {
    const gpio_config_t config = {
        .pin_bit_mask = 1ULL << MYBOT_AMOLED18_SPEAKER_PA,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t result = gpio_config(&config);
    if (result == ESP_OK) {
        result = gpio_set_level(MYBOT_AMOLED18_SPEAKER_PA, 0);
    }
    if (result == ESP_OK) {
        s_hardware.pa_ready = true;
    }
    return result;
}

static int release_hardware(void) {
    int result = 0;
    if (s_hardware.pa_ready && gpio_set_level(MYBOT_AMOLED18_SPEAKER_PA, 0) != ESP_OK) {
        result = -1;
    }
    if (s_hardware.axp2101 && i2c_master_bus_rm_device(s_hardware.axp2101) != ESP_OK) {
        result = -1;
    }
    if (mybot_tca9554_deinit(&s_hardware.expander) < 0) {
        result = -1;
    }
    if (s_hardware.bus && i2c_del_master_bus(s_hardware.bus) != ESP_OK) {
        result = -1;
    }
    if (result == 0) {
        s_hardware = (amoled18_hardware_t){0};
    } else {
        s_hardware.initialized = false;
        s_hardware.pa_enabled = false;
    }
    return result;
}

int mybot_amoled18_hardware_init(void) {
    if (s_hardware.initialized) {
        return 0;
    }
    if ((s_hardware.bus || s_hardware.axp2101 || s_hardware.expander.device ||
         s_hardware.pa_ready) &&
        release_hardware() < 0) {
        return -1;
    }

    esp_err_t result = initialize_pa();
    const i2c_master_bus_config_t bus_config = {
        .i2c_port = MYBOT_AMOLED18_I2C_PORT,
        .sda_io_num = MYBOT_AMOLED18_I2C_SDA,
        .scl_io_num = MYBOT_AMOLED18_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    if (result == ESP_OK) {
        result = i2c_new_master_bus(&bus_config, &s_hardware.bus);
    }
    if (result == ESP_OK) {
        result =
            mybot_tca9554_init(&s_hardware.expander, s_hardware.bus, MYBOT_AMOLED18_TCA9554_ADDRESS,
                               MYBOT_AMOLED18_I2C_SPEED_HZ) == 0
                ? ESP_OK
                : ESP_FAIL;
    }
    if (result == ESP_OK) {
        result = mybot_tca9554_configure_outputs(&s_hardware.expander, 0x07, 0x00) == 0 ? ESP_OK
                                                                                        : ESP_FAIL;
    }
    if (result == ESP_OK) {
        result =
            mybot_tca9554_set_levels(&s_hardware.expander, 0x07, 0x07) == 0 ? ESP_OK : ESP_FAIL;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    if (result == ESP_OK) {
        result =
            mybot_tca9554_set_levels(&s_hardware.expander, 0x07, 0x00) == 0 ? ESP_OK : ESP_FAIL;
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    if (result == ESP_OK) {
        result =
            mybot_tca9554_set_levels(&s_hardware.expander, 0x07, 0x07) == 0 ? ESP_OK : ESP_FAIL;
    }

    const i2c_device_config_t axp_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MYBOT_AMOLED18_AXP2101_ADDRESS,
        .scl_speed_hz = MYBOT_AMOLED18_I2C_SPEED_HZ,
    };
    if (result == ESP_OK) {
        result = i2c_master_bus_add_device(s_hardware.bus, &axp_config, &s_hardware.axp2101);
    }
    if (result == ESP_OK) {
        result = initialize_axp2101();
    }
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "event=hardware action=initialize result=error code=%s",
                 esp_err_to_name(result));
        (void)release_hardware();
        return -1;
    }

    s_hardware.initialized = true;
    ESP_LOGI(TAG, "event=hardware action=initialize result=ok expander=0x%02x axp2101=0x%02x",
             MYBOT_AMOLED18_TCA9554_ADDRESS, MYBOT_AMOLED18_AXP2101_ADDRESS);
    return 0;
}

int mybot_amoled18_hardware_deinit(void) {
    return release_hardware();
}

i2c_master_bus_handle_t mybot_amoled18_i2c_bus_handle(void) {
    return s_hardware.initialized ? s_hardware.bus : NULL;
}

int mybot_amoled18_set_speaker_power(bool enabled) {
    if (!s_hardware.initialized || !s_hardware.pa_ready) {
        return -1;
    }
    if (s_hardware.pa_enabled == enabled) {
        return 0;
    }
    if (gpio_set_level(MYBOT_AMOLED18_SPEAKER_PA, enabled ? 1 : 0) != ESP_OK) {
        return -1;
    }
    s_hardware.pa_enabled = enabled;
    return 0;
}

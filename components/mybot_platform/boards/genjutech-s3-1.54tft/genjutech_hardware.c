/* SPDX-License-Identifier: Apache-2.0 */
#include "genjutech_hardware.h"

#include "board_config.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

#define TAG "genjutech_hw"
#define BACKLIGHT_LEDC_MODE LEDC_LOW_SPEED_MODE
#define BACKLIGHT_LEDC_TIMER LEDC_TIMER_0
#define BACKLIGHT_LEDC_CHANNEL LEDC_CHANNEL_0
#define BACKLIGHT_LEDC_RESOLUTION LEDC_TIMER_13_BIT
#define BACKLIGHT_LEDC_FREQUENCY_HZ 5000
#define BACKLIGHT_MAX_DUTY ((1U << 13) - 1U)

typedef struct {
    i2c_master_bus_handle_t bus;
    bool pa_ready;
    bool pa_enabled;
    bool backlight_ready;
    bool initialized;
} genjutech_hardware_t;

static genjutech_hardware_t s_hardware;

static esp_err_t initialize_pa(void) {
    const gpio_config_t config = {
        .pin_bit_mask = 1ULL << MYBOT_GENJUTECH_SPEAKER_PA,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t result = gpio_config(&config);
    if (result == ESP_OK) {
        result = gpio_set_level(MYBOT_GENJUTECH_SPEAKER_PA, 0);
    }
    if (result == ESP_OK) {
        s_hardware.pa_ready = true;
        s_hardware.pa_enabled = false;
    }
    return result;
}

static esp_err_t initialize_backlight(void) {
    const ledc_timer_config_t timer_config = {
        .speed_mode = BACKLIGHT_LEDC_MODE,
        .duty_resolution = BACKLIGHT_LEDC_RESOLUTION,
        .timer_num = BACKLIGHT_LEDC_TIMER,
        .freq_hz = BACKLIGHT_LEDC_FREQUENCY_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    esp_err_t result = ledc_timer_config(&timer_config);
    if (result != ESP_OK) {
        return result;
    }
    const ledc_channel_config_t channel_config = {
        .gpio_num = MYBOT_DISPLAY_BACKLIGHT,
        .speed_mode = BACKLIGHT_LEDC_MODE,
        .channel = BACKLIGHT_LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = BACKLIGHT_LEDC_TIMER,
        .duty = 0,
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
    };
    result = ledc_channel_config(&channel_config);
    if (result == ESP_OK) {
        s_hardware.backlight_ready = true;
    }
    return result;
}

static int release_hardware(void) {
    int result = 0;
    if (s_hardware.backlight_ready) {
        if (ledc_stop(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL, 0) == ESP_OK) {
            esp_err_t reset_result = gpio_reset_pin(MYBOT_DISPLAY_BACKLIGHT);
            if (reset_result != ESP_OK) {
                result = -1;
            }
            s_hardware.backlight_ready = false;
        } else {
            result = -1;
        }
    }
    if (s_hardware.pa_ready && gpio_set_level(MYBOT_GENJUTECH_SPEAKER_PA, 0) != ESP_OK) {
        result = -1;
    }
    if (s_hardware.bus) {
        if (i2c_del_master_bus(s_hardware.bus) == ESP_OK) {
            s_hardware.bus = NULL;
        } else {
            result = -1;
        }
    }
    s_hardware.initialized = false;
    s_hardware.pa_enabled = false;
    return result;
}

int mybot_genjutech_hardware_init(void) {
    if (s_hardware.initialized) {
        return 0;
    }
    if ((s_hardware.bus || s_hardware.pa_ready || s_hardware.backlight_ready) &&
        release_hardware() < 0) {
        return -1;
    }

    esp_err_t result = initialize_pa();
    if (result == ESP_OK) {
        result = initialize_backlight();
    }
    const i2c_master_bus_config_t bus_config = {
        .i2c_port = MYBOT_GENJUTECH_I2C_PORT,
        .sda_io_num = MYBOT_GENJUTECH_I2C_SDA,
        .scl_io_num = MYBOT_GENJUTECH_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    if (result == ESP_OK) {
        result = i2c_new_master_bus(&bus_config, &s_hardware.bus);
    }
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "event=hardware action=initialize result=error code=%s",
                 esp_err_to_name(result));
        (void)release_hardware();
        return -1;
    }

    s_hardware.initialized = true;
    ESP_LOGI(TAG, "event=hardware action=initialize result=ok speaker=off backlight=off");
    return 0;
}

int mybot_genjutech_hardware_deinit(void) {
    return release_hardware();
}

i2c_master_bus_handle_t mybot_genjutech_i2c_bus_handle(void) {
    return s_hardware.initialized ? s_hardware.bus : NULL;
}

int mybot_genjutech_set_speaker_power(bool enabled) {
    if (!s_hardware.initialized || !s_hardware.pa_ready) {
        return -1;
    }
    if (s_hardware.pa_enabled == enabled) {
        return 0;
    }
    if (!enabled) {
        /* This board couples PA shutdown to display I/O. Keep the amplifier
         * enabled until hardware teardown and let the I2S stream go idle. */
        return 0;
    }
    esp_err_t result = gpio_set_level(MYBOT_GENJUTECH_SPEAKER_PA, enabled ? 1 : 0);
    if (result != ESP_OK) {
        return -1;
    }
    s_hardware.pa_enabled = enabled;
    return 0;
}

int mybot_board_set_display_backlight(unsigned int percent) {
    if (!s_hardware.initialized || !s_hardware.backlight_ready || percent > 100) {
        return -1;
    }
    uint32_t duty = (BACKLIGHT_MAX_DUTY * percent + 50U) / 100U;
    esp_err_t result = ledc_set_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL, duty);
    if (result == ESP_OK) {
        result = ledc_update_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL);
    }
    return result == ESP_OK ? 0 : -1;
}

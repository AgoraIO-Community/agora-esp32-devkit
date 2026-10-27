/* SPDX-License-Identifier: Apache-2.0 */
#include "lcd185_hardware.h"

#include "board_config.h"
#include "io_expander/tca9554.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "lcd185_hw"
#define BACKLIGHT_LEDC_MODE LEDC_LOW_SPEED_MODE
#define BACKLIGHT_LEDC_TIMER LEDC_TIMER_0
#define BACKLIGHT_LEDC_CHANNEL LEDC_CHANNEL_0
#define BACKLIGHT_LEDC_RESOLUTION LEDC_TIMER_13_BIT
#define BACKLIGHT_LEDC_FREQUENCY_HZ 5000
#define BACKLIGHT_MAX_DUTY ((1U << 13) - 1U)

typedef struct {
    i2c_master_bus_handle_t control_bus;
    i2c_master_bus_handle_t touch_bus;
    mybot_tca9554_t expander;
    bool power_hold_ready;
    bool backlight_ready;
    bool initialized;
} lcd185_hardware_t;

static lcd185_hardware_t s_hardware;

static esp_err_t initialize_power_hold(void) {
    const gpio_config_t config = {
        .pin_bit_mask = 1ULL << MYBOT_LCD185_POWER_CONTROL,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t result = gpio_config(&config);
    if (result == ESP_OK) {
        result = gpio_set_level(MYBOT_LCD185_POWER_CONTROL, 1);
    }
    if (result == ESP_OK) {
        s_hardware.power_hold_ready = true;
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

static esp_err_t initialize_i2c(void) {
    const i2c_master_bus_config_t control_config = {
        .i2c_port = MYBOT_LCD185_I2C_PORT,
        .sda_io_num = MYBOT_LCD185_I2C_SDA,
        .scl_io_num = MYBOT_LCD185_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    esp_err_t result = i2c_new_master_bus(&control_config, &s_hardware.control_bus);
    const i2c_master_bus_config_t touch_config = {
        .i2c_port = MYBOT_TOUCH_I2C_PORT,
        .sda_io_num = MYBOT_TOUCH_I2C_SDA,
        .scl_io_num = MYBOT_TOUCH_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    if (result == ESP_OK) {
        result = i2c_new_master_bus(&touch_config, &s_hardware.touch_bus);
    }
    return result;
}

static esp_err_t pulse_panel_resets(void) {
    esp_err_t result =
        mybot_tca9554_configure_outputs(&s_hardware.expander, 0x03, 0x00) == 0 ? ESP_OK : ESP_FAIL;
    if (result == ESP_OK) {
        result =
            mybot_tca9554_set_levels(&s_hardware.expander, 0x03, 0x03) == 0 ? ESP_OK : ESP_FAIL;
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    if (result == ESP_OK) {
        result =
            mybot_tca9554_set_levels(&s_hardware.expander, 0x03, 0x00) == 0 ? ESP_OK : ESP_FAIL;
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    if (result == ESP_OK) {
        result =
            mybot_tca9554_set_levels(&s_hardware.expander, 0x03, 0x03) == 0 ? ESP_OK : ESP_FAIL;
    }
    return result;
}

static int release_hardware(void) {
    int result = 0;
    if (s_hardware.backlight_ready &&
        ledc_stop(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL, 0) != ESP_OK) {
        result = -1;
    }
    if (s_hardware.backlight_ready && gpio_reset_pin(MYBOT_DISPLAY_BACKLIGHT) != ESP_OK) {
        result = -1;
    }
    if (s_hardware.power_hold_ready && gpio_set_level(MYBOT_LCD185_POWER_CONTROL, 0) != ESP_OK) {
        result = -1;
    }
    if (mybot_tca9554_deinit(&s_hardware.expander) < 0) {
        result = -1;
    }
    if (s_hardware.touch_bus && i2c_del_master_bus(s_hardware.touch_bus) != ESP_OK) {
        result = -1;
    }
    if (s_hardware.control_bus && i2c_del_master_bus(s_hardware.control_bus) != ESP_OK) {
        result = -1;
    }
    if (result == 0) {
        s_hardware = (lcd185_hardware_t){0};
    } else {
        s_hardware.initialized = false;
    }
    return result;
}

int mybot_lcd185_hardware_init(void) {
    if (s_hardware.initialized) {
        return 0;
    }
    if ((s_hardware.control_bus || s_hardware.touch_bus || s_hardware.expander.device ||
         s_hardware.power_hold_ready || s_hardware.backlight_ready) &&
        release_hardware() < 0) {
        return -1;
    }

    esp_err_t result = initialize_power_hold();
    if (result == ESP_OK) {
        result = initialize_backlight();
    }
    if (result == ESP_OK) {
        result = initialize_i2c();
    }
    if (result == ESP_OK) {
        result = mybot_tca9554_init(&s_hardware.expander, s_hardware.control_bus,
                                    MYBOT_LCD185_TCA9554_ADDRESS, 400000) == 0
                     ? ESP_OK
                     : ESP_FAIL;
    }
    if (result == ESP_OK) {
        result = pulse_panel_resets();
    }
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "event=hardware action=initialize result=error code=%s",
                 esp_err_to_name(result));
        (void)release_hardware();
        return -1;
    }

    s_hardware.initialized = true;
    ESP_LOGI(TAG, "event=hardware action=initialize result=ok");
    return 0;
}

int mybot_lcd185_hardware_deinit(void) {
    return release_hardware();
}

i2c_master_bus_handle_t mybot_lcd185_touch_i2c_bus_handle(void) {
    return s_hardware.initialized ? s_hardware.touch_bus : NULL;
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

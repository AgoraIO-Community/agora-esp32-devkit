/* SPDX-License-Identifier: Apache-2.0 */
#include "generic_hardware.h"

#include "board_config.h"
#include "io_expander/aw9523b.h"
#include "io_expander/pca9557.h"
#include "io_expander/tca9554.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "generic_board"
#define BACKLIGHT_LEDC_MODE LEDC_LOW_SPEED_MODE
#define BACKLIGHT_LEDC_TIMER LEDC_TIMER_0
#define BACKLIGHT_LEDC_CHANNEL LEDC_CHANNEL_0
#define BACKLIGHT_LEDC_RESOLUTION LEDC_TIMER_13_BIT
#define BACKLIGHT_LEDC_FREQUENCY_HZ 5000
#define BACKLIGHT_MAX_DUTY ((1U << 13) - 1U)

#if MYBOT_GENERIC_M5STACK_STOPWATCH
#include "m5stack_stopwatch_hardware.h"
#endif

#ifndef MYBOT_GENERIC_AUDIO_KIND
#define MYBOT_GENERIC_AUDIO_KIND 0
#endif

#ifndef MYBOT_GENERIC_TOUCH_KIND
#define MYBOT_GENERIC_TOUCH_KIND 0
#endif

#ifndef MYBOT_GENERIC_PA_ACTIVE_LEVEL
#define MYBOT_GENERIC_PA_ACTIVE_LEVEL 1
#endif

#ifndef MYBOT_GENERIC_PA_GPIO
#define MYBOT_GENERIC_PA_GPIO GPIO_NUM_NC
#endif

#ifndef MYBOT_GENERIC_POWER_ACTIVE_LEVEL
#define MYBOT_GENERIC_POWER_ACTIVE_LEVEL 1
#endif

#ifndef MYBOT_GENERIC_AUX_POWER_GPIO
#define MYBOT_GENERIC_AUX_POWER_GPIO GPIO_NUM_NC
#endif

#ifndef MYBOT_GENERIC_AUX_POWER_ACTIVE_LEVEL
#define MYBOT_GENERIC_AUX_POWER_ACTIVE_LEVEL 1
#endif

typedef struct {
    i2c_master_bus_handle_t audio_bus;
    i2c_master_bus_handle_t touch_bus;
    mybot_pca9557_t expander;
    mybot_tca9554_t tca9554;
    mybot_aw9523b_t aw9523b;
    i2c_master_dev_handle_t axp2101;
    bool expander_ready;
    bool aw9523b_ready;
    bool pa_ready;
    bool pa_enabled;
    bool power_ready;
    bool aux_power_ready;
    bool backlight_ready;
    bool stopwatch_hardware_ready;
    bool initialized;
} hardware_state_t;

static hardware_state_t s_hardware;

#ifdef MYBOT_GENERIC_AXP2101_ADDRESS
static esp_err_t initialize_axp2101(void) {
    static const uint8_t registers[][2] = {
        {0x22, 0x06}, {0x27, 0x10}, {0x80, 0x01}, {0x90, 0x00}, {0x91, 0x00},
        {0x82, 0x12}, {0x92, 0x1c}, {0x93, 0x1c}, {0x90, 0x03}, {0x64, 0x02},
        {0x61, 0x02}, {0x62, 0x0a}, {0x63, 0x01},
    };
    for (size_t i = 0; i < sizeof(registers) / sizeof(registers[0]); ++i) {
        const uint8_t command[] = {registers[i][0], registers[i][1]};
        const esp_err_t result =
            i2c_master_transmit(s_hardware.axp2101, command, sizeof(command), 100);
        if (result != ESP_OK) {
            return result;
        }
    }
    return ESP_OK;
}
#endif

static bool codec_needs_i2c(void) {
    return MYBOT_GENERIC_AUDIO_KIND == 1 || MYBOT_GENERIC_AUDIO_KIND == 3 ||
           MYBOT_GENERIC_AUDIO_KIND == 4;
}

static esp_err_t create_i2c_bus(int port, gpio_num_t sda, gpio_num_t scl,
                                i2c_master_bus_handle_t *out_bus) {
    const i2c_master_bus_config_t config = {
        .i2c_port = port,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    return i2c_new_master_bus(&config, out_bus);
}

static esp_err_t configure_output_gpio(gpio_num_t gpio, int initial_level) {
    if (gpio == GPIO_NUM_NC) {
        return ESP_OK;
    }
    const gpio_config_t config = {
        .pin_bit_mask = 1ULL << gpio,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    const esp_err_t result = gpio_config(&config);
    return result == ESP_OK ? gpio_set_level(gpio, initial_level) : result;
}

static esp_err_t initialize_backlight(void) {
#ifdef MYBOT_GENERIC_AW9523B_BACKLIGHT_MASK
    if (!s_hardware.aw9523b_ready ||
        mybot_aw9523b_set_levels(&s_hardware.aw9523b, MYBOT_GENERIC_AW9523B_BACKLIGHT_MASK, 0) <
            0) {
        return ESP_FAIL;
    }
    s_hardware.backlight_ready = true;
    return ESP_OK;
#elif defined(MYBOT_DISPLAY_BACKLIGHT_GPIO)
    if (MYBOT_DISPLAY_BACKLIGHT_GPIO == GPIO_NUM_NC) {
        return ESP_OK;
    }
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
        .gpio_num = MYBOT_DISPLAY_BACKLIGHT_GPIO,
        .speed_mode = BACKLIGHT_LEDC_MODE,
        .channel = BACKLIGHT_LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = BACKLIGHT_LEDC_TIMER,
        .duty = MYBOT_DISPLAY_BACKLIGHT_INVERT ? BACKLIGHT_MAX_DUTY : 0,
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
    };
    result = ledc_channel_config(&channel_config);
    if (result == ESP_OK) {
        s_hardware.backlight_ready = true;
    }
    return result;
#else
    return ESP_OK;
#endif
}

static int release_hardware(void) {
    int result = 0;
    if (s_hardware.backlight_ready &&
        ledc_stop(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL, 0) != ESP_OK) {
        result = -1;
    }
    s_hardware.backlight_ready = false;
    if (s_hardware.pa_ready) {
        (void)mybot_generic_set_speaker_power(false);
    }
#ifdef MYBOT_GENERIC_POWER_GPIO
    if (s_hardware.power_ready &&
        gpio_set_level(MYBOT_GENERIC_POWER_GPIO, !MYBOT_GENERIC_POWER_ACTIVE_LEVEL) != ESP_OK) {
        result = -1;
    }
#endif
    s_hardware.power_ready = false;
    if (s_hardware.aux_power_ready &&
        gpio_set_level(MYBOT_GENERIC_AUX_POWER_GPIO, !MYBOT_GENERIC_AUX_POWER_ACTIVE_LEVEL) !=
            ESP_OK) {
        result = -1;
    }
    s_hardware.aux_power_ready = false;
#if MYBOT_GENERIC_M5STACK_STOPWATCH
    if (s_hardware.stopwatch_hardware_ready && mybot_m5stack_stopwatch_hardware_deinit() < 0) {
        result = -1;
    }
    s_hardware.stopwatch_hardware_ready = false;
#endif
#ifdef MYBOT_GENERIC_AXP2101_ADDRESS
    if (s_hardware.axp2101) {
        if (i2c_master_bus_rm_device(s_hardware.axp2101) != ESP_OK) {
            result = -1;
        } else {
            s_hardware.axp2101 = NULL;
        }
    }
#endif
#ifdef MYBOT_GENERIC_TCA9554_ADDRESS
    if (s_hardware.tca9554.device && mybot_tca9554_deinit(&s_hardware.tca9554) < 0) {
        result = -1;
    }
#endif
#ifdef MYBOT_GENERIC_AW9523B_ADDRESS
    if (s_hardware.aw9523b.device && mybot_aw9523b_deinit(&s_hardware.aw9523b) < 0) {
        result = -1;
    }
#endif
#ifdef MYBOT_GENERIC_PCA9557_ADDRESS
    if (s_hardware.expander_ready && mybot_pca9557_deinit(&s_hardware.expander) < 0) {
        result = -1;
    }
#endif
    s_hardware.expander_ready = false;
    s_hardware.aw9523b_ready = false;
    if (s_hardware.touch_bus && i2c_del_master_bus(s_hardware.touch_bus) != ESP_OK) {
        result = -1;
    } else {
        s_hardware.touch_bus = NULL;
    }
    if (s_hardware.audio_bus && i2c_del_master_bus(s_hardware.audio_bus) != ESP_OK) {
        result = -1;
    } else {
        s_hardware.audio_bus = NULL;
    }
    s_hardware.initialized = false;
    s_hardware.pa_ready = false;
    s_hardware.pa_enabled = false;
    return result;
}

int mybot_generic_hardware_init(void) {
    if (s_hardware.initialized) {
        return 0;
    }
    if ((s_hardware.audio_bus || s_hardware.touch_bus || s_hardware.expander_ready ||
         s_hardware.tca9554.device || s_hardware.aw9523b.device || s_hardware.axp2101 ||
         s_hardware.pa_ready || s_hardware.power_ready || s_hardware.aux_power_ready ||
         s_hardware.backlight_ready) &&
        release_hardware() < 0) {
        return -1;
    }

#ifdef MYBOT_GENERIC_POWER_GPIO
    esp_err_t result =
        configure_output_gpio(MYBOT_GENERIC_POWER_GPIO, MYBOT_GENERIC_POWER_ACTIVE_LEVEL);
    if (result != ESP_OK) {
        return -1;
    }
    s_hardware.power_ready = MYBOT_GENERIC_POWER_GPIO != GPIO_NUM_NC;
#else
    esp_err_t result = ESP_OK;
#endif
    result =
        configure_output_gpio(MYBOT_GENERIC_AUX_POWER_GPIO, MYBOT_GENERIC_AUX_POWER_ACTIVE_LEVEL);
    if (result != ESP_OK) {
        return -1;
    }
    s_hardware.aux_power_ready = MYBOT_GENERIC_AUX_POWER_GPIO != GPIO_NUM_NC;

    const bool touch_separate_bus =
#ifdef MYBOT_GENERIC_TOUCH_I2C_PORT
        MYBOT_GENERIC_TOUCH_I2C_PORT != MYBOT_GENERIC_I2C_PORT;
#else
        false;
#endif
    if ((codec_needs_i2c() || (MYBOT_GENERIC_TOUCH_KIND != 0 && !touch_separate_bus)) &&
        !s_hardware.audio_bus) {
        result = create_i2c_bus(MYBOT_GENERIC_I2C_PORT, MYBOT_GENERIC_I2C_SDA,
                                MYBOT_GENERIC_I2C_SCL, &s_hardware.audio_bus);
    }
#if MYBOT_GENERIC_M5STACK_STOPWATCH
    if (result == ESP_OK && s_hardware.audio_bus &&
        mybot_m5stack_stopwatch_hardware_init(s_hardware.audio_bus) == 0) {
        s_hardware.stopwatch_hardware_ready = true;
    } else if (result == ESP_OK) {
        result = ESP_FAIL;
    }
#endif
#ifdef MYBOT_GENERIC_TOUCH_I2C_PORT
    if (MYBOT_GENERIC_TOUCH_KIND != 0 && touch_separate_bus && result == ESP_OK) {
        result = create_i2c_bus(MYBOT_GENERIC_TOUCH_I2C_PORT, MYBOT_GENERIC_TOUCH_I2C_SDA,
                                MYBOT_GENERIC_TOUCH_I2C_SCL, &s_hardware.touch_bus);
    }
#endif

#ifdef MYBOT_GENERIC_PCA9557_ADDRESS
    if (result == ESP_OK && s_hardware.audio_bus) {
        result = mybot_pca9557_init(&s_hardware.expander, s_hardware.audio_bus,
                                    MYBOT_GENERIC_PCA9557_ADDRESS, MYBOT_GENERIC_I2C_SPEED_HZ) == 0
                     ? ESP_OK
                     : ESP_FAIL;
        s_hardware.expander_ready = result == ESP_OK;
    }
    if (result == ESP_OK && s_hardware.expander_ready) {
        result =
            mybot_pca9557_configure_outputs(&s_hardware.expander, MYBOT_GENERIC_PCA9557_OUTPUT_MASK,
                                            MYBOT_GENERIC_PCA9557_HIGH_MASK) == 0
                ? ESP_OK
                : ESP_FAIL;
    }
#ifdef MYBOT_GENERIC_PCA9557_LOW_MASK
    if (result == ESP_OK && s_hardware.expander_ready) {
        result =
            mybot_pca9557_set_levels(&s_hardware.expander, MYBOT_GENERIC_PCA9557_LOW_MASK, 0) == 0
                ? ESP_OK
                : ESP_FAIL;
    }
#endif
#endif

#ifdef MYBOT_GENERIC_TCA9554_ADDRESS
    if (result == ESP_OK && s_hardware.audio_bus) {
        result = mybot_tca9554_init(&s_hardware.tca9554, s_hardware.audio_bus,
                                    MYBOT_GENERIC_TCA9554_ADDRESS, MYBOT_GENERIC_I2C_SPEED_HZ) == 0
                     ? ESP_OK
                     : ESP_FAIL;
    }
    if (result == ESP_OK) {
        result = mybot_tca9554_configure_outputs(&s_hardware.tca9554,
                                                 MYBOT_GENERIC_TCA9554_OUTPUT_MASK, 0) == 0
                     ? ESP_OK
                     : ESP_FAIL;
    }
    if (result == ESP_OK) {
        result = mybot_tca9554_set_levels(&s_hardware.tca9554, MYBOT_GENERIC_TCA9554_RESET_MASK,
                                          MYBOT_GENERIC_TCA9554_RESET_MASK) == 0
                     ? ESP_OK
                     : ESP_FAIL;
        vTaskDelay(pdMS_TO_TICKS(120));
    }
    if (result == ESP_OK) {
        result =
            mybot_tca9554_set_levels(&s_hardware.tca9554, MYBOT_GENERIC_TCA9554_RESET_MASK, 0) == 0
                ? ESP_OK
                : ESP_FAIL;
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    if (result == ESP_OK) {
        result = mybot_tca9554_set_levels(&s_hardware.tca9554, MYBOT_GENERIC_TCA9554_RESET_MASK,
                                          MYBOT_GENERIC_TCA9554_RESET_MASK) == 0
                     ? ESP_OK
                     : ESP_FAIL;
    }
#ifdef MYBOT_GENERIC_TCA9554_LOW_MASK
    if (result == ESP_OK) {
        result =
            mybot_tca9554_set_levels(&s_hardware.tca9554, MYBOT_GENERIC_TCA9554_LOW_MASK, 0) == 0
                ? ESP_OK
                : ESP_FAIL;
    }
#endif
#endif

#ifdef MYBOT_GENERIC_AW9523B_ADDRESS
    if (result == ESP_OK && s_hardware.audio_bus) {
        result = mybot_aw9523b_init(&s_hardware.aw9523b, s_hardware.audio_bus,
                                    MYBOT_GENERIC_AW9523B_ADDRESS, MYBOT_GENERIC_I2C_SPEED_HZ) == 0
                     ? ESP_OK
                     : ESP_FAIL;
        s_hardware.aw9523b_ready = result == ESP_OK;
    }
    if (result == ESP_OK && s_hardware.aw9523b_ready) {
        result =
            mybot_aw9523b_configure_outputs(&s_hardware.aw9523b, MYBOT_GENERIC_AW9523B_OUTPUT_MASK,
                                            MYBOT_GENERIC_AW9523B_HIGH_MASK) == 0
                ? ESP_OK
                : ESP_FAIL;
    }
    if (result == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(MYBOT_GENERIC_AW9523B_SETTLE_MS));
    }
#endif

#ifdef MYBOT_GENERIC_AXP2101_ADDRESS
    if (result == ESP_OK && s_hardware.audio_bus) {
        const i2c_device_config_t axp_config = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = MYBOT_GENERIC_AXP2101_ADDRESS,
            .scl_speed_hz = MYBOT_GENERIC_I2C_SPEED_HZ,
        };
        result = i2c_master_bus_add_device(s_hardware.audio_bus, &axp_config, &s_hardware.axp2101);
    }
    if (result == ESP_OK) {
        result = initialize_axp2101();
    }
#endif

    if (result == ESP_OK) {
        result = configure_output_gpio(MYBOT_GENERIC_PA_GPIO, !MYBOT_GENERIC_PA_ACTIVE_LEVEL);
        s_hardware.pa_ready = MYBOT_GENERIC_PA_GPIO != GPIO_NUM_NC;
    }
    if (result == ESP_OK) {
        result = initialize_backlight();
    }
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "event=hardware action=initialize result=error code=%s",
                 esp_err_to_name(result));
        (void)release_hardware();
        return -1;
    }

    s_hardware.initialized = true;
    ESP_LOGI(TAG, "event=hardware action=initialize board=%s result=ok", MYBOT_BOARD_NAME);
    return 0;
}

int mybot_generic_hardware_deinit(void) {
    return release_hardware();
}

i2c_master_bus_handle_t mybot_generic_audio_i2c_bus(void) {
    return s_hardware.initialized ? s_hardware.audio_bus : NULL;
}

i2c_master_bus_handle_t mybot_generic_touch_i2c_bus(void) {
    if (!s_hardware.initialized) {
        return NULL;
    }
    return s_hardware.touch_bus ? s_hardware.touch_bus : s_hardware.audio_bus;
}

int mybot_generic_set_speaker_power(bool enabled) {
    if (!s_hardware.initialized) {
        return -1;
    }
    if (s_hardware.pa_enabled == enabled) {
        return 0;
    }
#ifdef MYBOT_GENERIC_PCA9557_PA_MASK
    if (s_hardware.expander_ready) {
        const uint8_t mask = MYBOT_GENERIC_PCA9557_PA_MASK;
        if (mybot_pca9557_set_levels(&s_hardware.expander, mask, enabled ? mask : 0) < 0) {
            return -1;
        }
        s_hardware.pa_enabled = enabled;
        return 0;
    }
#endif
#ifdef MYBOT_GENERIC_AW9523B_PA_MASK
    if (s_hardware.aw9523b_ready) {
        const uint16_t mask = MYBOT_GENERIC_AW9523B_PA_MASK;
        if (mybot_aw9523b_set_levels(&s_hardware.aw9523b, mask, enabled ? mask : 0) < 0) {
            return -1;
        }
        s_hardware.pa_enabled = enabled;
        return 0;
    }
#endif
#if MYBOT_GENERIC_M5STACK_STOPWATCH
    if (s_hardware.stopwatch_hardware_ready) {
        if (mybot_m5stack_stopwatch_set_speaker_pa(enabled) < 0) {
            return -1;
        }
        s_hardware.pa_enabled = enabled;
        return 0;
    }
#endif
    if (!s_hardware.pa_ready) {
        s_hardware.pa_enabled = enabled;
        return 0;
    }
    const int level = enabled ? MYBOT_GENERIC_PA_ACTIVE_LEVEL : !MYBOT_GENERIC_PA_ACTIVE_LEVEL;
    if (gpio_set_level(MYBOT_GENERIC_PA_GPIO, level) != ESP_OK) {
        return -1;
    }
    s_hardware.pa_enabled = enabled;
    return 0;
}

int mybot_board_set_display_backlight(unsigned int percent) {
#ifdef MYBOT_GENERIC_AW9523B_BACKLIGHT_MASK
    if (!s_hardware.initialized || !s_hardware.backlight_ready || percent > 100) {
        return -1;
    }
    const uint16_t mask = MYBOT_GENERIC_AW9523B_BACKLIGHT_MASK;
    return mybot_aw9523b_set_levels(&s_hardware.aw9523b, mask, percent ? mask : 0);
#elif defined(MYBOT_DISPLAY_BACKLIGHT_GPIO)
    if (MYBOT_DISPLAY_BACKLIGHT_GPIO == GPIO_NUM_NC) {
        return 0;
    }
    if (!s_hardware.initialized || !s_hardware.backlight_ready || percent > 100) {
        return -1;
    }
    uint32_t duty = (BACKLIGHT_MAX_DUTY * percent + 50U) / 100U;
    if (MYBOT_DISPLAY_BACKLIGHT_INVERT) {
        duty = BACKLIGHT_MAX_DUTY - duty;
    }
    if (ledc_set_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL, duty) != ESP_OK ||
        ledc_update_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL) != ESP_OK) {
        return -1;
    }
    return 0;
#else
    (void)percent;
    return 0;
#endif
}

i2c_master_bus_handle_t mybot_es8311_board_i2c_bus_handle(void) {
    return mybot_generic_audio_i2c_bus();
}

int mybot_es8311_board_set_speaker_power(bool enabled) {
    return mybot_generic_set_speaker_power(enabled);
}

i2c_master_bus_handle_t mybot_amoled175_i2c_bus_handle(void) {
    return mybot_generic_audio_i2c_bus();
}

int mybot_amoled175_set_speaker_pa(bool enabled) {
    return mybot_generic_set_speaker_power(enabled);
}

i2c_master_bus_handle_t mybot_board_touch_i2c_bus_handle(void) {
    return mybot_generic_touch_i2c_bus();
}

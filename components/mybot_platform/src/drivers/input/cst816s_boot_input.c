/* SPDX-License-Identifier: Apache-2.0 */
#include "board_config.h"

#include <mybot/mybot.h>
#include <mybot/platform/mybot_key.h>

#include "button_gpio.h"

#ifndef MYBOT_TOUCH_DRIVER_KIND
#define MYBOT_TOUCH_DRIVER_KIND 1
#endif

#if MYBOT_TOUCH_DRIVER_KIND == 4
#include "driver/gpio.h"
#else
#include "esp_lcd_touch.h"
#endif

#if MYBOT_TOUCH_DRIVER_KIND == 1
#include "esp_lcd_touch_cst816s.h"
#elif MYBOT_TOUCH_DRIVER_KIND == 2
#include "esp_lcd_touch_ft5x06.h"
#elif MYBOT_TOUCH_DRIVER_KIND == 3
#include "esp_lcd_touch_gt911.h"
#elif MYBOT_TOUCH_DRIVER_KIND != 4
#error "Unsupported MYBOT_TOUCH_DRIVER_KIND"
#endif

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "iot_button.h"
#include "platform/board_actions.h"

#include <stdbool.h>

#define TAG "cst816s_input"
#define INPUT_STOP_TIMEOUT_MS 1000

#ifndef MYBOT_BOOT_BUTTON_LONG_PRESS_MS
#define MYBOT_BOOT_BUTTON_LONG_PRESS_MS 3000
#endif

#ifndef MYBOT_TOUCH_X_MAX
#define MYBOT_TOUCH_X_MAX (MYBOT_DISPLAY_WIDTH - 1)
#endif

#ifndef MYBOT_TOUCH_Y_MAX
#define MYBOT_TOUCH_Y_MAX (MYBOT_DISPLAY_HEIGHT - 1)
#endif

typedef struct {
    portMUX_TYPE lock;
    bool active;
    bool started;
    bool task_stop_requested;
    unsigned callbacks_in_flight;
    mybot_key_event_handler_t emit;
    void *user_data;
    button_handle_t boot;
#if MYBOT_TOUCH_DRIVER_KIND == 4
    i2c_master_dev_handle_t raw_touch;
#else
    esp_lcd_touch_handle_t touch;
    esp_lcd_panel_io_handle_t touch_io;
#endif
    TaskHandle_t touch_task;
} input_context_t;

static input_context_t s_context = {
    .lock = portMUX_INITIALIZER_UNLOCKED,
};
static StaticSemaphore_t s_task_stopped_storage;
static SemaphoreHandle_t s_task_stopped;

i2c_master_bus_handle_t mybot_board_touch_i2c_bus_handle(void);

#if MYBOT_TOUCH_DRIVER_KIND == 4
#define CST226_ADDRESS 0x5A
#define CST226_I2C_TIMEOUT_MS 100

static int read_cst226_touch(bool *down) {
    const uint8_t command = 0x00;
    uint8_t data[6] = {0};
    esp_err_t result =
        i2c_master_transmit(s_context.raw_touch, &command, sizeof(command), CST226_I2C_TIMEOUT_MS);
    if (result == ESP_OK) {
        result = i2c_master_receive(s_context.raw_touch, data, sizeof(data), CST226_I2C_TIMEOUT_MS);
    }
    if (result != ESP_OK) {
        return -1;
    }
    const int x = ((int)data[1] << 4) | (data[3] >> 4);
    const int y = ((int)data[2] << 4) | (data[3] & 0x0F);
    *down = (data[5] & 0x0F) != 0 && x >= 0 && x < MYBOT_DISPLAY_WIDTH && y >= 0 &&
            y < MYBOT_DISPLAY_HEIGHT;
    return 0;
}
#endif

static bool adapter_is_active(void) {
    portENTER_CRITICAL(&s_context.lock);
    const bool active = s_context.active;
    portEXIT_CRITICAL(&s_context.lock);
    return active;
}

static bool task_should_stop(void) {
    portENTER_CRITICAL(&s_context.lock);
    const bool stop = s_context.task_stop_requested;
    portEXIT_CRITICAL(&s_context.lock);
    return stop;
}

static void emit_event(mybot_key_event_t event) {
    mybot_key_event_handler_t emit = NULL;
    void *user_data = NULL;

    portENTER_CRITICAL(&s_context.lock);
    if (s_context.active && s_context.emit) {
        ++s_context.callbacks_in_flight;
        emit = s_context.emit;
        user_data = s_context.user_data;
    }
    portEXIT_CRITICAL(&s_context.lock);

    if (!emit) {
        return;
    }
    emit(event, user_data);

    portENTER_CRITICAL(&s_context.lock);
    --s_context.callbacks_in_flight;
    portEXIT_CRITICAL(&s_context.lock);
}

static void handle_short_press(void) {
    if (!adapter_is_active()) {
        return;
    }
    const mybot_state_t state = mybot_get_state();
    if (state == MYBOT_STATE_READY) {
        emit_event(MYBOT_KEY_EVENT_CONVERSATION_START);
    } else if (state == MYBOT_STATE_IN_CONVERSATION) {
        emit_event(MYBOT_KEY_EVENT_CONVERSATION_STOP);
    }
}

static void handle_long_press(const char *source) {
    (void)source;
    ESP_LOGI(TAG, "event=input source=%s action=wifi_provisioning_request", source);
    if (mybot_board_handle_boot_long_press() < 0) {
        ESP_LOGE(TAG, "event=provision_request source=%s result=error", source);
    }
}

static void boot_click(void *button, void *user_data) {
    (void)button;
    (void)user_data;
    handle_short_press();
}

static void boot_long_press(void *button, void *user_data) {
    (void)button;
    (void)user_data;
    handle_long_press("boot_button");
}

static void touch_task(void *argument) {
    (void)argument;
    bool pressed = false;
    bool long_fired = false;
    TickType_t pressed_at = 0;

    while (!task_should_stop()) {
#if MYBOT_TOUCH_DRIVER_KIND == 4
        bool down = false;
        (void)read_cst226_touch(&down);
#else
        esp_lcd_touch_point_data_t point = {0};
        uint8_t count = 0;
        const bool valid = esp_lcd_touch_read_data(s_context.touch) == ESP_OK &&
                           esp_lcd_touch_get_data(s_context.touch, &point, &count, 1) == ESP_OK;
        const bool down =
            valid && count > 0 && point.x <= MYBOT_DISPLAY_WIDTH && point.y <= MYBOT_DISPLAY_HEIGHT;
#endif
        const TickType_t now = xTaskGetTickCount();

        if (down && !pressed) {
            pressed = true;
            long_fired = false;
            pressed_at = now;
        } else if (down && pressed && !long_fired &&
                   now - pressed_at >= pdMS_TO_TICKS(MYBOT_TOUCH_LONG_PRESS_MS)) {
            long_fired = true;
            handle_long_press("touch");
        } else if (!down && pressed) {
            pressed = false;
            const TickType_t duration = now - pressed_at;
            if (!long_fired && duration >= pdMS_TO_TICKS(MYBOT_TOUCH_SHORT_PRESS_MIN_MS)) {
                handle_short_press();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(MYBOT_TOUCH_POLL_INTERVAL_MS));
    }

    /* The owner keeps the handle until this task is suspended and can release
     * its PSRAM stack without allocating an internal cleanup task. */
    xSemaphoreGive(s_task_stopped);
    vTaskSuspend(NULL);
}

static int create_touch(void) {
    i2c_master_bus_handle_t bus = mybot_board_touch_i2c_bus_handle();
    if (!bus) {
        return -1;
    }

#if MYBOT_TOUCH_DRIVER_KIND == 4
    if (MYBOT_TOUCH_RESET != GPIO_NUM_NC) {
        const gpio_config_t reset_config = {
            .pin_bit_mask = 1ULL << MYBOT_TOUCH_RESET,
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        esp_err_t result = gpio_config(&reset_config);
        if (result == ESP_OK) {
            result = gpio_set_level(MYBOT_TOUCH_RESET, MYBOT_TOUCH_RESET_LEVEL);
        }
        if (result != ESP_OK) {
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
        if (gpio_set_level(MYBOT_TOUCH_RESET, !MYBOT_TOUCH_RESET_LEVEL) != ESP_OK) {
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    const i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CST226_ADDRESS,
        .scl_speed_hz = MYBOT_TOUCH_I2C_SPEED_HZ,
    };
    return i2c_master_bus_add_device(bus, &device_config, &s_context.raw_touch) == ESP_OK ? 0 : -1;
#else
#if MYBOT_TOUCH_DRIVER_KIND == 1
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = ESP_LCD_TOUCH_IO_I2C_CST816S_ADDRESS,
        .scl_speed_hz = MYBOT_TOUCH_I2C_SPEED_HZ,
        .control_phase_bytes = 1,
        .dc_bit_offset = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 0,
        .flags.disable_control_phase = 1,
    };
#elif MYBOT_TOUCH_DRIVER_KIND == 2
    esp_lcd_panel_io_i2c_config_t io_config = ESP_LCD_TOUCH_IO_I2C_FT5x06_CONFIG();
    io_config.scl_speed_hz = MYBOT_TOUCH_I2C_SPEED_HZ;
#elif MYBOT_TOUCH_DRIVER_KIND == 3
    esp_lcd_panel_io_i2c_config_t io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
    io_config.scl_speed_hz = MYBOT_TOUCH_I2C_SPEED_HZ;
#endif
    esp_err_t result = esp_lcd_new_panel_io_i2c(bus, &io_config, &s_context.touch_io);
    if (result != ESP_OK) {
        return -1;
    }

    const esp_lcd_touch_config_t touch_config = {
        .x_max = MYBOT_TOUCH_X_MAX,
        .y_max = MYBOT_TOUCH_Y_MAX,
        .rst_gpio_num = MYBOT_TOUCH_RESET,
        .int_gpio_num = MYBOT_TOUCH_INTERRUPT,
        .levels =
            {
                .reset = MYBOT_TOUCH_RESET_LEVEL,
                .interrupt = MYBOT_TOUCH_INTERRUPT_LEVEL,
            },
        .flags =
            {
                .swap_xy = MYBOT_TOUCH_SWAP_XY,
                .mirror_x = MYBOT_TOUCH_MIRROR_X,
                .mirror_y = MYBOT_TOUCH_MIRROR_Y,
            },
    };
#if MYBOT_TOUCH_DRIVER_KIND == 1
    result = esp_lcd_touch_new_i2c_cst816s(s_context.touch_io, &touch_config, &s_context.touch);
#elif MYBOT_TOUCH_DRIVER_KIND == 2
    result = esp_lcd_touch_new_i2c_ft5x06(s_context.touch_io, &touch_config, &s_context.touch);
#elif MYBOT_TOUCH_DRIVER_KIND == 3
    result = esp_lcd_touch_new_i2c_gt911(s_context.touch_io, &touch_config, &s_context.touch);
#endif
    if (result != ESP_OK) {
        (void)esp_lcd_panel_io_del(s_context.touch_io);
        s_context.touch_io = NULL;
        return -1;
    }
    return 0;
#endif
}

static int join_input_task(TaskHandle_t task) {
    if (!task) {
        return 0;
    }
    if (!s_task_stopped ||
        xSemaphoreTake(s_task_stopped, pdMS_TO_TICKS(INPUT_STOP_TIMEOUT_MS)) != pdTRUE) {
        return -1;
    }
    /* Keep completion latched if suspension has not happened before the timeout. */
    xSemaphoreGive(s_task_stopped);
    const TickType_t started = xTaskGetTickCount();
    while (eTaskGetState(task) != eSuspended) {
        if (xTaskGetTickCount() - started >= pdMS_TO_TICKS(INPUT_STOP_TIMEOUT_MS)) {
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    vTaskDeleteWithCaps(task);
    portENTER_CRITICAL(&s_context.lock);
    s_context.touch_task = NULL;
    portEXIT_CRITICAL(&s_context.lock);
    return 0;
}

static int release_input_devices(void) {
    int result = 0;
    if (s_context.boot && iot_button_delete(s_context.boot) == ESP_OK) {
        s_context.boot = NULL;
    } else if (s_context.boot) {
        result = -1;
    }
#if MYBOT_TOUCH_DRIVER_KIND == 4
    if (s_context.raw_touch && i2c_master_bus_rm_device(s_context.raw_touch) == ESP_OK) {
        s_context.raw_touch = NULL;
    } else if (s_context.raw_touch) {
        result = -1;
    }
#else
    if (s_context.touch && esp_lcd_touch_del(s_context.touch) == ESP_OK) {
        s_context.touch = NULL;
    } else if (s_context.touch) {
        result = -1;
    }
    if (!s_context.touch && s_context.touch_io &&
        esp_lcd_panel_io_del(s_context.touch_io) == ESP_OK) {
        s_context.touch_io = NULL;
    } else if (!s_context.touch && s_context.touch_io) {
        result = -1;
    }
#endif
    return result;
}

static int release_input(void) {
    portENTER_CRITICAL(&s_context.lock);
    s_context.active = false;
    s_context.task_stop_requested = true;
    TaskHandle_t task = s_context.touch_task;
    portEXIT_CRITICAL(&s_context.lock);

    if (join_input_task(task) < 0) {
        ESP_LOGE(TAG, "event=input action=deinitialize result=error reason=task_join");
        return -1;
    }

    const TickType_t callbacks_started = xTaskGetTickCount();
    unsigned in_flight = 0;
    do {
        portENTER_CRITICAL(&s_context.lock);
        in_flight = s_context.callbacks_in_flight;
        portEXIT_CRITICAL(&s_context.lock);
        if (in_flight) {
            if (xTaskGetTickCount() - callbacks_started >= pdMS_TO_TICKS(INPUT_STOP_TIMEOUT_MS)) {
                return -1;
            }
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    } while (in_flight);

    const int result = release_input_devices();
    portENTER_CRITICAL(&s_context.lock);
    s_context.emit = NULL;
    s_context.user_data = NULL;
    s_context.started = s_context.boot ||
#if MYBOT_TOUCH_DRIVER_KIND == 4
                        s_context.raw_touch ||
#else
                        s_context.touch || s_context.touch_io ||
#endif
                        s_context.touch_task;
    if (!s_context.started) {
        s_context.task_stop_requested = false;
    }
    portEXIT_CRITICAL(&s_context.lock);
    return result;
}

int mybot_cst816s_input_start(void) {
    portENTER_CRITICAL(&s_context.lock);
    const bool already_started = s_context.started || s_context.boot ||
#if MYBOT_TOUCH_DRIVER_KIND == 4
                                 s_context.raw_touch ||
#else
                                 s_context.touch || s_context.touch_io ||
#endif
                                 s_context.touch_task;
    portEXIT_CRITICAL(&s_context.lock);
    if (already_started) {
        return -1;
    }

    if (!s_task_stopped) {
        s_task_stopped = xSemaphoreCreateBinaryStatic(&s_task_stopped_storage);
    }
    if (!s_task_stopped) {
        return -1;
    }
    (void)xSemaphoreTake(s_task_stopped, 0);

    const button_config_t button_config = {
        .long_press_time = MYBOT_BOOT_BUTTON_LONG_PRESS_MS,
        .short_press_time = 50,
    };
    const button_gpio_config_t gpio_config = {
        .gpio_num = MYBOT_BOOT_BUTTON_GPIO,
        .active_level = 0,
        .enable_power_save = false,
        .disable_pull = false,
    };
    esp_err_t result = iot_button_new_gpio_device(&button_config, &gpio_config, &s_context.boot);
    if (result == ESP_OK) {
        result =
            iot_button_register_cb(s_context.boot, BUTTON_SINGLE_CLICK, NULL, boot_click, NULL);
    }
    if (result == ESP_OK) {
        result = iot_button_register_cb(s_context.boot, BUTTON_LONG_PRESS_START, NULL,
                                        boot_long_press, NULL);
    }
    if (result == ESP_OK && create_touch() < 0) {
        result = ESP_FAIL;
    }
    if (result == ESP_OK) {
        portENTER_CRITICAL(&s_context.lock);
        s_context.task_stop_requested = false;
        portEXIT_CRITICAL(&s_context.lock);
        if (xTaskCreateWithCaps(touch_task, "mybot_touch", 4096, NULL, 5, &s_context.touch_task,
                                MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT) != pdPASS) {
            result = ESP_ERR_NO_MEM;
        }
    }
    if (result != ESP_OK) {
        portENTER_CRITICAL(&s_context.lock);
        s_context.task_stop_requested = true;
        portEXIT_CRITICAL(&s_context.lock);
        if (release_input() < 0) {
            ESP_LOGE(TAG, "event=input action=initialize cleanup=error");
        }
        return -1;
    }

    portENTER_CRITICAL(&s_context.lock);
    s_context.started = true;
    portEXIT_CRITICAL(&s_context.lock);
    return 0;
}

int mybot_cst816s_input_stop(void) {
    return release_input();
}

static int input_init(void **out_context, mybot_key_event_handler_t emit, void *user_data) {
    if (!out_context || !emit) {
        return -1;
    }
    *out_context = NULL;

    portENTER_CRITICAL(&s_context.lock);
    if (s_context.active || !s_context.started) {
        portEXIT_CRITICAL(&s_context.lock);
        return -1;
    }
    s_context.emit = emit;
    s_context.user_data = user_data;
    s_context.active = true;
    portEXIT_CRITICAL(&s_context.lock);
    *out_context = &s_context;
    return 0;
}

static void input_destroy(void *opaque) {
    if (opaque != &s_context) {
        return;
    }

    portENTER_CRITICAL(&s_context.lock);
    s_context.active = false;
    unsigned in_flight = s_context.callbacks_in_flight;
    portEXIT_CRITICAL(&s_context.lock);
    while (in_flight) {
        vTaskDelay(pdMS_TO_TICKS(1));
        portENTER_CRITICAL(&s_context.lock);
        in_flight = s_context.callbacks_in_flight;
        portEXIT_CRITICAL(&s_context.lock);
    }

    portENTER_CRITICAL(&s_context.lock);
    s_context.emit = NULL;
    s_context.user_data = NULL;
    portEXIT_CRITICAL(&s_context.lock);
}

static const mybot_key_ops_t s_ops = {
    .init = input_init,
    .destroy = input_destroy,
};

const mybot_key_ops_t *mybot_cst816s_input_ops(void) {
    return &s_ops;
}

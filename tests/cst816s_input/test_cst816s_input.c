/* SPDX-License-Identifier: MIT */
#include "mock_platform.h"

#include "board_config.h"

#include <errno.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
struct mock_panel_io {
    bool open;
};
struct mock_touch {
    bool open;
};
struct mock_button {
    bool open;
    button_callback_t click;
    button_callback_t long_press;
    void *click_user_data;
    void *long_press_user_data;
};
struct mock_task {
    pthread_t thread;
    task_function_t function;
    void *argument;
};

static struct mock_i2c_bus g_bus;
static struct mock_panel_io g_io;
static struct mock_touch g_touch;
static struct mock_button g_button;
static bool g_fail_button_new;
static button_event_t g_fail_register_event;
static bool g_fail_button_delete;
static bool g_fail_io_new;
static bool g_fail_io_delete;
static bool g_fail_touch_new;
static bool g_fail_touch_delete;
static bool g_fail_task_create;
static unsigned int g_button_delete_calls;
static unsigned int g_provision_calls;
static unsigned int g_state_calls;
static mybot_state_t g_state = MYBOT_STATE_STARTING;
static _Thread_local struct mock_task *g_current_task;

const char *esp_err_to_name(esp_err_t error) {
    return error == ESP_OK ? "ESP_OK" : "ESP_FAIL";
}

i2c_master_bus_handle_t mybot_board_touch_i2c_bus_handle(void) {
    return &g_bus;
}

esp_err_t esp_lcd_new_panel_io_i2c(i2c_master_bus_handle_t bus,
                                   const esp_lcd_panel_io_i2c_config_t *config,
                                   esp_lcd_panel_io_handle_t *out_io) {
    CHECK(bus == &g_bus && !g_io.open);
    CHECK(config->dev_addr == ESP_LCD_TOUCH_IO_I2C_CST816S_ADDRESS);
    CHECK(config->scl_speed_hz == MYBOT_TOUCH_I2C_SPEED_HZ);
    CHECK(config->control_phase_bytes == 1 && config->dc_bit_offset == 0);
    CHECK(config->lcd_cmd_bits == 8 && config->lcd_param_bits == 0);
    CHECK(config->flags.disable_control_phase == 1);
    if (g_fail_io_new) {
        g_fail_io_new = false;
        return ESP_FAIL;
    }
    g_io.open = true;
    *out_io = &g_io;
    return ESP_OK;
}

esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io) {
    CHECK(io == &g_io && g_io.open);
    if (g_fail_io_delete) {
        g_fail_io_delete = false;
        return ESP_FAIL;
    }
    g_io.open = false;
    return ESP_OK;
}

esp_err_t esp_lcd_touch_new_i2c_cst816s(esp_lcd_panel_io_handle_t io,
                                        const esp_lcd_touch_config_t *config,
                                        esp_lcd_touch_handle_t *out_touch) {
    CHECK(io == &g_io && g_io.open && !g_touch.open);
    CHECK(config->x_max == MYBOT_DISPLAY_WIDTH - 1);
    CHECK(config->y_max == MYBOT_DISPLAY_HEIGHT - 1);
    CHECK(config->rst_gpio_num == MYBOT_TOUCH_RESET);
    CHECK(config->int_gpio_num == MYBOT_TOUCH_INTERRUPT);
    CHECK(config->levels.reset == MYBOT_TOUCH_RESET_LEVEL);
    CHECK(config->levels.interrupt == MYBOT_TOUCH_INTERRUPT_LEVEL);
    CHECK(config->flags.swap_xy == MYBOT_TOUCH_SWAP_XY);
    CHECK(config->flags.mirror_x == MYBOT_TOUCH_MIRROR_X);
    CHECK(config->flags.mirror_y == MYBOT_TOUCH_MIRROR_Y);
    if (g_fail_touch_new) {
        g_fail_touch_new = false;
        return ESP_FAIL;
    }
    g_touch.open = true;
    *out_touch = &g_touch;
    return ESP_OK;
}

esp_err_t esp_lcd_touch_del(esp_lcd_touch_handle_t touch) {
    CHECK(touch == &g_touch && g_touch.open);
    if (g_fail_touch_delete) {
        g_fail_touch_delete = false;
        return ESP_FAIL;
    }
    g_touch.open = false;
    return ESP_OK;
}

esp_err_t esp_lcd_touch_read_data(esp_lcd_touch_handle_t touch) {
    CHECK(touch == &g_touch && g_touch.open);
    return ESP_FAIL;
}

esp_err_t esp_lcd_touch_get_data(esp_lcd_touch_handle_t touch, esp_lcd_touch_point_data_t *data,
                                 uint8_t *count, uint8_t max_count) {
    (void)touch;
    (void)data;
    (void)max_count;
    *count = 0;
    return ESP_FAIL;
}

esp_err_t iot_button_new_gpio_device(const button_config_t *config,
                                     const button_gpio_config_t *gpio_config,
                                     button_handle_t *out_button) {
    CHECK(config->long_press_time == MYBOT_BOOT_BUTTON_LONG_PRESS_MS);
    CHECK(config->short_press_time == 50);
    CHECK(gpio_config->gpio_num == MYBOT_BOOT_BUTTON_GPIO);
    CHECK(gpio_config->active_level == 0);
    CHECK(!g_button.open);
    if (g_fail_button_new) {
        g_fail_button_new = false;
        return ESP_FAIL;
    }
    g_button.open = true;
    *out_button = &g_button;
    return ESP_OK;
}

esp_err_t iot_button_register_cb(button_handle_t button, button_event_t event, void *event_data,
                                 button_callback_t callback, void *user_data) {
    CHECK(button == &g_button && button->open);
    CHECK(event_data == NULL && callback != NULL);
    if (g_fail_register_event == event) {
        g_fail_register_event = 0;
        return ESP_FAIL;
    }
    if (event == BUTTON_SINGLE_CLICK) {
        button->click = callback;
        button->click_user_data = user_data;
    } else {
        CHECK(event == BUTTON_LONG_PRESS_START);
        button->long_press = callback;
        button->long_press_user_data = user_data;
    }
    return ESP_OK;
}

esp_err_t iot_button_delete(button_handle_t button) {
    CHECK(button == &g_button && button->open);
    ++g_button_delete_calls;
    if (g_fail_button_delete) {
        g_fail_button_delete = false;
        return ESP_FAIL;
    }
    button->open = false;
    button->click = NULL;
    button->long_press = NULL;
    return ESP_OK;
}

mybot_state_t mybot_get_state(void) {
    ++g_state_calls;
    return g_state;
}

int mybot_board_handle_boot_long_press(void) {
    ++g_provision_calls;
    return 0;
}

static void *task_trampoline(void *argument) {
    struct mock_task *task = argument;
    g_current_task = task;
    task->function(task->argument);
    return NULL;
}

int xTaskCreate(task_function_t task, const char *name, uint32_t stack_size, void *argument,
                unsigned int priority, TaskHandle_t *out_task) {
    (void)name;
    CHECK(stack_size == 4096 && priority == 5);
    if (g_fail_task_create) {
        g_fail_task_create = false;
        *out_task = NULL;
        return 0;
    }
    struct mock_task *created = calloc(1, sizeof(*created));
    CHECK(created);
    created->function = task;
    created->argument = argument;
    CHECK(pthread_create(&created->thread, NULL, task_trampoline, created) == 0);
    CHECK(pthread_detach(created->thread) == 0);
    *out_task = created;
    return pdPASS;
}

TaskHandle_t xTaskGetCurrentTaskHandle(void) {
    return g_current_task;
}

void vTaskDelete(TaskHandle_t task) {
    CHECK(task == NULL || task == g_current_task);
    if (g_current_task) {
        free(g_current_task);
        g_current_task = NULL;
    }
    pthread_exit(NULL);
}

void vTaskDelay(TickType_t ticks) {
    struct timespec delay = {
        .tv_sec = ticks / 1000,
        .tv_nsec = (long)(ticks % 1000) * 1000000L,
    };
    nanosleep(&delay, NULL);
}

TickType_t xTaskGetTickCount(void) {
    struct timespec now;
    CHECK(clock_gettime(CLOCK_MONOTONIC, &now) == 0);
    return (TickType_t)((uint64_t)now.tv_sec * 1000U + (uint64_t)now.tv_nsec / 1000000U);
}

SemaphoreHandle_t xSemaphoreCreateBinaryStatic(StaticSemaphore_t *storage) {
    CHECK(pthread_mutex_init(&storage->lock, NULL) == 0);
    CHECK(pthread_cond_init(&storage->condition, NULL) == 0);
    storage->given = false;
    return storage;
}

int xSemaphoreTake(SemaphoreHandle_t semaphore, TickType_t ticks) {
    CHECK(semaphore);
    pthread_mutex_lock(&semaphore->lock);
    if (!ticks) {
        const bool taken = semaphore->given;
        semaphore->given = false;
        pthread_mutex_unlock(&semaphore->lock);
        return taken ? pdTRUE : 0;
    }
    CHECK(ticks == portMAX_DELAY);
    while (!semaphore->given) {
        CHECK(pthread_cond_wait(&semaphore->condition, &semaphore->lock) == 0);
    }
    semaphore->given = false;
    pthread_mutex_unlock(&semaphore->lock);
    return pdTRUE;
}

int xSemaphoreGive(SemaphoreHandle_t semaphore) {
    CHECK(semaphore);
    pthread_mutex_lock(&semaphore->lock);
    semaphore->given = true;
    CHECK(pthread_cond_signal(&semaphore->condition) == 0);
    pthread_mutex_unlock(&semaphore->lock);
    return pdTRUE;
}

int mybot_cst816s_input_start(void);
int mybot_cst816s_input_stop(void);
const mybot_key_ops_t *mybot_cst816s_input_ops(void);

static void fire_button(button_event_t event) {
    CHECK(g_button.open);
    button_callback_t callback =
        event == BUTTON_SINGLE_CLICK ? g_button.click : g_button.long_press;
    void *user_data =
        event == BUTTON_SINGLE_CLICK ? g_button.click_user_data : g_button.long_press_user_data;
    CHECK(callback);
    callback(&g_button, user_data);
}

static void assert_released(void) {
    CHECK(!g_button.open && !g_touch.open && !g_io.open);
}

static unsigned int g_event_count;
static mybot_key_event_t g_last_event;

static void record_event(mybot_key_event_t event, void *user_data) {
    CHECK(user_data == &g_event_count);
    ++g_event_count;
    g_last_event = event;
}

static void test_start_failures_and_retry(void) {
    const mybot_key_ops_t *ops = mybot_cst816s_input_ops();
    void *context = NULL;
    CHECK(ops && ops->init && ops->destroy);
    CHECK(ops->init(&context, record_event, &g_event_count) < 0 && !context);
    CHECK(mybot_cst816s_input_stop() == 0);

    g_fail_button_new = true;
    CHECK(mybot_cst816s_input_start() < 0);
    assert_released();
    g_fail_register_event = BUTTON_LONG_PRESS_START;
    CHECK(mybot_cst816s_input_start() < 0);
    assert_released();
    g_fail_io_new = true;
    CHECK(mybot_cst816s_input_start() < 0);
    assert_released();
    g_fail_touch_new = true;
    CHECK(mybot_cst816s_input_start() < 0);
    assert_released();
    g_fail_task_create = true;
    CHECK(mybot_cst816s_input_start() < 0);
    assert_released();

    CHECK(mybot_cst816s_input_start() == 0);
    CHECK(g_button.open && g_touch.open && g_io.open);
    CHECK(mybot_cst816s_input_start() < 0);
    CHECK(mybot_cst816s_input_stop() == 0);
    CHECK(mybot_cst816s_input_stop() == 0);
    assert_released();
    CHECK(mybot_cst816s_input_start() == 0);
    CHECK(mybot_cst816s_input_stop() == 0);
    assert_released();
}

static void test_cleanup_retry(void) {
    CHECK(mybot_cst816s_input_start() == 0);
    g_fail_touch_delete = true;
    CHECK(mybot_cst816s_input_stop() < 0);
    CHECK(!g_button.open && g_touch.open && g_io.open);
    CHECK(mybot_cst816s_input_stop() == 0);
    assert_released();

    CHECK(mybot_cst816s_input_start() == 0);
    g_fail_io_delete = true;
    CHECK(mybot_cst816s_input_stop() < 0);
    CHECK(!g_button.open && !g_touch.open && g_io.open);
    CHECK(mybot_cst816s_input_stop() == 0);
    assert_released();

    CHECK(mybot_cst816s_input_start() == 0);
    g_fail_button_delete = true;
    CHECK(mybot_cst816s_input_stop() < 0);
    CHECK(g_button.open && !g_touch.open && !g_io.open);
    CHECK(mybot_cst816s_input_stop() == 0);
    assert_released();
}

static void test_callback_lifecycle(void) {
    const mybot_key_ops_t *ops = mybot_cst816s_input_ops();
    CHECK(mybot_cst816s_input_start() == 0);
    for (unsigned int cycle = 0; cycle < 3; ++cycle) {
        void *context = NULL;
        CHECK(ops->init(&context, record_event, &g_event_count) == 0 && context);
        void *duplicate = NULL;
        CHECK(ops->init(&duplicate, record_event, &g_event_count) < 0 && !duplicate);

        g_state = MYBOT_STATE_READY;
        fire_button(BUTTON_SINGLE_CLICK);
        CHECK(g_event_count == cycle * 2 + 1);
        CHECK(g_last_event == MYBOT_KEY_EVENT_CONVERSATION_START);
        g_state = MYBOT_STATE_IN_CONVERSATION;
        fire_button(BUTTON_SINGLE_CLICK);
        CHECK(g_event_count == cycle * 2 + 2);
        CHECK(g_last_event == MYBOT_KEY_EVENT_CONVERSATION_STOP);
        g_state = MYBOT_STATE_STARTING;
        fire_button(BUTTON_SINGLE_CLICK);
        CHECK(g_event_count == cycle * 2 + 2);

        unsigned int provisions = g_provision_calls;
        fire_button(BUTTON_LONG_PRESS_START);
        CHECK(g_provision_calls == provisions + 1);
        ops->destroy(context);

        const unsigned int states = g_state_calls;
        g_state = MYBOT_STATE_READY;
        fire_button(BUTTON_SINGLE_CLICK);
        CHECK(g_state_calls == states && g_event_count == cycle * 2 + 2);
        fire_button(BUTTON_LONG_PRESS_START);
        CHECK(g_provision_calls == provisions + 2);
    }
    CHECK(mybot_cst816s_input_stop() == 0);
}

static pthread_mutex_t g_callback_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_callback_condition = PTHREAD_COND_INITIALIZER;
static bool g_callback_entered;
static bool g_callback_release;
static atomic_bool g_destroy_done;

static void blocking_event(mybot_key_event_t event, void *user_data) {
    CHECK(event == MYBOT_KEY_EVENT_CONVERSATION_START);
    CHECK(user_data == &g_callback_entered);
    pthread_mutex_lock(&g_callback_lock);
    g_callback_entered = true;
    pthread_cond_broadcast(&g_callback_condition);
    while (!g_callback_release) {
        CHECK(pthread_cond_wait(&g_callback_condition, &g_callback_lock) == 0);
    }
    pthread_mutex_unlock(&g_callback_lock);
}

static void wait_for_callback(void) {
    struct timespec deadline;
    CHECK(clock_gettime(CLOCK_REALTIME, &deadline) == 0);
    deadline.tv_sec += 2;
    pthread_mutex_lock(&g_callback_lock);
    while (!g_callback_entered) {
        const int result =
            pthread_cond_timedwait(&g_callback_condition, &g_callback_lock, &deadline);
        CHECK(result != ETIMEDOUT && result == 0);
    }
    pthread_mutex_unlock(&g_callback_lock);
}

static void *click_thread(void *argument) {
    (void)argument;
    fire_button(BUTTON_SINGLE_CLICK);
    return NULL;
}

static void *destroy_thread(void *argument) {
    mybot_cst816s_input_ops()->destroy(argument);
    atomic_store(&g_destroy_done, true);
    return NULL;
}

static void test_destroy_waits_for_callback(void) {
    const mybot_key_ops_t *ops = mybot_cst816s_input_ops();
    CHECK(mybot_cst816s_input_start() == 0);
    void *context = NULL;
    g_state = MYBOT_STATE_READY;
    CHECK(ops->init(&context, blocking_event, &g_callback_entered) == 0);

    pthread_t click;
    CHECK(pthread_create(&click, NULL, click_thread, NULL) == 0);
    wait_for_callback();
    atomic_store(&g_destroy_done, false);
    pthread_t destroy;
    CHECK(pthread_create(&destroy, NULL, destroy_thread, context) == 0);

    struct timespec pause = {.tv_nsec = 50000000L};
    nanosleep(&pause, NULL);
    CHECK(!atomic_load(&g_destroy_done));

    pthread_mutex_lock(&g_callback_lock);
    g_callback_release = true;
    pthread_cond_broadcast(&g_callback_condition);
    pthread_mutex_unlock(&g_callback_lock);
    CHECK(pthread_join(click, NULL) == 0);
    CHECK(pthread_join(destroy, NULL) == 0);
    CHECK(atomic_load(&g_destroy_done));
    CHECK(mybot_cst816s_input_stop() == 0);
    assert_released();
}

int main(void) {
    test_start_failures_and_retry();
    test_cleanup_retry();
    test_callback_lifecycle();
    test_destroy_waits_for_callback();
    printf("CST816S/Boot input host fault-injection tests passed (%u button deletes)\n",
           g_button_delete_calls);
    return 0;
}

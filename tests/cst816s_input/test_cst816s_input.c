/* SPDX-License-Identifier: MIT */
#include "mock_platform.h"

#include "board_config.h"

#if INPUT_TEST_AMOLED175
#define MYBOT_TOUCH_I2C_SPEED_HZ MYBOT_AMOLED175_I2C_SPEED_HZ
#define MYBOT_TOUCH_RESET MYBOT_AMOLED175_TOUCH_RESET
#define MYBOT_TOUCH_INTERRUPT MYBOT_AMOLED175_TOUCH_INTERRUPT
#define MYBOT_TOUCH_RESET_LEVEL 0
#define MYBOT_TOUCH_INTERRUPT_LEVEL 0
#define MYBOT_TOUCH_SWAP_XY 0
#define MYBOT_TOUCH_MIRROR_X 1
#define MYBOT_TOUCH_MIRROR_Y 1
#define MYBOT_BOOT_BUTTON_LONG_PRESS_MS MYBOT_AMOLED175_INPUT_LONG_PRESS_MS
#define EXPECTED_TOUCH_ADDRESS ESP_LCD_TOUCH_IO_I2C_CST9217_ADDRESS
#define EXPECTED_TASK_STACK 3072
#define EXPECTED_TASK_PRIORITY 4
#define mybot_cst816s_input_start mybot_amoled175_input_start
#define mybot_cst816s_input_stop mybot_amoled175_input_stop
#define mybot_cst816s_input_ops mybot_amoled175_input_ops
#define mybot_board_touch_i2c_bus_handle mybot_amoled175_i2c_bus_handle
#define esp_lcd_touch_new_i2c_cst816s esp_lcd_touch_new_i2c_cst9217
#elif INPUT_TEST_VOCAT
#define MYBOT_TOUCH_I2C_SPEED_HZ MYBOT_VOCAT_I2C_SPEED_HZ
#define MYBOT_TOUCH_RESET MYBOT_VOCAT_TOUCH_RESET
#define MYBOT_TOUCH_INTERRUPT MYBOT_VOCAT_TOUCH_INTERRUPT
#define MYBOT_TOUCH_RESET_LEVEL 0
#define MYBOT_TOUCH_INTERRUPT_LEVEL 0
#define MYBOT_TOUCH_SWAP_XY 0
#define MYBOT_TOUCH_MIRROR_X 0
#define MYBOT_TOUCH_MIRROR_Y 0
#define MYBOT_BOOT_BUTTON_LONG_PRESS_MS MYBOT_VOCAT_INPUT_LONG_PRESS_MS
#define EXPECTED_TOUCH_ADDRESS ESP_LCD_TOUCH_IO_I2C_CST816S_ADDRESS
#define EXPECTED_TASK_STACK 3072
#define EXPECTED_TASK_PRIORITY 4
#define mybot_cst816s_input_start mybot_vocat_input_start
#define mybot_cst816s_input_stop mybot_vocat_input_stop
#define mybot_cst816s_input_ops mybot_vocat_input_ops
#define mybot_board_touch_i2c_bus_handle mybot_vocat_i2c_bus_handle
#else
#define EXPECTED_TOUCH_ADDRESS ESP_LCD_TOUCH_IO_I2C_CST816S_ADDRESS
#define EXPECTED_TASK_STACK 4096
#define EXPECTED_TASK_PRIORITY 5
#endif

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
    atomic_bool suspended;
    atomic_bool delete_requested;
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
static atomic_bool g_hold_task;
static atomic_bool g_task_entered;
static atomic_bool g_hold_suspend;
static atomic_bool g_suspend_entered;
static atomic_uint g_tasks_live;
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
    CHECK(config->dev_addr == EXPECTED_TOUCH_ADDRESS);
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
    atomic_store(&g_task_entered, true);
    while (atomic_load(&g_hold_task)) {
        vTaskDelay(1);
    }
    task->function(task->argument);
    return NULL;
}

int xTaskCreateWithCaps(task_function_t task, const char *name, uint32_t stack_size, void *argument,
                        unsigned int priority, TaskHandle_t *out_task, unsigned int capabilities) {
    (void)name;
    CHECK(stack_size == EXPECTED_TASK_STACK && priority == EXPECTED_TASK_PRIORITY);
    CHECK(capabilities == (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (g_fail_task_create) {
        g_fail_task_create = false;
        *out_task = NULL;
        return 0;
    }
    struct mock_task *created = calloc(1, sizeof(*created));
    CHECK(created);
    created->function = task;
    created->argument = argument;
    *out_task = created;
    atomic_fetch_add(&g_tasks_live, 1);
    CHECK(pthread_create(&created->thread, NULL, task_trampoline, created) == 0);
    return pdPASS;
}

TaskHandle_t xTaskGetCurrentTaskHandle(void) {
    return g_current_task;
}

eTaskState eTaskGetState(TaskHandle_t task) {
    CHECK(task);
    return atomic_load(&task->suspended) ? eSuspended : eRunning;
}

void vTaskSuspend(TaskHandle_t task) {
    CHECK(task == NULL && g_current_task);
    atomic_store(&g_suspend_entered, true);
    while (atomic_load(&g_hold_suspend)) {
        vTaskDelay(1);
    }
    atomic_store(&g_current_task->suspended, true);
    while (!atomic_load(&g_current_task->delete_requested)) {
        vTaskDelay(1);
    }
    pthread_exit(NULL);
}

void vTaskDeleteWithCaps(TaskHandle_t task) {
    CHECK(task && task != g_current_task);
    CHECK(atomic_load(&task->suspended));
    atomic_store(&task->delete_requested, true);
    CHECK(pthread_join(task->thread, NULL) == 0);
    atomic_fetch_sub(&g_tasks_live, 1);
    free(task);
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
    struct timespec deadline;
    CHECK(clock_gettime(CLOCK_REALTIME, &deadline) == 0);
    deadline.tv_sec += ticks / 1000;
    deadline.tv_nsec += (long)(ticks % 1000) * 1000000L;
    if (deadline.tv_nsec >= 1000000000L) {
        ++deadline.tv_sec;
        deadline.tv_nsec -= 1000000000L;
    }
    while (!semaphore->given) {
        const int result =
            ticks == portMAX_DELAY
                ? pthread_cond_wait(&semaphore->condition, &semaphore->lock)
                : pthread_cond_timedwait(&semaphore->condition, &semaphore->lock, &deadline);
        if (result == ETIMEDOUT) {
            pthread_mutex_unlock(&semaphore->lock);
            return 0;
        }
        CHECK(result == 0);
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
    CHECK(atomic_load(&g_tasks_live) == 0);
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

    g_fail_task_create = true;
    g_fail_touch_delete = true;
    CHECK(mybot_cst816s_input_start() < 0);
    CHECK(g_touch.open && g_io.open && !g_button.open);
    CHECK(mybot_cst816s_input_start() < 0);
    CHECK(mybot_cst816s_input_stop() == 0);
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

static void test_stop_waits_for_callback_and_retries(void) {
    pthread_mutex_lock(&g_callback_lock);
    g_callback_entered = false;
    g_callback_release = false;
    pthread_mutex_unlock(&g_callback_lock);
    CHECK(mybot_cst816s_input_start() == 0);
    void *context = NULL;
    g_state = MYBOT_STATE_READY;
    CHECK(mybot_cst816s_input_ops()->init(&context, blocking_event, &g_callback_entered) == 0);
    pthread_t click;
    CHECK(pthread_create(&click, NULL, click_thread, NULL) == 0);
    wait_for_callback();

    const TickType_t started = xTaskGetTickCount();
    CHECK(mybot_cst816s_input_stop() < 0);
    CHECK(xTaskGetTickCount() - started < 2000);
    CHECK(g_button.open && g_touch.open && g_io.open);
    CHECK(atomic_load(&g_tasks_live) == 0);
    CHECK(mybot_cst816s_input_start() < 0);

    pthread_mutex_lock(&g_callback_lock);
    g_callback_release = true;
    pthread_cond_broadcast(&g_callback_condition);
    pthread_mutex_unlock(&g_callback_lock);
    CHECK(pthread_join(click, NULL) == 0);
    CHECK(mybot_cst816s_input_stop() == 0);
    assert_released();
}

esp_err_t gpio_set_intr_type(gpio_num_t gpio, int interrupt_type) {
    CHECK(gpio == MYBOT_TOUCH_INTERRUPT && interrupt_type == GPIO_INTR_ANYEDGE);
    return ESP_OK;
}

int xSemaphoreGiveFromISR(SemaphoreHandle_t semaphore, BaseType_t *woken) {
    *woken = pdFALSE;
    return xSemaphoreGive(semaphore);
}

uint32_t ulTaskNotifyTake(BaseType_t clear, TickType_t ticks) {
    CHECK(clear == pdTRUE);
    vTaskDelay(ticks);
    return 0;
}

static void wait_for_flag(atomic_bool *flag) {
    const TickType_t started = xTaskGetTickCount();
    while (!atomic_load(flag)) {
        CHECK(xTaskGetTickCount() - started < 2000);
        vTaskDelay(1);
    }
}

static void test_stop_timeout_retry(void) {
    atomic_store(&g_hold_task, true);
    atomic_store(&g_task_entered, false);
    CHECK(mybot_cst816s_input_start() == 0);
    wait_for_flag(&g_task_entered);
    const TickType_t started = xTaskGetTickCount();
    CHECK(mybot_cst816s_input_stop() < 0);
    CHECK(xTaskGetTickCount() - started < 2000);
    CHECK(g_button.open && g_touch.open && g_io.open);
    CHECK(atomic_load(&g_tasks_live) == 1);
    CHECK(mybot_cst816s_input_start() < 0);
    atomic_store(&g_hold_task, false);
    CHECK(mybot_cst816s_input_stop() == 0);
    assert_released();

    atomic_store(&g_hold_suspend, true);
    atomic_store(&g_suspend_entered, false);
    CHECK(mybot_cst816s_input_start() == 0);
    CHECK(mybot_cst816s_input_stop() < 0);
    wait_for_flag(&g_suspend_entered);
    CHECK(g_button.open && g_touch.open && g_io.open);
    CHECK(atomic_load(&g_tasks_live) == 1);
    atomic_store(&g_hold_suspend, false);
    CHECK(mybot_cst816s_input_stop() == 0);
    assert_released();
}

int main(void) {
    test_start_failures_and_retry();
    test_cleanup_retry();
    test_stop_timeout_retry();
    test_callback_lifecycle();
    test_destroy_waits_for_callback();
    test_stop_waits_for_callback_and_retries();
    printf("Touch/Boot input PSRAM lifecycle fault-injection tests passed (%u button deletes)\n",
           g_button_delete_calls);
    return 0;
}

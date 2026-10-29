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

struct mock_i2c_device {
    bool open;
};

struct mock_button {
    bool open;
    button_callback_t click;
    button_callback_t long_press;
};

struct mock_task {
    pthread_t thread;
    task_function_t function;
    void *argument;
    atomic_bool suspended;
    atomic_bool delete_requested;
};

static struct mock_i2c_bus g_bus;
static struct mock_i2c_device g_device;
static struct mock_button g_button;
static bool g_gpio_configured;
static unsigned int g_gpio_levels[2];
static unsigned int g_gpio_level_count;
static unsigned int g_add_count;
static unsigned int g_remove_count;
static unsigned int g_transmit_count;
static _Atomic unsigned int g_receive_count;
static _Atomic unsigned int g_down_reads;
static bool g_fail_add;
static bool g_fail_remove;
static bool g_fail_task_create;
static mybot_state_t g_state = MYBOT_STATE_STARTING;
static _Thread_local struct mock_task *g_current_task;

static pthread_mutex_t g_event_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_event_condition = PTHREAD_COND_INITIALIZER;
static bool g_event_seen;

static void reset_mock(void) {
    g_device = (struct mock_i2c_device){0};
    g_button = (struct mock_button){0};
    g_gpio_configured = false;
    g_gpio_level_count = 0;
    g_add_count = 0;
    g_remove_count = 0;
    g_transmit_count = 0;
    atomic_store(&g_receive_count, 0);
    atomic_store(&g_down_reads, 0);
    g_fail_add = false;
    g_fail_remove = false;
    g_fail_task_create = false;
    g_state = MYBOT_STATE_STARTING;
    pthread_mutex_lock(&g_event_lock);
    g_event_seen = false;
    pthread_mutex_unlock(&g_event_lock);
}

const char *esp_err_to_name(esp_err_t error) {
    return error == ESP_OK ? "ESP_OK" : "ESP_FAIL";
}

esp_err_t gpio_config(const gpio_config_t *config) {
    CHECK(config && config->pin_bit_mask == 1ULL << MYBOT_TOUCH_RESET);
    CHECK(config->mode == GPIO_MODE_OUTPUT);
    CHECK(config->pull_up_en == GPIO_PULLUP_DISABLE);
    CHECK(config->pull_down_en == GPIO_PULLDOWN_DISABLE);
    CHECK(config->intr_type == GPIO_INTR_DISABLE);
    CHECK(!g_gpio_configured);
    g_gpio_configured = true;
    return ESP_OK;
}

esp_err_t gpio_set_level(gpio_num_t gpio, uint32_t level) {
    CHECK(g_gpio_configured && gpio == MYBOT_TOUCH_RESET && level <= 1);
    CHECK(g_gpio_level_count < sizeof(g_gpio_levels) / sizeof(g_gpio_levels[0]));
    g_gpio_levels[g_gpio_level_count++] = level;
    return ESP_OK;
}

i2c_master_bus_handle_t mybot_board_touch_i2c_bus_handle(void) {
    return &g_bus;
}

esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus, const i2c_device_config_t *config,
                                    i2c_master_dev_handle_t *out_device) {
    CHECK(bus == &g_bus && config && out_device && !g_device.open);
    CHECK(config->dev_addr_length == I2C_ADDR_BIT_LEN_7);
    CHECK(config->device_address == 0x5A && config->scl_speed_hz == MYBOT_TOUCH_I2C_SPEED_HZ);
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
        g_fail_remove = false;
        return ESP_FAIL;
    }
    g_device.open = false;
    return ESP_OK;
}

esp_err_t i2c_master_transmit(i2c_master_dev_handle_t device, const uint8_t *data, size_t size,
                              int timeout_ms) {
    CHECK(device == &g_device && device->open && data && size == 1 && data[0] == 0x00);
    CHECK(timeout_ms == 100);
    ++g_transmit_count;
    return ESP_OK;
}

esp_err_t i2c_master_receive(i2c_master_dev_handle_t device, uint8_t *data, size_t size,
                             int timeout_ms) {
    CHECK(device == &g_device && device->open && data && size == 6 && timeout_ms == 100);
    atomic_fetch_add(&g_receive_count, 1);
    unsigned int down_reads = atomic_load(&g_down_reads);
    if (down_reads) {
        atomic_store(&g_down_reads, down_reads - 1);
        data[1] = 0x01;
        data[2] = 0x01;
        data[3] = 0xF4;
        data[5] = 0x01;
    } else {
        for (size_t index = 0; index < size; ++index) {
            data[index] = 0;
        }
    }
    return ESP_OK;
}

esp_err_t iot_button_new_gpio_device(const button_config_t *config,
                                     const button_gpio_config_t *gpio_config,
                                     button_handle_t *out_button) {
    CHECK(config && gpio_config && out_button && !g_button.open);
    CHECK(config->long_press_time == MYBOT_BOOT_BUTTON_LONG_PRESS_MS);
    CHECK(gpio_config->gpio_num == MYBOT_BOOT_BUTTON_GPIO);
    g_button.open = true;
    *out_button = &g_button;
    return ESP_OK;
}

esp_err_t iot_button_register_cb(button_handle_t button, button_event_t event, void *event_data,
                                 button_callback_t callback, void *user_data) {
    CHECK(button == &g_button && button->open && !event_data && callback && !user_data);
    if (event == BUTTON_SINGLE_CLICK) {
        button->click = callback;
    } else {
        CHECK(event == BUTTON_LONG_PRESS_START);
        button->long_press = callback;
    }
    return ESP_OK;
}

esp_err_t iot_button_delete(button_handle_t button) {
    CHECK(button == &g_button && button->open);
    button->open = false;
    return ESP_OK;
}

mybot_state_t mybot_get_state(void) {
    return g_state;
}

int mybot_board_handle_boot_long_press(void) {
    return 0;
}

static void *task_trampoline(void *argument) {
    struct mock_task *task = argument;
    g_current_task = task;
    task->function(task->argument);
    return NULL;
}

int xTaskCreateWithCaps(task_function_t task, const char *name, uint32_t stack_size, void *argument,
                        unsigned int priority, TaskHandle_t *out_task, unsigned int capabilities) {
    (void)name;
    CHECK(stack_size == 4096 && priority == 5);
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

static void record_event(mybot_key_event_t event, void *user_data) {
    CHECK(event == MYBOT_KEY_EVENT_CONVERSATION_START);
    CHECK(user_data == &g_event_seen);
    pthread_mutex_lock(&g_event_lock);
    g_event_seen = true;
    pthread_cond_broadcast(&g_event_condition);
    pthread_mutex_unlock(&g_event_lock);
}

static void wait_for_event(void) {
    struct timespec deadline;
    CHECK(clock_gettime(CLOCK_REALTIME, &deadline) == 0);
    deadline.tv_sec += 2;
    pthread_mutex_lock(&g_event_lock);
    while (!g_event_seen) {
        const int result = pthread_cond_timedwait(&g_event_condition, &g_event_lock, &deadline);
        CHECK(result != ETIMEDOUT && result == 0);
    }
    pthread_mutex_unlock(&g_event_lock);
}

static void test_start_failures(void) {
    reset_mock();
    g_fail_add = true;
    CHECK(mybot_cst816s_input_start() < 0);
    CHECK(!g_button.open && !g_device.open && g_add_count == 1 && g_remove_count == 0);

    reset_mock();
    g_fail_task_create = true;
    CHECK(mybot_cst816s_input_start() < 0);
    CHECK(!g_button.open && !g_device.open && g_add_count == 1 && g_remove_count == 1);
}

static void test_cleanup_retry(void) {
    reset_mock();
    CHECK(mybot_cst816s_input_start() == 0);
    CHECK(g_button.open && g_device.open);
    CHECK(g_gpio_level_count == 2);
    CHECK(g_gpio_levels[0] == MYBOT_TOUCH_RESET_LEVEL);
    CHECK(g_gpio_levels[1] == !MYBOT_TOUCH_RESET_LEVEL);
    g_fail_remove = true;
    CHECK(mybot_cst816s_input_stop() < 0);
    CHECK(!g_button.open && g_device.open);
    CHECK(mybot_cst816s_input_stop() == 0);
    CHECK(!g_button.open && !g_device.open);
    CHECK(mybot_cst816s_input_stop() == 0);
}

static void test_touch_short_press(void) {
    reset_mock();
    g_state = MYBOT_STATE_READY;
    atomic_store(&g_down_reads, 3);
    CHECK(mybot_cst816s_input_start() == 0);
    const mybot_key_ops_t *ops = mybot_cst816s_input_ops();
    void *context = NULL;
    CHECK(ops && ops->init(&context, record_event, &g_event_seen) == 0 && context);
    wait_for_event();
    CHECK(atomic_load(&g_receive_count) >= 4 && g_transmit_count >= 4);
    ops->destroy(context);
    CHECK(mybot_cst816s_input_stop() == 0);
    CHECK(!g_button.open && !g_device.open);
}

int main(void) {
    test_start_failures();
    test_cleanup_retry();
    test_touch_short_press();
    puts("CST226 direct-I2C input host tests passed");
    return 0;
}

/* SPDX-License-Identifier: MIT */
#ifndef CST816S_INPUT_MOCK_PLATFORM_H_
#define CST816S_INPUT_MOCK_PLATFORM_H_

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERR_NO_MEM 0x101
const char *esp_err_to_name(esp_err_t error);

typedef int gpio_num_t;
#define GPIO_NUM_NC -1
#define GPIO_NUM_0 0
#define GPIO_NUM_1 1
#define GPIO_NUM_2 2
#define GPIO_NUM_3 3
#define GPIO_NUM_4 4
#define GPIO_NUM_5 5
#define GPIO_NUM_7 7
#define GPIO_NUM_10 10
#define GPIO_NUM_11 11
#define GPIO_NUM_15 15
#define GPIO_NUM_21 21
#define GPIO_NUM_38 38
#define GPIO_NUM_39 39
#define GPIO_NUM_40 40
#define GPIO_NUM_41 41
#define GPIO_NUM_42 42
#define GPIO_NUM_45 45
#define GPIO_NUM_46 46
#define GPIO_NUM_47 47
#define GPIO_NUM_48 48

#define I2C_NUM_0 0
#define I2C_NUM_1 1
#define SPI2_HOST 2

#define ESP_LOGI(...) ((void)0)
#define ESP_LOGW(...) ((void)0)
#define ESP_LOGE(...) ((void)0)

typedef pthread_mutex_t portMUX_TYPE;
#define portMUX_INITIALIZER_UNLOCKED PTHREAD_MUTEX_INITIALIZER
#define portENTER_CRITICAL(lock) ((void)pthread_mutex_lock(lock))
#define portEXIT_CRITICAL(lock) ((void)pthread_mutex_unlock(lock))

#define pdMS_TO_TICKS(ms) (ms)
#define pdPASS 1
#define pdTRUE 1
#define portMAX_DELAY UINT32_MAX
typedef uint32_t TickType_t;

typedef struct mock_task *TaskHandle_t;
typedef void (*task_function_t)(void *);
int xTaskCreate(task_function_t task, const char *name, uint32_t stack_size, void *argument,
                unsigned int priority, TaskHandle_t *out_task);
TaskHandle_t xTaskGetCurrentTaskHandle(void);
void vTaskDelete(TaskHandle_t task);
void vTaskDelay(TickType_t ticks);
TickType_t xTaskGetTickCount(void);

typedef struct mock_semaphore {
    pthread_mutex_t lock;
    pthread_cond_t condition;
    bool given;
} StaticSemaphore_t;
typedef StaticSemaphore_t *SemaphoreHandle_t;
SemaphoreHandle_t xSemaphoreCreateBinaryStatic(StaticSemaphore_t *storage);
int xSemaphoreTake(SemaphoreHandle_t semaphore, TickType_t ticks);
int xSemaphoreGive(SemaphoreHandle_t semaphore);

typedef struct mock_i2c_bus *i2c_master_bus_handle_t;
typedef struct mock_panel_io *esp_lcd_panel_io_handle_t;
typedef struct mock_touch *esp_lcd_touch_handle_t;

typedef struct {
    uint16_t dev_addr;
    uint32_t scl_speed_hz;
    unsigned int control_phase_bytes;
    unsigned int dc_bit_offset;
    unsigned int lcd_cmd_bits;
    unsigned int lcd_param_bits;
    struct {
        unsigned int disable_control_phase : 1;
    } flags;
} esp_lcd_panel_io_i2c_config_t;
esp_err_t esp_lcd_new_panel_io_i2c(i2c_master_bus_handle_t bus,
                                   const esp_lcd_panel_io_i2c_config_t *config,
                                   esp_lcd_panel_io_handle_t *out_io);
esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io);

typedef struct {
    uint16_t x;
    uint16_t y;
} esp_lcd_touch_point_data_t;
typedef struct {
    uint16_t x_max;
    uint16_t y_max;
    gpio_num_t rst_gpio_num;
    gpio_num_t int_gpio_num;
    struct {
        uint8_t reset;
        uint8_t interrupt;
    } levels;
    struct {
        uint8_t swap_xy;
        uint8_t mirror_x;
        uint8_t mirror_y;
    } flags;
} esp_lcd_touch_config_t;
esp_err_t esp_lcd_touch_new_i2c_cst816s(esp_lcd_panel_io_handle_t io,
                                        const esp_lcd_touch_config_t *config,
                                        esp_lcd_touch_handle_t *out_touch);
esp_err_t esp_lcd_touch_del(esp_lcd_touch_handle_t touch);
esp_err_t esp_lcd_touch_read_data(esp_lcd_touch_handle_t touch);
esp_err_t esp_lcd_touch_get_data(esp_lcd_touch_handle_t touch, esp_lcd_touch_point_data_t *data,
                                 uint8_t *count, uint8_t max_count);
#define ESP_LCD_TOUCH_IO_I2C_CST816S_ADDRESS 0x15

typedef struct mock_button *button_handle_t;
typedef struct {
    uint32_t long_press_time;
    uint32_t short_press_time;
} button_config_t;
typedef struct {
    gpio_num_t gpio_num;
    int active_level;
    bool enable_power_save;
    bool disable_pull;
} button_gpio_config_t;
typedef enum {
    BUTTON_SINGLE_CLICK = 1,
    BUTTON_LONG_PRESS_START = 2,
} button_event_t;
typedef void (*button_callback_t)(void *button, void *user_data);
esp_err_t iot_button_new_gpio_device(const button_config_t *config,
                                     const button_gpio_config_t *gpio_config,
                                     button_handle_t *out_button);
esp_err_t iot_button_register_cb(button_handle_t button, button_event_t event, void *event_data,
                                 button_callback_t callback, void *user_data);
esp_err_t iot_button_delete(button_handle_t button);

typedef enum {
    MYBOT_STATE_STARTING,
    MYBOT_STATE_READY,
    MYBOT_STATE_IN_CONVERSATION,
} mybot_state_t;
mybot_state_t mybot_get_state(void);

typedef enum {
    MYBOT_KEY_EVENT_CONVERSATION_START,
    MYBOT_KEY_EVENT_CONVERSATION_STOP,
} mybot_key_event_t;
typedef void (*mybot_key_event_handler_t)(mybot_key_event_t event, void *user_data);
typedef struct {
    int (*init)(void **out_context, mybot_key_event_handler_t emit, void *user_data);
    void (*destroy)(void *context);
} mybot_key_ops_t;

int mybot_board_handle_boot_long_press(void);

#endif /* CST816S_INPUT_MOCK_PLATFORM_H_ */

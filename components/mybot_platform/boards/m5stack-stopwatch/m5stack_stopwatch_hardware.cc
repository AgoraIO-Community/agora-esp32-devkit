/* SPDX-License-Identifier: Apache-2.0 */
#include "m5stack_stopwatch_hardware.h"

#include "M5IOE1.h"
#include "M5PM1.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <cstdint>

#define TAG "m5stack_stopwatch_hw"

namespace {
constexpr uint8_t kIoExpanderAddress = 0x4F;
constexpr uint8_t kPowerManagerAddress = 0x6E;
constexpr uint8_t kChargeStatePin = 2;
constexpr uint8_t kChargeCurrentPin = 3;
constexpr uint8_t kLcdPowerPin = 7;
constexpr uint8_t kLcdResetPin = 4;
constexpr uint8_t kTouchResetPin = 3;
constexpr uint8_t kCodecPowerPin = 2;
constexpr uint8_t kSpeakerPaPin = 9;
constexpr uint8_t kMotorPin = 8;

M5IOE1 *s_io_expander;
M5PM1 *s_power_manager;
i2c_master_bus_handle_t s_bus;
bool s_initialized;

int set_ioe_output(M5IOE1 &ioe, uint8_t pin, uint8_t level) {
    ioe.pinMode(pin, OUTPUT);
    if (ioe.setDriveMode(pin, M5IOE1_DRIVE_PUSHPULL) != M5IOE1_OK) {
        return -1;
    }
    m5ioe1_err_t result = M5IOE1_FAIL;
    ioe.digitalWriteWithRes(pin, level, &result);
    return result == M5IOE1_OK ? 0 : -1;
}

int initialize_expander() {
    s_io_expander = new M5IOE1();
    if (!s_io_expander || s_io_expander->begin(s_bus, kIoExpanderAddress, M5IOE1_I2C_FREQ_100K,
                                               M5IOE1_INT_MODE_POLLING) != M5IOE1_OK) {
        return -1;
    }
    if (set_ioe_output(*s_io_expander, kLcdPowerPin, HIGH) < 0 ||
        set_ioe_output(*s_io_expander, kCodecPowerPin, HIGH) < 0 ||
        set_ioe_output(*s_io_expander, kSpeakerPaPin, LOW) < 0 ||
        set_ioe_output(*s_io_expander, kMotorPin, LOW) < 0 ||
        set_ioe_output(*s_io_expander, kTouchResetPin, HIGH) < 0 ||
        set_ioe_output(*s_io_expander, kLcdResetPin, HIGH) < 0) {
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(20));
    if (set_ioe_output(*s_io_expander, kLcdResetPin, LOW) < 0) {
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    if (set_ioe_output(*s_io_expander, kLcdResetPin, HIGH) < 0) {
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(120));
    return 0;
}

int initialize_power_manager() {
    s_power_manager = new M5PM1();
    if (!s_power_manager ||
        s_power_manager->begin(s_bus, kPowerManagerAddress, M5PM1_I2C_FREQ_100K) != M5PM1_OK) {
        return -1;
    }
    if (s_power_manager->setChargeEnable(true) != M5PM1_OK ||
        s_power_manager->setBoostEnable(true) != M5PM1_OK) {
        return -1;
    }
    s_power_manager->pinMode(kChargeStatePin, INPUT);
    s_power_manager->pinMode(kChargeCurrentPin, OUTPUT);
    s_power_manager->digitalWrite(kChargeCurrentPin, LOW);
    return 0;
}
} // namespace

int mybot_m5stack_stopwatch_hardware_init(i2c_master_bus_handle_t bus) {
    if (s_initialized || !bus) {
        return -1;
    }
    s_bus = bus;
    if (initialize_expander() < 0 || initialize_power_manager() < 0) {
        ESP_LOGE(TAG, "event=hardware action=initialize result=error");
        (void)mybot_m5stack_stopwatch_hardware_deinit();
        return -1;
    }
    s_initialized = true;
    return 0;
}

int mybot_m5stack_stopwatch_set_speaker_pa(bool enabled) {
    if (!s_initialized || !s_io_expander) {
        return -1;
    }
    m5ioe1_err_t result = M5IOE1_FAIL;
    s_io_expander->digitalWriteWithRes(kSpeakerPaPin, enabled ? HIGH : LOW, &result);
    return result == M5IOE1_OK ? 0 : -1;
}

int mybot_m5stack_stopwatch_hardware_deinit(void) {
    int result = 0;
    if (s_io_expander) {
        m5ioe1_err_t io_result = M5IOE1_FAIL;
        s_io_expander->digitalWriteWithRes(kSpeakerPaPin, LOW, &io_result);
        if (io_result != M5IOE1_OK) {
            result = -1;
        }
        s_io_expander->digitalWriteWithRes(kMotorPin, LOW, &io_result);
        if (io_result != M5IOE1_OK) {
            result = -1;
        }
    }
    delete s_io_expander;
    s_io_expander = nullptr;
    delete s_power_manager;
    s_power_manager = nullptr;
    s_bus = nullptr;
    s_initialized = false;
    return result;
}

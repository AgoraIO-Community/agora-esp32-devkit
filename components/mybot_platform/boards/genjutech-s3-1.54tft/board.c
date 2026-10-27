/* SPDX-License-Identifier: Apache-2.0 */
#include "mybot_platform/board.h"

#include <mybot/platform/mybot_platform.h>

#include "announcement/embedded_ogg_prompt.h"
#include "board_config.h"
#include "esp_log.h"
#include "genjutech_hardware.h"
#include "network/wifi_control.h"
#include "platform/board_actions.h"

#include <stdbool.h>

#define TAG "genjutech_board"

const mybot_audio_capture_ops_t *mybot_es8311_audio_capture_ops(void);
const mybot_audio_playback_ops_t *mybot_es8311_audio_playback_ops(void);
const mybot_audio_volume_ops_t *mybot_es8311_audio_volume_ops(void);
const mybot_announce_ops_t *mybot_esp32s3_announce_ops(void);
const mybot_https_ops_t *mybot_esp32s3_https_ops(void);
const mybot_key_ops_t *mybot_esp32s3_button_ops(void);
const mybot_kv_store_ops_t *mybot_esp32s3_kv_store_ops(void);
const mybot_lcd_ops_t *mybot_shared_lvgl_ops(void);
const mybot_wifi_ops_t *mybot_esp32s3_wifi_ops(void);
int mybot_esp32s3_buttons_start(void);

static const mybot_lcd_ops_t *s_lcd_ops;
static void *s_lcd_context;
static bool s_automatic_provisioning_started;
static bool s_prepared;

static int board_show_screen(mybot_lcd_screen_t screen) {
    if (!s_lcd_ops || !s_lcd_context) {
        return -1;
    }
    const mybot_lcd_content_t content = {.screen = screen};
    return s_lcd_ops->render(s_lcd_context, &content);
}

static void board_on_provisioning(const char *trigger) {
    (void)board_show_screen(MYBOT_LCD_SCREEN_WIFI_PROVISIONING);
    (void)mybot_embedded_ogg_play_wifi_provisioning(mybot_es8311_audio_playback_ops(),
                                                    mybot_es8311_audio_volume_ops(), trigger);
}

static void board_on_automatic_provisioning(void) {
    s_automatic_provisioning_started = true;
    board_on_provisioning("automatic");
}

static void board_on_button_provisioning(void) {
    board_on_provisioning("button");
}

static int board_prepare(void) {
    if (s_prepared) {
        return 0;
    }
    if (mybot_genjutech_hardware_init() < 0) {
        return -1;
    }
    s_lcd_ops = mybot_shared_lvgl_ops();
    if (!s_lcd_ops || s_lcd_ops->init(&s_lcd_context) < 0 ||
        board_show_screen(MYBOT_LCD_SCREEN_STARTING) < 0 || mybot_esp32s3_buttons_start() < 0) {
        if (s_lcd_context) {
            s_lcd_ops->destroy(s_lcd_context);
            s_lcd_context = NULL;
        }
        (void)mybot_genjutech_hardware_deinit();
        return -1;
    }
    s_prepared = true;
    return 0;
}

int mybot_board_handle_boot_long_press(void) {
    mybot_board_request_wifi_provisioning();
    return 0;
}

static int board_provision_wifi(void) {
    return mybot_wifi_run_provisioning(board_on_button_provisioning);
}

static int board_ensure_network(const char *device_id) {
    s_automatic_provisioning_started = false;
    int result = mybot_wifi_ensure_network(device_id, board_on_automatic_provisioning);
    if (s_automatic_provisioning_started) {
        (void)mybot_board_wait_wifi_provisioning_request(0);
    }
    return result;
}

static void board_shutdown_network(void) {
    mybot_wifi_shutdown_network();
}

static int board_register_platform(void) {
    const mybot_platform_descriptor_t descriptor = {
        .wifi = mybot_esp32s3_wifi_ops(),
        .kv_store = mybot_esp32s3_kv_store_ops(),
        .key = mybot_esp32s3_button_ops(),
        .audio_capture = mybot_es8311_audio_capture_ops(),
        .audio_playback = mybot_es8311_audio_playback_ops(),
        .audio_volume = mybot_es8311_audio_volume_ops(),
        .announce = mybot_esp32s3_announce_ops(),
        .https = mybot_esp32s3_https_ops(),
        .lcd = mybot_shared_lvgl_ops(),
    };
    return mybot_platform_register(&descriptor);
}

void *mybot_es8311_board_i2c_bus_handle(void) {
    return mybot_genjutech_i2c_bus_handle();
}

int mybot_es8311_board_set_speaker_power(bool enabled) {
    return mybot_genjutech_set_speaker_power(enabled);
}

static const mybot_board_t s_board = {
    .id = MYBOT_BOARD_NAME,
    .hw_model = MYBOT_BOARD_NAME,
    .prepare = board_prepare,
    .register_platform = board_register_platform,
    .ensure_network = board_ensure_network,
    .provision_wifi = board_provision_wifi,
    .shutdown_network = board_shutdown_network,
};

const mybot_board_t *mybot_board_get(void) {
    return &s_board;
}

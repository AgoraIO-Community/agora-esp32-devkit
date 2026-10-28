/* SPDX-License-Identifier: Apache-2.0 */
#include "generic_board.h"
#include "generic_hardware.h"

#include <mybot/platform/mybot_platform.h>

#include "announcement/embedded_ogg_prompt.h"
#include "board_config.h"
#include "esp_log.h"
#include "network/wifi_control.h"
#include "platform/board_actions.h"

#include <stdbool.h>

#define TAG "generic_board"

#if MYBOT_GENERIC_AUDIO_KIND == 1 || MYBOT_GENERIC_AUDIO_KIND == 4 || MYBOT_GENERIC_AUDIO_KIND == 5
const mybot_audio_capture_ops_t *mybot_es8311_audio_capture_ops(void);
const mybot_audio_playback_ops_t *mybot_es8311_audio_playback_ops(void);
const mybot_audio_volume_ops_t *mybot_es8311_audio_volume_ops(void);
#define CAPTURE_OPS mybot_es8311_audio_capture_ops
#define PLAYBACK_OPS mybot_es8311_audio_playback_ops
#define VOLUME_OPS mybot_es8311_audio_volume_ops
#elif MYBOT_GENERIC_AUDIO_KIND == 2
const mybot_audio_capture_ops_t *mybot_esp32s3_audio_capture_ops(void);
const mybot_audio_playback_ops_t *mybot_esp32s3_audio_playback_ops(void);
const mybot_audio_volume_ops_t *mybot_esp32s3_audio_volume_ops(void);
#define CAPTURE_OPS mybot_esp32s3_audio_capture_ops
#define PLAYBACK_OPS mybot_esp32s3_audio_playback_ops
#define VOLUME_OPS mybot_esp32s3_audio_volume_ops
#elif MYBOT_GENERIC_AUDIO_KIND == 3
const mybot_audio_capture_ops_t *mybot_amoled175_audio_capture_ops(void);
const mybot_audio_playback_ops_t *mybot_amoled175_audio_playback_ops(void);
const mybot_audio_volume_ops_t *mybot_amoled175_audio_volume_ops(void);
#define CAPTURE_OPS mybot_amoled175_audio_capture_ops
#define PLAYBACK_OPS mybot_amoled175_audio_playback_ops
#define VOLUME_OPS mybot_amoled175_audio_volume_ops
#else
#error "MYBOT_GENERIC_AUDIO_KIND must select an audio path"
#endif

const mybot_announce_ops_t *mybot_esp32s3_announce_ops(void);
const mybot_https_ops_t *mybot_esp32s3_https_ops(void);
const mybot_kv_store_ops_t *mybot_esp32s3_kv_store_ops(void);
const mybot_wifi_ops_t *mybot_esp32s3_wifi_ops(void);

#if MYBOT_GENERIC_TOUCH_KIND
const mybot_key_ops_t *mybot_cst816s_input_ops(void);
int mybot_cst816s_input_start(void);
int mybot_cst816s_input_stop(void);
#define KEY_OPS mybot_cst816s_input_ops
#else
const mybot_key_ops_t *mybot_esp32s3_button_ops(void);
int mybot_esp32s3_buttons_start(void);
#define KEY_OPS mybot_esp32s3_button_ops
#endif

#if MYBOT_BOARD_HAS_DISPLAY
const mybot_lcd_ops_t *mybot_shared_lvgl_ops(void);
static const mybot_lcd_ops_t *s_lcd_ops;
static void *s_lcd_context;
#endif
static bool s_automatic_provisioning_started;
static bool s_prepared;
static bool s_cleanup_pending;

static int board_show_screen(mybot_lcd_screen_t screen) {
#if MYBOT_BOARD_HAS_DISPLAY
    if (s_lcd_ops && s_lcd_context) {
        const mybot_lcd_content_t content = {.screen = screen};
        return s_lcd_ops->render(s_lcd_context, &content);
    }
#else
    (void)screen;
#endif
    return 0;
}

static void board_on_provisioning(const char *trigger) {
    (void)board_show_screen(MYBOT_LCD_SCREEN_WIFI_PROVISIONING);
    (void)mybot_embedded_ogg_play_wifi_provisioning(PLAYBACK_OPS(), VOLUME_OPS(), trigger);
}

static void board_on_automatic_provisioning(void) {
    s_automatic_provisioning_started = true;
    board_on_provisioning("automatic");
}

static void board_on_input_provisioning(void) {
    board_on_provisioning("input");
}

static int board_rollback_prepare(void) {
    s_cleanup_pending = true;
#if MYBOT_GENERIC_TOUCH_KIND
    if (mybot_cst816s_input_stop() < 0) {
        return -1;
    }
#endif
#if MYBOT_BOARD_HAS_DISPLAY
    if (s_lcd_ops && s_lcd_context) {
        s_lcd_ops->destroy(s_lcd_context);
        s_lcd_context = NULL;
    }
    s_lcd_ops = NULL;
#endif
    if (mybot_generic_hardware_deinit() < 0) {
        return -1;
    }
    s_cleanup_pending = false;
    return 0;
}

static int board_prepare(void) {
    if (s_prepared) {
        return 0;
    }
    if (s_cleanup_pending && board_rollback_prepare() < 0) {
        return -1;
    }
    if (mybot_generic_hardware_init() < 0) {
        return -1;
    }
#if MYBOT_BOARD_HAS_DISPLAY
    s_lcd_ops = mybot_shared_lvgl_ops();
    if (!s_lcd_ops || s_lcd_ops->init(&s_lcd_context) < 0 ||
        board_show_screen(MYBOT_LCD_SCREEN_STARTING) < 0) {
        (void)board_rollback_prepare();
        return -1;
    }
#endif
#if MYBOT_GENERIC_TOUCH_KIND
    if (mybot_cst816s_input_start() < 0) {
        (void)board_rollback_prepare();
        return -1;
    }
#else
    if (mybot_esp32s3_buttons_start() < 0) {
        (void)board_rollback_prepare();
        return -1;
    }
#endif
    s_prepared = true;
    return 0;
}

int mybot_board_handle_boot_long_press(void) {
    mybot_board_request_wifi_provisioning();
    return 0;
}

static int board_provision_wifi(void) {
    return mybot_wifi_run_provisioning(board_on_input_provisioning);
}

static int board_ensure_network(const char *device_id) {
    s_automatic_provisioning_started = false;
    const int result = mybot_wifi_ensure_network(device_id, board_on_automatic_provisioning);
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
        .key = KEY_OPS(),
        .audio_capture = CAPTURE_OPS(),
        .audio_playback = PLAYBACK_OPS(),
        .audio_volume = VOLUME_OPS(),
        .announce = mybot_esp32s3_announce_ops(),
        .https = mybot_esp32s3_https_ops(),
#if MYBOT_BOARD_HAS_DISPLAY
        .lcd = mybot_shared_lvgl_ops(),
#else
        .lcd = NULL,
#endif
    };
    return mybot_platform_register(&descriptor);
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

const mybot_board_t *mybot_generic_board_get(void) {
    return &s_board;
}

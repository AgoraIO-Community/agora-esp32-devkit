/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
#include "esp_err.h"
#include <stdbool.h>
#define CAM_CTLR_DATA_WIDTH_8 8
typedef struct {
    struct {
        bool init_sccb;
        void *i2c_handle;
        int freq;
    } sccb_config;
    int reset_pin, pwdn_pin;
    struct {
        int data_width, data_io[8], vsync_io, de_io, pclk_io, xclk_io;
    } dvp_pin;
    int xclk_freq;
} esp_video_init_dvp_config_t;
typedef struct {
    const esp_video_init_dvp_config_t *dvp;
} esp_video_init_config_t;
esp_err_t esp_video_init(const esp_video_init_config_t *config);
esp_err_t esp_video_deinit(void);

/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
#include <stdint.h>
#include <stdbool.h>
typedef void *jpeg_enc_handle_t;
typedef struct {
    unsigned int width, height, src_type, quality;
    bool task_enable;
} jpeg_enc_config_t;
#define DEFAULT_JPEG_ENC_CONFIG() ((jpeg_enc_config_t){0})
#define JPEG_ERR_OK 0
#define JPEG_PIXEL_FORMAT_YCbYCr 1
int jpeg_enc_open(const jpeg_enc_config_t *config, jpeg_enc_handle_t *encoder);
int jpeg_enc_close(jpeg_enc_handle_t encoder);
int jpeg_enc_set_quality(jpeg_enc_handle_t encoder, unsigned int quality);
int jpeg_enc_process(jpeg_enc_handle_t encoder, const uint8_t *input, int bytes, uint8_t *output,
                     int capacity, int *written);

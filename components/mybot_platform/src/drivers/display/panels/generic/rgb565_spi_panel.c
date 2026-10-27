/* SPDX-License-Identifier: Apache-2.0 */
#include "board_config.h"
#include "display/board_backlight.h"
#include "display/display_panel.h"

#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_lcd_io_spi.h"
#include "esp_log.h"

#if MYBOT_PANEL_CONTROLLER == 1
#include "esp_lcd_gc9a01.h"
#elif MYBOT_PANEL_CONTROLLER == 2
#include "esp_lcd_nv3023.h"
#elif MYBOT_PANEL_CONTROLLER == 3
#include "esp_lcd_ili9341.h"
#else
#error "Unsupported MYBOT_PANEL_CONTROLLER"
#endif

#include <stddef.h>
#include <stdint.h>

#define TAG "generic_spi_panel"

#ifndef MYBOT_DISPLAY_RESET_ACTIVE_HIGH
#define MYBOT_DISPLAY_RESET_ACTIVE_HIGH 0
#endif

#if MYBOT_PANEL_CONTROLLER == 3
#ifndef MYBOT_ILI9341_PROFILE
#define MYBOT_ILI9341_PROFILE 0
#endif

#if MYBOT_ILI9341_PROFILE == 1
static const ili9341_lcd_init_cmd_t s_ili9341_profile_1[] = {
    {0xC8, (uint8_t[]){0xFF, 0x93, 0x42}, 3, 0},
    {0xC0, (uint8_t[]){0x0E, 0x0E}, 2, 0},
    {0xC5, (uint8_t[]){0xD0}, 1, 0},
    {0xC1, (uint8_t[]){0x02}, 1, 0},
    {0xB4, (uint8_t[]){0x02}, 1, 0},
    {0xE0,
     (uint8_t[]){0x00, 0x03, 0x08, 0x06, 0x13, 0x09, 0x39, 0x39, 0x48, 0x02, 0x0A, 0x08, 0x17, 0x17,
                 0x0F},
     15, 0},
    {0xE1,
     (uint8_t[]){0x00, 0x28, 0x29, 0x01, 0x0D, 0x03, 0x3F, 0x33, 0x52, 0x04, 0x0F, 0x0E, 0x37, 0x38,
                 0x0F},
     15, 0},
    {0xB1, (uint8_t[]){0x00, 0x1B}, 2, 0},
    {0x36, (uint8_t[]){0x08}, 1, 0},
    {0x3A, (uint8_t[]){0x55}, 1, 0},
    {0xB7, (uint8_t[]){0x06}, 1, 0},
    {0x11, NULL, 0, 128},
    {0x29, NULL, 0, 128},
};
#elif MYBOT_ILI9341_PROFILE == 2
static const ili9341_lcd_init_cmd_t s_ili9341_profile_2[] = {
    {0xCF, (uint8_t[]){0x00, 0xC1, 0x30}, 3, 0},
    {0xED, (uint8_t[]){0x64, 0x03, 0x12, 0x81}, 4, 0},
    {0xE8, (uint8_t[]){0x85, 0x00, 0x78}, 3, 0},
    {0xCB, (uint8_t[]){0x39, 0x2C, 0x00, 0x34, 0x02}, 5, 0},
    {0xF7, (uint8_t[]){0x20}, 1, 0},
    {0xEA, (uint8_t[]){0x00, 0x00}, 2, 0},
    {0xC0, (uint8_t[]){0x13}, 1, 0},
    {0xC1, (uint8_t[]){0x13}, 1, 0},
    {0xC5, (uint8_t[]){0x22, 0x35}, 2, 0},
    {0xC7, (uint8_t[]){0xBD}, 1, 0},
    {0x36, (uint8_t[]){0x08}, 1, 0},
    {0xB6, (uint8_t[]){0x0A, 0xA2}, 2, 0},
    {0x3A, (uint8_t[]){0x55}, 1, 0},
    {0xF6, (uint8_t[]){0x01, 0x30}, 2, 0},
    {0xB1, (uint8_t[]){0x00, 0x1B}, 2, 0},
    {0xF2, (uint8_t[]){0x00}, 1, 0},
    {0x26, (uint8_t[]){0x01}, 1, 0},
    {0xE0,
     (uint8_t[]){0x0F, 0x35, 0x31, 0x0B, 0x0E, 0x06, 0x49, 0xA7, 0x33, 0x07, 0x0F, 0x03, 0x0C, 0x0A,
                 0x00},
     15, 0},
    {0xE1,
     (uint8_t[]){0x00, 0x0A, 0x0F, 0x04, 0x11, 0x08, 0x36, 0x58, 0x4D, 0x07, 0x10, 0x0C, 0x32, 0x34,
                 0x0F},
     15, 0},
    {0x11, NULL, 0, 0},
    {0x29, NULL, 0, 0},
};
#endif
#endif

int mybot_display_panel_close(mybot_display_panel_t *panel) {
    if (!panel) {
        return -1;
    }
    if (panel->backlight_ready && mybot_board_set_display_backlight(0) < 0) {
        return -1;
    }
    panel->backlight_ready = false;
    if (panel->panel) {
        if (panel->ready && esp_lcd_panel_disp_on_off(panel->panel, false) != ESP_OK) {
            ESP_LOGW(TAG, "event=panel action=display_off result=error");
        }
        if (esp_lcd_panel_del(panel->panel) != ESP_OK) {
            return -1;
        }
        panel->panel = NULL;
    }
    if (panel->io) {
        if (esp_lcd_panel_io_del(panel->io) != ESP_OK) {
            return -1;
        }
        panel->io = NULL;
    }
    if (panel->spi_ready) {
        if (spi_bus_free(MYBOT_DISPLAY_SPI_HOST) != ESP_OK) {
            return -1;
        }
        panel->spi_ready = false;
    }
    panel->ready = false;
    panel->x_alignment = 0;
    panel->y_alignment = 0;
    return 0;
}

int mybot_display_panel_open(mybot_display_panel_t *panel) {
    if (!panel || panel->ready || mybot_display_panel_close(panel) < 0 ||
        mybot_board_set_display_backlight(0) < 0) {
        return -1;
    }
    panel->backlight_ready = true;

    const spi_bus_config_t bus_config = {
        .mosi_io_num = MYBOT_DISPLAY_MOSI,
        .miso_io_num = GPIO_NUM_NC,
        .sclk_io_num = MYBOT_DISPLAY_SCLK,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = MYBOT_DISPLAY_WIDTH * MYBOT_DISPLAY_TRANSFER_ROWS * sizeof(uint16_t),
    };
    esp_err_t result = spi_bus_initialize(MYBOT_DISPLAY_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO);
    if (result != ESP_OK) {
        goto fail;
    }
    panel->spi_ready = true;

    const esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = MYBOT_DISPLAY_CS,
        .dc_gpio_num = MYBOT_DISPLAY_DC,
        .spi_mode = MYBOT_DISPLAY_SPI_MODE,
        .pclk_hz = MYBOT_DISPLAY_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 2,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    result = esp_lcd_new_panel_io_spi(MYBOT_DISPLAY_SPI_HOST, &io_config, &panel->io);
    if (result != ESP_OK) {
        goto fail;
    }

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = MYBOT_DISPLAY_RESET,
        .rgb_ele_order = MYBOT_DISPLAY_RGB_ORDER,
        .bits_per_pixel = 16,
        .flags.reset_active_high = MYBOT_DISPLAY_RESET_ACTIVE_HIGH,
    };
#if MYBOT_PANEL_CONTROLLER == 1
    result = esp_lcd_new_panel_gc9a01(panel->io, &panel_config, &panel->panel);
#elif MYBOT_PANEL_CONTROLLER == 2
    result = esp_lcd_new_panel_nv3023(panel->io, &panel_config, &panel->panel);
#else
#if MYBOT_ILI9341_PROFILE == 1
    const ili9341_vendor_config_t vendor_config = {
        .init_cmds = s_ili9341_profile_1,
        .init_cmds_size = sizeof(s_ili9341_profile_1) / sizeof(s_ili9341_profile_1[0]),
    };
    panel_config.vendor_config = (void *)&vendor_config;
#elif MYBOT_ILI9341_PROFILE == 2
    const ili9341_vendor_config_t vendor_config = {
        .init_cmds = s_ili9341_profile_2,
        .init_cmds_size = sizeof(s_ili9341_profile_2) / sizeof(s_ili9341_profile_2[0]),
    };
    panel_config.vendor_config = (void *)&vendor_config;
#endif
    result = esp_lcd_new_panel_ili9341(panel->io, &panel_config, &panel->panel);
#endif
    if (result == ESP_OK) {
        result = esp_lcd_panel_reset(panel->panel);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_init(panel->panel);
    }
    if (result == ESP_OK) {
        result =
            esp_lcd_panel_set_gap(panel->panel, MYBOT_DISPLAY_OFFSET_X, MYBOT_DISPLAY_OFFSET_Y);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_invert_color(panel->panel, MYBOT_DISPLAY_INVERT_COLOR);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_swap_xy(panel->panel, MYBOT_DISPLAY_SWAP_XY);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_mirror(panel->panel, MYBOT_DISPLAY_MIRROR_X, MYBOT_DISPLAY_MIRROR_Y);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_disp_on_off(panel->panel, true);
    }
    if (result == ESP_OK) {
        result = mybot_board_set_display_backlight(MYBOT_DISPLAY_BRIGHTNESS_PERCENT) == 0
                     ? ESP_OK
                     : ESP_FAIL;
    }
    if (result != ESP_OK) {
        goto fail;
    }
    panel->ready = true;
    panel->x_alignment = 1;
    panel->y_alignment = 1;
    return 0;

fail:
    ESP_LOGE(TAG, "event=panel action=initialize result=error code=%s", esp_err_to_name(result));
    (void)mybot_display_panel_close(panel);
    return -1;
}

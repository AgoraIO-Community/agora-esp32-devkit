/* SPDX-License-Identifier: Apache-2.0 */
#include "board_config.h"
#include "display/board_backlight.h"
#include "display/display_panel.h"

#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_lcd_io_spi.h"
#include "esp_log.h"

#if MYBOT_PANEL_CONTROLLER == 1 || MYBOT_PANEL_CONTROLLER == 4 || MYBOT_PANEL_CONTROLLER == 5
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

#if MYBOT_PANEL_CONTROLLER == 4
static const gc9a01_lcd_init_cmd_t s_gc9d01n_init[] = {
    {0xFE, (uint8_t[]){0x00}, 0, 0},
    {0xEF, (uint8_t[]){0x00}, 0, 0},
    {0x80, (uint8_t[]){0xFF}, 1, 0},
    {0x81, (uint8_t[]){0xFF}, 1, 0},
    {0x82, (uint8_t[]){0xFF}, 1, 0},
    {0x84, (uint8_t[]){0xFF}, 1, 0},
    {0x85, (uint8_t[]){0xFF}, 1, 0},
    {0x86, (uint8_t[]){0xFF}, 1, 0},
    {0x87, (uint8_t[]){0xFF}, 1, 0},
    {0x88, (uint8_t[]){0xFF}, 1, 0},
    {0x89, (uint8_t[]){0xFF}, 1, 0},
    {0x8A, (uint8_t[]){0xFF}, 1, 0},
    {0x8B, (uint8_t[]){0xFF}, 1, 0},
    {0x8C, (uint8_t[]){0xFF}, 1, 0},
    {0x8D, (uint8_t[]){0xFF}, 1, 0},
    {0x8E, (uint8_t[]){0xFF}, 1, 0},
    {0x8F, (uint8_t[]){0xFF}, 1, 0},
    {0x3A, (uint8_t[]){0x05}, 1, 0},
    {0xEC, (uint8_t[]){0x01}, 1, 0},
    {0x74, (uint8_t[]){0x02, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00}, 7, 0},
    {0x98, (uint8_t[]){0x3E}, 1, 0},
    {0x99, (uint8_t[]){0x3E}, 1, 0},
    {0xB5, (uint8_t[]){0x0D, 0x0D}, 2, 0},
    {0x60, (uint8_t[]){0x38, 0x0F, 0x79, 0x67}, 4, 0},
    {0x61, (uint8_t[]){0x38, 0x11, 0x79, 0x67}, 4, 0},
    {0x64, (uint8_t[]){0x38, 0x17, 0x71, 0x5F, 0x79, 0x67}, 6, 0},
    {0x65, (uint8_t[]){0x38, 0x13, 0x71, 0x5B, 0x79, 0x67}, 6, 0},
    {0x6A, (uint8_t[]){0x00, 0x00}, 2, 0},
    {0x6C, (uint8_t[]){0x22, 0x02, 0x22, 0x02, 0x22, 0x22, 0x50}, 7, 0},
    {0x6E, (uint8_t[]){0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 0x0F, 0x0F, 0x0D, 0x0D, 0x0B,
                       0x0B, 0x09, 0x09, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0C, 0x0C,
                       0x0E, 0x0E, 0x10, 0x10, 0x00, 0x00, 0x02, 0x02, 0x04, 0x04},
     32, 0},
    {0xBF, (uint8_t[]){0x01}, 1, 0},
    {0xF9, (uint8_t[]){0x40}, 1, 0},
    {0x9B, (uint8_t[]){0x3B, 0x93, 0x33, 0x7F, 0x00}, 5, 0},
    {0x7E, (uint8_t[]){0x30}, 1, 0},
    {0x70, (uint8_t[]){0x0D, 0x02, 0x08, 0x0D, 0x02, 0x08}, 6, 0},
    {0x71, (uint8_t[]){0x0D, 0x02, 0x08}, 3, 0},
    {0x91, (uint8_t[]){0x0E, 0x09}, 2, 0},
    {0xC3, (uint8_t[]){0x19, 0xC4, 0x19, 0xC9, 0x3C}, 5, 0},
    {0xF0, (uint8_t[]){0x53, 0x15, 0x0A, 0x04, 0x00, 0x3E}, 6, 0},
    {0xF1, (uint8_t[]){0x56, 0xA8, 0x7F, 0x33, 0x34, 0x5F}, 6, 0},
    {0xF2, (uint8_t[]){0x53, 0x15, 0x0A, 0x04, 0x00, 0x3A}, 6, 0},
    {0xF3, (uint8_t[]){0x52, 0xA4, 0x7F, 0x33, 0x34, 0xDF}, 6, 0},
    {0x36, (uint8_t[]){0x00}, 1, 0},
    {0x11, NULL, 0, 200},
    {0x29, NULL, 0, 0},
    {0x2C, NULL, 0, 20},
};
#elif MYBOT_PANEL_CONTROLLER == 5
static const gc9a01_lcd_init_cmd_t s_gc9309na_init[] = {
    {0xFE, NULL, 0, 0},
    {0xEF, NULL, 0, 0},
    {0x80, (uint8_t[]){0xC0}, 1, 0},
    {0x81, (uint8_t[]){0x01}, 1, 0},
    {0x82, (uint8_t[]){0x07}, 1, 0},
    {0x83, (uint8_t[]){0x38}, 1, 0},
    {0x88, (uint8_t[]){0x64}, 1, 0},
    {0x89, (uint8_t[]){0x86}, 1, 0},
    {0x8B, (uint8_t[]){0x3C}, 1, 0},
    {0x8D, (uint8_t[]){0x51}, 1, 0},
    {0x8E, (uint8_t[]){0x70}, 1, 0},
    {0xB4, (uint8_t[]){0x80}, 1, 0},
    {0x3A, (uint8_t[]){0x05}, 1, 0},
    {0x36, (uint8_t[]){0x48}, 1, 0},
    {0xBF, (uint8_t[]){0x1F}, 1, 0},
    {0x7D, (uint8_t[]){0x45, 0x06}, 2, 0},
    {0xEE, (uint8_t[]){0x00, 0x06}, 2, 0},
    {0xF4, (uint8_t[]){0x53}, 1, 0},
    {0xF6, (uint8_t[]){0x17, 0x08}, 2, 0},
    {0x70, (uint8_t[]){0x4F, 0x4F}, 2, 0},
    {0x71, (uint8_t[]){0x12, 0x20}, 2, 0},
    {0x72, (uint8_t[]){0x12, 0x20}, 2, 0},
    {0xB5, (uint8_t[]){0x50}, 1, 0},
    {0xBA, (uint8_t[]){0x00}, 1, 0},
    {0xEC, (uint8_t[]){0x71}, 1, 0},
    {0x7B, (uint8_t[]){0x00, 0x0D}, 2, 0},
    {0x7C, (uint8_t[]){0x0D, 0x03}, 2, 0},
    {0xF5, (uint8_t[]){0x02, 0x10, 0x12}, 3, 0},
    {0xF0, (uint8_t[]){0x0C, 0x11, 0x0B, 0x0A, 0x05, 0x32, 0x44, 0x8E, 0x9A, 0x29, 0x2E, 0x5F}, 12,
     0},
    {0xF1, (uint8_t[]){0x0B, 0x11, 0x0B, 0x07, 0x07, 0x32, 0x45, 0xBD, 0x8D, 0x21, 0x28, 0xAF}, 12,
     0},
    {0x2A, (uint8_t[]){0x00, 0x00, 0x00, 0xEF}, 4, 0},
    {0x2B, (uint8_t[]){0x00, 0x00, 0x01, 0x27}, 4, 0},
    {0x66, (uint8_t[]){0x2C}, 1, 0},
    {0x67, (uint8_t[]){0x18}, 1, 0},
    {0x68, (uint8_t[]){0x3E}, 1, 0},
    {0xCA, (uint8_t[]){0x0E}, 1, 0},
    {0xE8, (uint8_t[]){0xF0}, 1, 0},
    {0xCB, (uint8_t[]){0x06}, 1, 0},
    {0xB6, (uint8_t[]){0x5C, 0x40, 0x40}, 3, 0},
    {0xCC, (uint8_t[]){0x33}, 1, 0},
    {0xCD, (uint8_t[]){0x33}, 1, 0},
    {0x11, NULL, 0, 80},
    {0xE8, (uint8_t[]){0xA0}, 1, 0},
    {0xE8, (uint8_t[]){0xF0}, 1, 0},
    {0xFE, NULL, 0, 0},
    {0xEE, NULL, 0, 0},
    {0x29, NULL, 0, 0},
    {0x2C, NULL, 0, 10},
};
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
#elif MYBOT_PANEL_CONTROLLER == 3
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
#elif MYBOT_PANEL_CONTROLLER == 4
    const gc9a01_vendor_config_t vendor_config = {
        .init_cmds = s_gc9d01n_init,
        .init_cmds_size = sizeof(s_gc9d01n_init) / sizeof(s_gc9d01n_init[0]),
    };
    panel_config.vendor_config = (void *)&vendor_config;
    result = esp_lcd_new_panel_gc9a01(panel->io, &panel_config, &panel->panel);
#elif MYBOT_PANEL_CONTROLLER == 5
    const gc9a01_vendor_config_t vendor_config = {
        .init_cmds = s_gc9309na_init,
        .init_cmds_size = sizeof(s_gc9309na_init) / sizeof(s_gc9309na_init[0]),
    };
    panel_config.vendor_config = (void *)&vendor_config;
    result = esp_lcd_new_panel_gc9a01(panel->io, &panel_config, &panel->panel);
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

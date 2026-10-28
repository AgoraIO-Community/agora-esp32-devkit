/* SPDX-License-Identifier: Apache-2.0 */
#include "board_config.h"
#include "display/board_backlight.h"
#include "display/display_panel.h"

#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_lcd_gc9a01.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"

#include <stddef.h>
#include <stdint.h>

#define TAG "compact_128_panel"
#define MYBOT_COMPACT_PANEL_GC9107 1
#define MYBOT_COMPACT_PANEL_ST7735 2

#ifndef MYBOT_COMPACT_GC9107_OFFSET_X
#define MYBOT_COMPACT_GC9107_OFFSET_X 0
#endif
#ifndef MYBOT_COMPACT_GC9107_OFFSET_Y
#define MYBOT_COMPACT_GC9107_OFFSET_Y 0
#endif
#ifndef MYBOT_COMPACT_ST7735_OFFSET_X
#define MYBOT_COMPACT_ST7735_OFFSET_X 2
#endif
#ifndef MYBOT_COMPACT_ST7735_OFFSET_Y
#define MYBOT_COMPACT_ST7735_OFFSET_Y 3
#endif
#ifndef MYBOT_COMPACT_GC9107_RGB_ORDER
#define MYBOT_COMPACT_GC9107_RGB_ORDER LCD_RGB_ELEMENT_ORDER_RGB
#endif
#ifndef MYBOT_COMPACT_ST7735_RGB_ORDER
#define MYBOT_COMPACT_ST7735_RGB_ORDER LCD_RGB_ELEMENT_ORDER_BGR
#endif

int mybot_board_compact_panel_profile(void);

static const gc9a01_lcd_init_cmd_t s_gc9107_init[] = {
    {0xfe, (uint8_t[]){0x00}, 0, 0},
    {0xef, (uint8_t[]){0x00}, 0, 0},
    {0xb0, (uint8_t[]){0xc0}, 1, 0},
    {0xb1, (uint8_t[]){0x80}, 1, 0},
    {0xb2, (uint8_t[]){0x27}, 1, 0},
    {0xb3, (uint8_t[]){0x13}, 1, 0},
    {0xb6, (uint8_t[]){0x19}, 1, 0},
    {0xb7, (uint8_t[]){0x05}, 1, 0},
    {0xac, (uint8_t[]){0xc8}, 1, 0},
    {0xab, (uint8_t[]){0x0f}, 1, 0},
    {0x3a, (uint8_t[]){0x05}, 1, 0},
    {0xb4, (uint8_t[]){0x04}, 1, 0},
    {0xa8, (uint8_t[]){0x08}, 1, 0},
    {0xb8, (uint8_t[]){0x08}, 1, 0},
    {0xea, (uint8_t[]){0x02}, 1, 0},
    {0xe8, (uint8_t[]){0x2a}, 1, 0},
    {0xe9, (uint8_t[]){0x47}, 1, 0},
    {0xe7, (uint8_t[]){0x5f}, 1, 0},
    {0xc6, (uint8_t[]){0x21}, 1, 0},
    {0xc7, (uint8_t[]){0x15}, 1, 0},
    {0xf0,
     (uint8_t[]){0x1d, 0x38, 0x09, 0x4d, 0x92, 0x2f, 0x35, 0x52, 0x1e, 0x0c, 0x04, 0x12, 0x14,
                 0x1f},
     14, 0},
    {0xf1,
     (uint8_t[]){0x16, 0x40, 0x1c, 0x54, 0xa9, 0x2d, 0x2e, 0x56, 0x10, 0x0d, 0x0c, 0x1a, 0x14,
                 0x1e},
     14, 0},
    {0xf4, (uint8_t[]){0x00, 0x00, 0xff}, 3, 0},
    {0xba, (uint8_t[]){0xff, 0xff}, 2, 0},
};

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

    const int profile = mybot_board_compact_panel_profile();
    if (profile != MYBOT_COMPACT_PANEL_GC9107 && profile != MYBOT_COMPACT_PANEL_ST7735) {
        result = ESP_ERR_INVALID_STATE;
        goto fail;
    }

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

    const bool gc9107 = profile == MYBOT_COMPACT_PANEL_GC9107;
    const gc9a01_vendor_config_t vendor_config = {
        .init_cmds = s_gc9107_init,
        .init_cmds_size = sizeof(s_gc9107_init) / sizeof(s_gc9107_init[0]),
    };
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = MYBOT_DISPLAY_RESET,
        .rgb_ele_order = gc9107 ? MYBOT_COMPACT_GC9107_RGB_ORDER : MYBOT_COMPACT_ST7735_RGB_ORDER,
        .bits_per_pixel = 16,
    };
    if (gc9107) {
        panel_config.vendor_config = (void *)&vendor_config;
        result = esp_lcd_new_panel_gc9a01(panel->io, &panel_config, &panel->panel);
    } else {
        result = esp_lcd_new_panel_st7789(panel->io, &panel_config, &panel->panel);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_reset(panel->panel);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_init(panel->panel);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_set_gap(
            panel->panel, gc9107 ? MYBOT_COMPACT_GC9107_OFFSET_X : MYBOT_COMPACT_ST7735_OFFSET_X,
            gc9107 ? MYBOT_COMPACT_GC9107_OFFSET_Y : MYBOT_COMPACT_ST7735_OFFSET_Y);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_invert_color(panel->panel, !gc9107);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_swap_xy(panel->panel, false);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_mirror(panel->panel, !gc9107, !gc9107);
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
    ESP_LOGI(TAG, "event=panel action=initialize profile=%s result=ok",
             gc9107 ? "gc9107" : "st7735");
    return 0;

fail:
    ESP_LOGE(TAG, "event=panel action=initialize result=error code=%s", esp_err_to_name(result));
    (void)mybot_display_panel_close(panel);
    return -1;
}

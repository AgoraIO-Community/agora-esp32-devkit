/* SPDX-License-Identifier: Apache-2.0 */
#include "board_config.h"
#include "display/display_panel.h"

#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_lcd_co5300.h"
#include "esp_lcd_io_spi.h"
#include "esp_log.h"

#include <stddef.h>
#include <stdint.h>

#define TAG "co5300_amoled18"
#define BRIGHTNESS_PERCENT 60

_Static_assert((MYBOT_DISPLAY_WIDTH % 2) == 0, "CO5300 transfers require an even width");
_Static_assert((MYBOT_DISPLAY_HEIGHT % 2) == 0, "CO5300 transfers require an even height");
_Static_assert((MYBOT_DISPLAY_TRANSFER_ROWS % 2) == 0, "CO5300 transfer rows must be even");

static const co5300_lcd_init_cmd_t s_vendor_init[] = {
    {0x11, NULL, 0, 600},
    {0xFE, (uint8_t[]){0x20}, 1, 0},
    {0x19, (uint8_t[]){0x10}, 1, 0},
    {0x1C, (uint8_t[]){0xA0}, 1, 0},
    {0xFE, (uint8_t[]){0x00}, 1, 0},
    {0xC4, (uint8_t[]){0x80}, 1, 0},
    {0x3A, (uint8_t[]){0x55}, 1, 0},
    {0x35, (uint8_t[]){0x00}, 1, 0},
    {0x53, (uint8_t[]){0x20}, 1, 0},
    {0x51, (uint8_t[]){0xFF}, 1, 0},
    {0x63, (uint8_t[]){0xFF}, 1, 0},
    {0x2A, (uint8_t[]){0x00, 0x00, 0x01, 0xDF}, 4, 0},
    {0x2B, (uint8_t[]){0x00, 0x00, 0x01, 0xDF}, 4, 0},
    {0x36, (uint8_t[]){0x00}, 1, 0},
    {0x29, NULL, 0, 600},
};

int mybot_display_panel_close(mybot_display_panel_t *panel) {
    if (!panel) {
        return -1;
    }
    if (panel->panel && esp_lcd_panel_co5300_set_brightness(panel->panel, 0) != ESP_OK) {
        ESP_LOGW(TAG, "event=panel action=brightness_off result=error");
    }
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
    if (!panel || panel->ready || mybot_display_panel_close(panel) < 0) {
        return -1;
    }

    const spi_bus_config_t bus_config = {
        .sclk_io_num = MYBOT_DISPLAY_PCLK,
        .data0_io_num = MYBOT_DISPLAY_DATA0,
        .data1_io_num = MYBOT_DISPLAY_DATA1,
        .data2_io_num = MYBOT_DISPLAY_DATA2,
        .data3_io_num = MYBOT_DISPLAY_DATA3,
        .max_transfer_sz = MYBOT_DISPLAY_WIDTH * MYBOT_DISPLAY_TRANSFER_ROWS * sizeof(uint16_t),
        .flags = SPICOMMON_BUSFLAG_QUAD,
    };
    esp_err_t result = spi_bus_initialize(MYBOT_DISPLAY_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO);
    if (result != ESP_OK) {
        goto fail;
    }
    panel->spi_ready = true;

    const esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = MYBOT_DISPLAY_CS,
        .dc_gpio_num = GPIO_NUM_NC,
        .spi_mode = 0,
        .pclk_hz = MYBOT_DISPLAY_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 2,
        .lcd_cmd_bits = 32,
        .lcd_param_bits = 8,
        .flags.quad_mode = true,
    };
    result = esp_lcd_new_panel_io_spi(MYBOT_DISPLAY_SPI_HOST, &io_config, &panel->io);
    if (result != ESP_OK) {
        goto fail;
    }

    const co5300_vendor_config_t vendor_config = {
        .init_cmds = s_vendor_init,
        .init_cmds_size = sizeof(s_vendor_init) / sizeof(s_vendor_init[0]),
        .flags.use_qspi_interface = true,
    };
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = MYBOT_DISPLAY_RESET,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = (void *)&vendor_config,
    };
    result = esp_lcd_new_panel_co5300(panel->io, &panel_config, &panel->panel);
    if (result == ESP_OK) {
        result =
            esp_lcd_panel_set_gap(panel->panel, MYBOT_DISPLAY_OFFSET_X, MYBOT_DISPLAY_OFFSET_Y);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_reset(panel->panel);
    }
    if (result == ESP_OK) {
        result = esp_lcd_panel_init(panel->panel);
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
        result = esp_lcd_panel_co5300_set_brightness(panel->panel, BRIGHTNESS_PERCENT);
    }
    if (result != ESP_OK) {
        goto fail;
    }

    panel->ready = true;
    panel->x_alignment = 2;
    panel->y_alignment = 2;
    return 0;

fail:
    ESP_LOGE(TAG, "event=panel action=initialize result=error code=%s", esp_err_to_name(result));
    (void)mybot_display_panel_close(panel);
    return -1;
}

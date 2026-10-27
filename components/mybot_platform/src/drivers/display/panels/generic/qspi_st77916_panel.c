/* SPDX-License-Identifier: Apache-2.0 */
#include "board_config.h"
#include "display/board_backlight.h"
#include "display/display_panel.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_st77916.h"
#include "esp_log.h"

#include <stddef.h>
#include <stdint.h>

#define TAG "st77916_qspi_panel"

#ifndef MYBOT_DISPLAY_RESET_ACTIVE_HIGH
#define MYBOT_DISPLAY_RESET_ACTIVE_HIGH 0
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

    const st77916_vendor_config_t vendor_config = {
        .flags.use_qspi_interface = true,
    };
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = MYBOT_DISPLAY_RESET,
        .rgb_ele_order = MYBOT_DISPLAY_RGB_ORDER,
        .bits_per_pixel = 16,
        .flags.reset_active_high = MYBOT_DISPLAY_RESET_ACTIVE_HIGH,
        .vendor_config = (void *)&vendor_config,
    };
    result = esp_lcd_new_panel_st77916(panel->io, &panel_config, &panel->panel);
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
        result = mybot_board_set_display_backlight(MYBOT_DISPLAY_BRIGHTNESS_PERCENT) == 0
                     ? ESP_OK
                     : ESP_FAIL;
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

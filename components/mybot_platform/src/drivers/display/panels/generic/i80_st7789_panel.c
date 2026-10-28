/* SPDX-License-Identifier: Apache-2.0 */
#include "board_config.h"
#include "display/board_backlight.h"
#include "display/display_panel.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"

#include <stddef.h>
#include <stdint.h>

#define TAG "i80_st7789_panel"

#ifndef MYBOT_DISPLAY_I80_RD
#define MYBOT_DISPLAY_I80_RD GPIO_NUM_NC
#endif

#ifndef MYBOT_DISPLAY_ST7789_POST_INIT
#define MYBOT_DISPLAY_ST7789_POST_INIT 0
#endif

static esp_lcd_i80_bus_handle_t s_i80_bus;

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
    if (s_i80_bus) {
        if (esp_lcd_del_i80_bus(s_i80_bus) != ESP_OK) {
            return -1;
        }
        s_i80_bus = NULL;
    }
    panel->ready = false;
    panel->x_alignment = 0;
    panel->y_alignment = 0;
    return 0;
}

static esp_err_t prepare_rd_pin(void) {
    if (MYBOT_DISPLAY_I80_RD == GPIO_NUM_NC) {
        return ESP_OK;
    }
    const gpio_config_t config = {
        .pin_bit_mask = 1ULL << MYBOT_DISPLAY_I80_RD,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    const esp_err_t result = gpio_config(&config);
    return result == ESP_OK ? gpio_set_level(MYBOT_DISPLAY_I80_RD, 1) : result;
}

static esp_err_t send_post_init_commands(mybot_display_panel_t *panel) {
#if MYBOT_DISPLAY_ST7789_POST_INIT
    const uint8_t madctl = MYBOT_DISPLAY_ST7789_MADCTL;
    const uint8_t colmod = MYBOT_DISPLAY_ST7789_COLMOD;
    esp_err_t result = esp_lcd_panel_io_tx_param(panel->io, 0x36, &madctl, 1);
    if (result == ESP_OK) {
        result = esp_lcd_panel_io_tx_param(panel->io, 0x3A, &colmod, 1);
    }
    return result;
#else
    (void)panel;
    return ESP_OK;
#endif
}

int mybot_display_panel_open(mybot_display_panel_t *panel) {
    if (!panel || panel->ready || mybot_display_panel_close(panel) < 0 ||
        mybot_board_set_display_backlight(0) < 0) {
        return -1;
    }
    panel->backlight_ready = true;

    esp_err_t result = prepare_rd_pin();
    if (result != ESP_OK) {
        goto fail;
    }
    const esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = MYBOT_DISPLAY_DC,
        .wr_gpio_num = MYBOT_DISPLAY_SCLK,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums =
            {
                MYBOT_DISPLAY_I80_D0,
                MYBOT_DISPLAY_I80_D1,
                MYBOT_DISPLAY_I80_D2,
                MYBOT_DISPLAY_I80_D3,
                MYBOT_DISPLAY_I80_D4,
                MYBOT_DISPLAY_I80_D5,
                MYBOT_DISPLAY_I80_D6,
                MYBOT_DISPLAY_I80_D7,
            },
        .bus_width = 8,
        .max_transfer_bytes = MYBOT_DISPLAY_WIDTH * MYBOT_DISPLAY_HEIGHT * sizeof(uint16_t),
        .dma_burst_size = 64,
    };
    result = esp_lcd_new_i80_bus(&bus_config, &s_i80_bus);
    if (result != ESP_OK) {
        goto fail;
    }

    const esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = MYBOT_DISPLAY_CS,
        .pclk_hz = MYBOT_DISPLAY_I80_CLOCK_HZ,
        .trans_queue_depth = 2,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_levels =
            {
                .dc_idle_level = 0,
                .dc_cmd_level = 0,
                .dc_dummy_level = 0,
                .dc_data_level = 1,
            },
    };
    result = esp_lcd_new_panel_io_i80(s_i80_bus, &io_config, &panel->io);
    if (result != ESP_OK) {
        goto fail;
    }

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = MYBOT_DISPLAY_RESET,
        .rgb_ele_order = MYBOT_DISPLAY_RGB_ORDER,
        .bits_per_pixel = 16,
    };
    result = esp_lcd_new_panel_st7789(panel->io, &panel_config, &panel->panel);
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
        result = send_post_init_commands(panel);
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
    ESP_LOGE(TAG, "event=panel action=initialize bus=i80 result=error code=%s",
             esp_err_to_name(result));
    (void)mybot_display_panel_close(panel);
    return -1;
}

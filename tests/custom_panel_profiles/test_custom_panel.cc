/* SPDX-License-Identifier: MIT */
#include "board_config.h"
#include "display/display_panel.h"

#include <cstdio>
#include <cstdlib>

#ifndef EXPECTED_CONTROLLER
#error "EXPECTED_CONTROLLER must select the custom panel profile"
#endif

#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);              \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

struct host_panel_io {
    bool open = false;
};
struct host_panel {
    bool open = false;
    int gap_x = -1;
    int gap_y = -1;
};

static host_panel_io io;
static host_panel panel;
static bool spi_open;
static int backlight;
static unsigned int panel_deletes;
static unsigned int io_deletes;
static unsigned int spi_frees;
static bool display_on_seen;

extern "C" {
const char *esp_err_to_name(esp_err_t error) {
    return error == ESP_OK ? "ESP_OK" : "ESP_FAIL";
}

int mybot_board_set_display_backlight(unsigned int percent) {
    CHECK(percent <= 100);
    backlight = static_cast<int>(percent);
    return 0;
}

esp_err_t spi_bus_initialize(int host, const spi_bus_config_t *config, int dma) {
    CHECK(host == MYBOT_DISPLAY_SPI_HOST && dma == SPI_DMA_CH_AUTO && !spi_open);
    CHECK(config->mosi_io_num == MYBOT_DISPLAY_MOSI);
    CHECK(config->sclk_io_num == MYBOT_DISPLAY_SCLK);
    CHECK(config->miso_io_num == GPIO_NUM_NC);
    spi_open = true;
    return ESP_OK;
}

esp_err_t spi_bus_free(int host) {
    CHECK(host == MYBOT_DISPLAY_SPI_HOST && spi_open && !io.open && !panel.open);
    spi_open = false;
    ++spi_frees;
    return ESP_OK;
}

esp_err_t esp_lcd_new_panel_io_spi(esp_lcd_spi_bus_handle_t host,
                                   const esp_lcd_panel_io_spi_config_t *config,
                                   esp_lcd_panel_io_handle_t *out) {
    CHECK(host == MYBOT_DISPLAY_SPI_HOST && spi_open && !io.open);
    CHECK(config->cs_gpio_num == MYBOT_DISPLAY_CS);
    CHECK(config->dc_gpio_num == MYBOT_DISPLAY_DC);
    CHECK(config->spi_mode == MYBOT_DISPLAY_SPI_MODE);
    CHECK(config->pclk_hz == MYBOT_DISPLAY_PIXEL_CLOCK_HZ);
    io = {};
    io.open = true;
    *out = &io;
    return ESP_OK;
}

esp_err_t esp_lcd_new_panel_gc9a01(esp_lcd_panel_io_handle_t handle,
                                   const esp_lcd_panel_dev_config_t *config,
                                   esp_lcd_panel_handle_t *out) {
    CHECK(handle == &io && io.open && !panel.open);
    CHECK(config->reset_gpio_num == MYBOT_DISPLAY_RESET);
    CHECK(config->rgb_ele_order == MYBOT_DISPLAY_RGB_ORDER);
    CHECK(config->vendor_config);
    const auto *vendor = static_cast<const gc9a01_vendor_config_t *>(config->vendor_config);
    CHECK(vendor->init_cmds && vendor->init_cmds_size >= (EXPECTED_CONTROLLER == 6 ? 13 : 25));
    bool found_controller_command = false;
    for (size_t index = 0; index < vendor->init_cmds_size; ++index) {
        if (EXPECTED_CONTROLLER == 4 && vendor->init_cmds[index].cmd == 0x74) {
            found_controller_command = true;
        }
        if (EXPECTED_CONTROLLER == 5 && vendor->init_cmds[index].cmd == 0x7D) {
            found_controller_command = true;
        }
        if (EXPECTED_CONTROLLER == 6 && vendor->init_cmds[index].cmd == 0xB4) {
            found_controller_command = true;
        }
    }
    CHECK(found_controller_command);
    panel = {};
    panel.open = true;
    *out = &panel;
    return ESP_OK;
}

esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t handle) {
    CHECK(handle == &panel && panel.open);
    return ESP_OK;
}
esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t handle) {
    CHECK(handle == &panel && panel.open);
    return ESP_OK;
}
esp_err_t esp_lcd_panel_set_gap(esp_lcd_panel_handle_t handle, int x, int y) {
    CHECK(handle == &panel && panel.open);
    panel.gap_x = x;
    panel.gap_y = y;
    return ESP_OK;
}
esp_err_t esp_lcd_panel_invert_color(esp_lcd_panel_handle_t handle, bool invert) {
    CHECK(handle == &panel && panel.open);
    CHECK(invert == MYBOT_DISPLAY_INVERT_COLOR);
    return ESP_OK;
}
esp_err_t esp_lcd_panel_swap_xy(esp_lcd_panel_handle_t handle, bool swap) {
    CHECK(handle == &panel && panel.open);
    CHECK(swap == MYBOT_DISPLAY_SWAP_XY);
    return ESP_OK;
}
esp_err_t esp_lcd_panel_mirror(esp_lcd_panel_handle_t handle, bool x, bool y) {
    CHECK(handle == &panel && panel.open);
    CHECK(x == MYBOT_DISPLAY_MIRROR_X && y == MYBOT_DISPLAY_MIRROR_Y);
    return ESP_OK;
}
esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t handle, bool on) {
    CHECK(handle == &panel && panel.open);
    display_on_seen = display_on_seen || on;
    return ESP_OK;
}
esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t handle) {
    CHECK(handle == &panel && panel.open && io.open && backlight == 0);
    panel = {};
    ++panel_deletes;
    return ESP_OK;
}
esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t handle) {
    CHECK(handle == &io && io.open && !panel.open);
    io = {};
    ++io_deletes;
    return ESP_OK;
}
}

int main() {
    mybot_display_panel_t display{};
    CHECK(MYBOT_PANEL_CONTROLLER == EXPECTED_CONTROLLER);
    CHECK(mybot_display_panel_open(&display) == 0);
    CHECK(display.ready && display.spi_ready && display.backlight_ready);
    CHECK(panel.gap_x == MYBOT_DISPLAY_OFFSET_X && panel.gap_y == MYBOT_DISPLAY_OFFSET_Y);
    CHECK(backlight == MYBOT_DISPLAY_BRIGHTNESS_PERCENT);
    CHECK(display_on_seen);
    CHECK(mybot_display_panel_close(&display) == 0);
    CHECK(!display.ready && !display.spi_ready && !display.backlight_ready);
    CHECK(panel_deletes == 1 && io_deletes == 1 && spi_frees == 1);
    CHECK(mybot_display_panel_close(&display) == 0);
    std::puts("Custom controller panel lifecycle tests passed");
    return 0;
}

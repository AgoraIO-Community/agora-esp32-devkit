/* SPDX-License-Identifier: MIT */
#include "host_platform.h"

#include "board_config.h"
#include "display/display_panel.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);              \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

struct host_i80_bus {
    bool open = false;
};

struct host_panel_io {
    bool open = false;
};

struct host_panel {
    bool open = false;
    bool initialized = false;
    bool on = false;
    int gap_x = -1;
    int gap_y = -1;
};

static host_i80_bus bus;
static host_panel_io io;
static host_panel panel;
static bool rd_configured;
static int rd_level = -1;
static unsigned int backlight;
static unsigned int bus_initializations;
static unsigned int io_deletes;
static unsigned int panel_deletes;
static unsigned int bus_deletes;
static unsigned int post_command_count;
static const char *failed_step;

static esp_err_t step(const char *name) {
    if (failed_step == name) {
        failed_step = nullptr;
        return ESP_FAIL;
    }
    return ESP_OK;
}

extern "C" {
const char *esp_err_to_name(esp_err_t error) {
    return error == ESP_OK ? "ESP_OK" : "ESP_FAIL";
}

int mybot_board_set_display_backlight(unsigned int percent) {
    CHECK(percent <= 100);
    const esp_err_t result = step(percent ? "backlight_on" : "backlight_off");
    if (result != ESP_OK) {
        return -1;
    }
    backlight = percent;
    return 0;
}

esp_err_t gpio_config(const gpio_config_t *config) {
    CHECK(config && config->pin_bit_mask == 1ULL << MYBOT_DISPLAY_I80_RD);
    CHECK(config->mode == GPIO_MODE_INPUT_OUTPUT);
    CHECK(config->pull_up_en == GPIO_PULLUP_ENABLE);
    CHECK(config->pull_down_en == GPIO_PULLDOWN_DISABLE);
    CHECK(config->intr_type == GPIO_INTR_DISABLE);
    const esp_err_t result = step("rd_config");
    if (result == ESP_OK) {
        rd_configured = true;
    }
    return result;
}

esp_err_t gpio_set_level(gpio_num_t gpio, uint32_t level) {
    CHECK(gpio == MYBOT_DISPLAY_I80_RD && level <= 1);
    const esp_err_t result = step("rd_high");
    if (result == ESP_OK) {
        rd_level = static_cast<int>(level);
    }
    return result;
}

esp_err_t esp_lcd_new_i80_bus(const esp_lcd_i80_bus_config_t *config,
                              esp_lcd_i80_bus_handle_t *out) {
    CHECK(config && out && !bus.open);
    CHECK(config->dc_gpio_num == MYBOT_DISPLAY_DC);
    CHECK(config->wr_gpio_num == MYBOT_DISPLAY_SCLK);
    CHECK(config->clk_src == LCD_CLK_SRC_DEFAULT && config->bus_width == 8);
    CHECK(config->max_transfer_bytes ==
          MYBOT_DISPLAY_WIDTH * MYBOT_DISPLAY_HEIGHT * sizeof(uint16_t));
    CHECK(config->dma_burst_size == 64);
    CHECK(config->data_gpio_nums[0] == MYBOT_DISPLAY_I80_D0);
    CHECK(config->data_gpio_nums[1] == MYBOT_DISPLAY_I80_D1);
    CHECK(config->data_gpio_nums[2] == MYBOT_DISPLAY_I80_D2);
    CHECK(config->data_gpio_nums[3] == MYBOT_DISPLAY_I80_D3);
    CHECK(config->data_gpio_nums[4] == MYBOT_DISPLAY_I80_D4);
    CHECK(config->data_gpio_nums[5] == MYBOT_DISPLAY_I80_D5);
    CHECK(config->data_gpio_nums[6] == MYBOT_DISPLAY_I80_D6);
    CHECK(config->data_gpio_nums[7] == MYBOT_DISPLAY_I80_D7);
    ++bus_initializations;
    const esp_err_t result = step("bus_new");
    if (result == ESP_OK) {
        bus.open = true;
        *out = &bus;
    }
    return result;
}

esp_err_t esp_lcd_new_panel_io_i80(esp_lcd_i80_bus_handle_t handle,
                                   const esp_lcd_panel_io_i80_config_t *config,
                                   esp_lcd_panel_io_handle_t *out) {
    CHECK(handle == &bus && bus.open && config && out && !io.open);
    CHECK(config->cs_gpio_num == MYBOT_DISPLAY_CS);
    CHECK(config->pclk_hz == MYBOT_DISPLAY_I80_CLOCK_HZ);
    CHECK(config->trans_queue_depth == 2);
    CHECK(config->lcd_cmd_bits == 8 && config->lcd_param_bits == 8);
    CHECK(!config->dc_levels.dc_idle_level && !config->dc_levels.dc_cmd_level);
    CHECK(!config->dc_levels.dc_dummy_level && config->dc_levels.dc_data_level);
    const esp_err_t result = step("io_new");
    if (result == ESP_OK) {
        io.open = true;
        *out = &io;
    }
    return result;
}

esp_err_t esp_lcd_new_panel_st7789(esp_lcd_panel_io_handle_t handle,
                                   const esp_lcd_panel_dev_config_t *config,
                                   esp_lcd_panel_handle_t *out) {
    CHECK(handle == &io && io.open && config && out && !panel.open);
    CHECK(config->reset_gpio_num == MYBOT_DISPLAY_RESET);
    CHECK(config->rgb_ele_order == MYBOT_DISPLAY_RGB_ORDER);
    CHECK(config->bits_per_pixel == 16 && !config->vendor_config);
    const esp_err_t result = step("panel_new");
    if (result == ESP_OK) {
        panel.open = true;
        *out = &panel;
    }
    return result;
}

esp_err_t esp_lcd_panel_io_tx_param(esp_lcd_panel_io_handle_t handle, int command,
                                    const void *param, size_t param_size) {
    CHECK(handle == &io && io.open && param && param_size == 1);
    const auto value = *static_cast<const uint8_t *>(param);
    if (post_command_count % 2 == 0) {
        CHECK(command == 0x36 && value == MYBOT_DISPLAY_ST7789_MADCTL);
    } else {
        CHECK(command == 0x3A && value == MYBOT_DISPLAY_ST7789_COLMOD);
    }
    ++post_command_count;
    return step("post_cmd");
}

esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t handle) {
    CHECK(handle == &panel && panel.open);
    return step("panel_reset");
}
esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t handle) {
    CHECK(handle == &panel && panel.open);
    const esp_err_t result = step("panel_init");
    if (result == ESP_OK) {
        panel.initialized = true;
    }
    return result;
}
esp_err_t esp_lcd_panel_set_gap(esp_lcd_panel_handle_t handle, int x, int y) {
    CHECK(handle == &panel && panel.open);
    CHECK(x == MYBOT_DISPLAY_OFFSET_X && y == MYBOT_DISPLAY_OFFSET_Y);
    const esp_err_t result = step("panel_gap");
    if (result == ESP_OK) {
        panel.gap_x = x;
        panel.gap_y = y;
    }
    return result;
}
esp_err_t esp_lcd_panel_invert_color(esp_lcd_panel_handle_t handle, bool invert) {
    CHECK(handle == &panel && panel.open && invert == MYBOT_DISPLAY_INVERT_COLOR);
    return step("panel_invert");
}
esp_err_t esp_lcd_panel_swap_xy(esp_lcd_panel_handle_t handle, bool swap) {
    CHECK(handle == &panel && panel.open && swap == MYBOT_DISPLAY_SWAP_XY);
    return step("panel_swap");
}
esp_err_t esp_lcd_panel_mirror(esp_lcd_panel_handle_t handle, bool x, bool y) {
    CHECK(handle == &panel && panel.open);
    CHECK(x == MYBOT_DISPLAY_MIRROR_X && y == MYBOT_DISPLAY_MIRROR_Y);
    return step("panel_mirror");
}
esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t handle, bool on) {
    CHECK(handle == &panel && panel.open);
    const esp_err_t result = step(on ? "display_on" : "display_off");
    if (result == ESP_OK) {
        panel.on = on;
    }
    return result;
}
esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t handle) {
    CHECK(handle == &panel && panel.open && backlight == 0);
    const esp_err_t result = step("panel_delete");
    if (result == ESP_OK) {
        panel = {};
        ++panel_deletes;
    }
    return result;
}
esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t handle) {
    CHECK(handle == &io && io.open && !panel.open);
    const esp_err_t result = step("io_delete");
    if (result == ESP_OK) {
        io = {};
        ++io_deletes;
    }
    return result;
}
esp_err_t esp_lcd_del_i80_bus(esp_lcd_i80_bus_handle_t handle) {
    CHECK(handle == &bus && bus.open && !io.open && !panel.open);
    const esp_err_t result = step("bus_delete");
    if (result == ESP_OK) {
        bus = {};
        ++bus_deletes;
    }
    return result;
}
}

static void reset_state() {
    bus = {};
    io = {};
    panel = {};
    rd_configured = false;
    rd_level = -1;
    backlight = 0;
    post_command_count = 0;
    failed_step = nullptr;
}

static void assert_released(const mybot_display_panel_t &display) {
    CHECK(!display.ready && !display.backlight_ready && !display.io && !display.panel);
    CHECK(!bus.open && !io.open && !panel.open && !panel.on && backlight == 0);
}

static void test_open_and_close() {
    reset_state();
    mybot_display_panel_t display{};
    CHECK(mybot_display_panel_open(&display) == 0);
    CHECK(display.ready && display.backlight_ready);
    CHECK(rd_configured && rd_level == 1);
    CHECK(panel.open && panel.initialized && panel.on);
    CHECK(panel.gap_x == MYBOT_DISPLAY_OFFSET_X && panel.gap_y == MYBOT_DISPLAY_OFFSET_Y);
    CHECK(post_command_count == 2 && backlight == MYBOT_DISPLAY_BRIGHTNESS_PERCENT);
    CHECK(mybot_display_panel_open(&display) < 0 && bus_initializations == 1);
    CHECK(mybot_display_panel_close(&display) == 0);
    assert_released(display);
    CHECK(panel_deletes == 1 && io_deletes == 1 && bus_deletes == 1);
    CHECK(mybot_display_panel_close(&display) == 0);
}

static void test_open_failures_cleanup() {
    const char *failures[] = {
        "backlight_off", "rd_config",   "rd_high",      "bus_new",    "io_new",
        "panel_new",     "panel_reset", "panel_init",   "panel_gap",  "panel_invert",
        "post_cmd",      "panel_swap",  "panel_mirror", "display_on", "backlight_on",
    };
    for (const char *failure : failures) {
        reset_state();
        mybot_display_panel_t display{};
        failed_step = failure;
        CHECK(mybot_display_panel_open(&display) < 0);
        CHECK(!failed_step);
        CHECK(mybot_display_panel_close(&display) == 0);
        assert_released(display);
    }
}

static void test_cleanup_retry() {
    const char *failures[] = {"panel_delete", "io_delete", "bus_delete"};
    for (const char *failure : failures) {
        reset_state();
        mybot_display_panel_t display{};
        CHECK(mybot_display_panel_open(&display) == 0);
        failed_step = failure;
        CHECK(mybot_display_panel_close(&display) < 0);
        CHECK(!failed_step);
        CHECK(mybot_display_panel_close(&display) == 0);
        assert_released(display);
        const unsigned int inits = bus_initializations;
        CHECK(mybot_display_panel_open(&display) == 0);
        CHECK(bus_initializations == inits + 1);
        CHECK(mybot_display_panel_close(&display) == 0);
        assert_released(display);
    }
}

int main() {
    test_open_and_close();
    test_open_failures_cleanup();
    test_cleanup_retry();
    std::puts("ST7789 i80 panel lifecycle host tests passed");
    return 0;
}

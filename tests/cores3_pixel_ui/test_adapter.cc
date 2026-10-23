/* SPDX-License-Identifier: MIT */
#include "host_platform.h"
#include "display/ili9342_panel.h"
#include "display/lvgl_view.h"
#include "display/pixel_glasses.h"
#include "network/wifi_control.h"
#include <mybot_platform/display_status.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);              \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

struct host_panel_io {
    bool open;
    esp_lcd_panel_io_color_trans_done_cb_t done;
    void *user;
};
struct host_panel {
    bool open;
};
struct host_display {
    bool open;
    bool refresh;
    lv_display_flush_cb_t flush;
};
struct host_timer {
    bool open;
    lv_timer_cb_t callback;
};
static host_panel_io io{};
static host_panel panel{};
static host_display display{};
static host_timer timer{};
static bool port_open, port_locked, view_open, finish_on_delay;
static unsigned pending_dma, updates, closes;
static int64_t now_us;
static std::string fail_step, fail_cleanup;
static mybot_lcd_content_t applied{};
static mybot_display_runtime_status_t applied_runtime{};
static std::string applied_ssid;
static mybot_expression_t visible = MYBOT_EXPRESSION_NEUTRAL;
static bool replay_ok = true;
static void *active_owner;
extern "C" const mybot_lcd_ops_t *mybot_cores3_lcd_ops(void);

static bool fail(const char *name) {
    if (fail_step == name) {
        fail_step.clear();
        return true;
    }
    if (fail_cleanup == name) {
        fail_cleanup.clear();
        return true;
    }
    return false;
}
static void send(mybot_lcd_screen_t screen, uint32_t indicators = 0) {
    mybot_lcd_content_t content{};
    content.screen = screen;
    content.indicators = indicators;
    CHECK(mybot_cores3_lcd_ops()->render(active_owner, &content) == 0);
}
static void complete_dma() {
    CHECK(pending_dma && io.open && display.open && io.done);
    --pending_dma;
    io.done(&io, nullptr, io.user);
}
static void tick(unsigned ms = 0) {
    CHECK(timer.open && port_open && !port_locked);
    now_us += (int64_t)ms * 1000;
    port_locked = true;
    timer.callback(&timer);
    port_locked = false;
}

extern "C" {
const char *esp_err_to_name(esp_err_t) {
    return "mock error";
}
int mybot_cores3_set_display_backlight(unsigned int) {
    return 0;
}
int mybot_ili9342_panel_open(ili9342_panel_t *lcd, esp_lcd_panel_io_color_trans_done_cb_t, void *) {
    CHECK(!io.open && !panel.open);
    io = {true, nullptr, nullptr};
    panel.open = true;
    lcd->io = &io;
    lcd->panel = &panel;
    lcd->spi_ready = true;
    return fail("panel_open") ? -1 : 0;
}
int mybot_ili9342_panel_close(ili9342_panel_t *lcd) {
    CHECK(!pending_dma);
    if (fail("panel_close"))
        return -1;
    io = {};
    panel = {};
    *lcd = {};
    ++closes;
    return 0;
}
esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t, bool) {
    return ESP_OK;
}
esp_err_t esp_lcd_panel_draw_bitmap(esp_lcd_panel_handle_t handle, int, int, int, int,
                                    const void *) {
    CHECK(handle == &panel && io.open && panel.open && display.open);
    if (fail("draw"))
        return ESP_FAIL;
    ++pending_dma;
    return ESP_OK;
}
esp_err_t esp_lcd_panel_io_register_event_callbacks(esp_lcd_panel_io_handle_t handle,
                                                    const esp_lcd_panel_io_callbacks_t *callbacks,
                                                    void *user) {
    CHECK(handle == &io && io.open);
    if (fail("register"))
        return ESP_FAIL;
    io.done = callbacks->on_color_trans_done;
    io.user = user;
    return ESP_OK;
}
SemaphoreHandle_t xSemaphoreCreateMutexStatic(StaticSemaphore_t *storage) {
    storage->taken = false;
    return storage;
}
int xSemaphoreTake(SemaphoreHandle_t sem, unsigned int) {
    CHECK(sem && !sem->taken);
    sem->taken = true;
    return pdTRUE;
}
int xSemaphoreGive(SemaphoreHandle_t sem) {
    CHECK(sem && sem->taken);
    sem->taken = false;
    return pdTRUE;
}
void vTaskDelay(unsigned int ticks) {
    now_us += ticks * 1000;
    if (finish_on_delay) {
        finish_on_delay = false;
        complete_dma();
    }
}
int64_t esp_timer_get_time(void) {
    return now_us;
}
int lv_area_get_size(const lv_area_t *a) {
    return (a->x2 - a->x1 + 1) * (a->y2 - a->y1 + 1);
}
void lv_draw_sw_rgb565_swap(void *pixels, int size) {
    auto *data = static_cast<uint8_t *>(pixels);
    for (int i = 0; i < size; ++i) {
        uint8_t v = data[2 * i];
        data[2 * i] = data[2 * i + 1];
        data[2 * i + 1] = v;
    }
}
void lv_display_set_flush_cb(lv_display_t *, lv_display_flush_cb_t cb) {
    display.flush = cb;
}
void lv_display_delete_refr_timer(lv_display_t *) {
    CHECK(port_locked);
    display.refresh = false;
}
lv_timer_t *lv_timer_create(lv_timer_cb_t cb, unsigned int ms, void *) {
    CHECK(port_locked && !timer.open && ms == 50);
    if (fail("timer"))
        return nullptr;
    timer = {true, cb};
    return &timer;
}
void lv_timer_delete(lv_timer_t *) {
    CHECK(port_locked);
    timer = {};
}
esp_err_t lvgl_port_init(const lvgl_port_cfg_t *cfg) {
    CHECK(!port_open && cfg->task_priority == 1 && cfg->task_affinity == 1);
    port_open = true;
    return fail("port_init") ? ESP_FAIL : ESP_OK;
}
esp_err_t lvgl_port_deinit(void) {
    CHECK(!display.open && !panel.open && !io.open);
    if (fail("port_deinit"))
        return ESP_FAIL;
    port_open = false;
    return ESP_OK;
}
bool lvgl_port_lock(unsigned int) {
    CHECK(port_open && !port_locked);
    if (fail("port_lock"))
        return false;
    port_locked = true;
    return true;
}
void lvgl_port_unlock(void) {
    CHECK(port_locked);
    port_locked = false;
}
lv_display_t *lvgl_port_add_disp(const lvgl_port_display_cfg_t *cfg) {
    CHECK(port_locked && cfg->hres == 320 && cfg->vres == 240);
    CHECK(cfg->buffer_size == 320 * 16 && cfg->flags.buff_dma && !cfg->flags.swap_bytes);
    if (fail("display"))
        return nullptr;
    display = {true, true, nullptr};
    return &display;
}
esp_err_t lvgl_port_remove_disp(lv_display_t *) {
    CHECK(port_locked && !display.refresh && !timer.open && !view_open && !io.open && !pending_dma);
    if (fail("display_remove"))
        return ESP_FAIL;
    display = {};
    return ESP_OK;
}
void lvgl_port_flush_ready(lv_display_t *) {
    CHECK(display.open);
}
esp_err_t lvgl_port_task_wake(int, void *) {
    return ESP_OK;
}
int mybot_lvgl_view_create(lv_display_t *) {
    CHECK(port_locked);
    view_open = true;
    return fail("view") ? -1 : 0;
}
void mybot_lvgl_view_destroy(void) {
    CHECK(port_locked && !pending_dma);
    view_open = false;
}
int mybot_wifi_get_provisioning_ssid(char *ssid, size_t capacity) {
    CHECK(capacity >= sizeof("mybot-test"));
    strcpy(ssid, "mybot-test");
    return 0;
}
void mybot_pixel_glasses_update(const mybot_lcd_content_t *content, const char *ssid,
                                const mybot_display_runtime_status_t *runtime) {
    CHECK(view_open && port_locked && content && runtime);
    applied = *content;
    applied_runtime = *runtime;
    applied_ssid = ssid;
    ++updates;
}
int mybot_pixel_glasses_show_expression(mybot_expression_t expression) {
    CHECK(view_open && port_locked);
    visible = expression;
    return 0;
}
int mybot_pixel_glasses_replay_expression(mybot_expression_t expression) {
    CHECK(view_open && port_locked);
    if (!replay_ok)
        return -1;
    visible = expression;
    return 0;
}
}

static void released() {
    CHECK(!io.open && !panel.open && !display.open && !timer.open && !view_open && !port_open);
    CHECK(!port_locked && !pending_dma);
}
static void init() {
    CHECK(mybot_cores3_lcd_ops()->init(&active_owner) == 0 && active_owner);
    tick();
}
static void stop() {
    mybot_cores3_lcd_ops()->destroy(active_owner);
    active_owner = nullptr;
    released();
}

static void lifecycle_tests() {
    const auto *ops = mybot_cores3_lcd_ops();
    CHECK(ops->init(nullptr) < 0);
    for (const char *failure :
         {"panel_open", "port_init", "port_lock", "display", "register", "view", "timer"}) {
        fail_step = failure;
        void *out = reinterpret_cast<void *>(1);
        CHECK(ops->init(&out) < 0 && !out && fail_step.empty());
        released();
    }
    for (const char *failure : {"panel_close", "display_remove", "port_deinit"}) {
        init();
        fail_cleanup = failure;
        ops->destroy(active_owner);
        CHECK(fail_cleanup.empty());
        CHECK(ops->init(&active_owner) == 0); // finishes deferred cleanup first
        stop();
    }
    init();
    const lv_area_t area{0, 0, 1, 0};
    uint8_t pixels[]{0x34, 0x12, 0x78, 0x56};
    display.flush(&display, &area, pixels);
    CHECK(pending_dma == 1 && pixels[0] == 0x12 && pixels[1] == 0x34);
    finish_on_delay = true;
    stop();
    CHECK(!finish_on_delay);
    init();
    fail_step = "draw";
    display.flush(&display, &area, pixels);
    CHECK(!pending_dma && fail_step.empty());
    stop();
}

static void ui_tests() {
    init();
    send(MYBOT_LCD_SCREEN_READY);
    tick();
    CHECK(visible == MYBOT_EXPRESSION_NEUTRAL);
    CHECK(mybot_display_set_runtime_status("ready", false) == 0);
    tick();
    CHECK(!strcmp(applied_runtime.state, "ready") && !applied_runtime.in_conversation);
    const unsigned before = updates;
    CHECK(mybot_display_set_runtime_status("ready", false) == 0);
    tick();
    CHECK(updates == before);
    CHECK(mybot_display_set_runtime_status("", false) < 0);
    send(MYBOT_LCD_SCREEN_WIFI_PROVISIONING);
    tick();
    CHECK(applied_ssid == "mybot-test");
    mybot_lcd_content_t pair{};
    pair.screen = MYBOT_LCD_SCREEN_PAIR_CODE;
    strcpy(pair.pair_code, "123456");
    CHECK(mybot_cores3_lcd_ops()->render(active_owner, &pair) == 0);
    strcpy(pair.pair_code, "xxxxxx");
    tick();
    CHECK(!strcmp(applied.pair_code, "123456"));

    send(MYBOT_LCD_SCREEN_IN_CONVERSATION, MYBOT_LCD_INDICATOR_LISTENING);
    tick();
    CHECK(visible == MYBOT_EXPRESSION_SURPRISED);
    tick(549);
    CHECK(visible == MYBOT_EXPRESSION_SURPRISED);
    tick(1);
    CHECK(visible == MYBOT_EXPRESSION_LISTENING);
    send(MYBOT_LCD_SCREEN_IN_CONVERSATION, MYBOT_LCD_INDICATOR_THINKING);
    tick();
    CHECK(visible == MYBOT_EXPRESSION_THINKING);
    tick(2999);
    CHECK(visible == MYBOT_EXPRESSION_THINKING);
    tick(1);
    CHECK(visible == MYBOT_EXPRESSION_PROCESSING);
    send(MYBOT_LCD_SCREEN_IN_CONVERSATION,
         MYBOT_LCD_INDICATOR_LISTENING | MYBOT_LCD_INDICATOR_VP_REGISTERED);
    tick();
    CHECK(visible == MYBOT_EXPRESSION_LISTENING);
    CHECK(applied.indicators ==
          (MYBOT_LCD_INDICATOR_LISTENING | MYBOT_LCD_INDICATOR_VP_REGISTERED));

    send(MYBOT_LCD_SCREEN_READY);
    tick();
    CHECK(visible == MYBOT_EXPRESSION_NEUTRAL);
    replay_ok = false;
    send(MYBOT_LCD_SCREEN_IN_CONVERSATION, MYBOT_LCD_INDICATOR_LISTENING);
    tick();
    CHECK(visible == MYBOT_EXPRESSION_NEUTRAL);
    replay_ok = true;
    tick();
    CHECK(visible == MYBOT_EXPRESSION_SURPRISED);

    send(MYBOT_LCD_SCREEN_IN_CONVERSATION, MYBOT_LCD_INDICATOR_LISTENING | (1u << 31));
    tick();
    CHECK(visible == MYBOT_EXPRESSION_SURPRISED);

    void *sdk = nullptr;
    CHECK(mybot_cores3_lcd_ops()->init(&sdk) == 0 && sdk == active_owner);
    tick();
    mybot_cores3_lcd_ops()->destroy(sdk);
    tick();
    CHECK(visible == MYBOT_EXPRESSION_NEUTRAL && !applied.indicators && display.open);
    send(MYBOT_LCD_SCREEN_WIFI_PROVISIONING);
    tick();
    CHECK(applied_ssid == "mybot-test");
    stop();
    CHECK(mybot_display_set_runtime_status("ready", false) < 0);
}

int main() {
    lifecycle_tests();
    ui_tests();
    std::puts("CoreS3 pixel adapter: lifecycle, DMA, indicator and phase checks passed");
}

/* SPDX-License-Identifier: MIT */
#include "lvgl.h"
#include "esp_heap_caps.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_set>

#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);              \
            std::abort();                                                                          \
        }                                                                                          \
    } while (0)

static unsigned creation_step;
static unsigned fail_creation_step;
static bool fail_state_allocation;
static void *state_allocation;
static size_t state_bytes;
static unsigned state_allocations;
static unsigned state_frees;
static unsigned delete_callbacks;
static lv_display_t *display;
static std::unordered_set<void *> lv_allocations;

static lv_obj_t *renderer_obj_create(lv_obj_t *parent);
static lv_obj_t *renderer_label_create(lv_obj_t *parent);
static lv_obj_t *renderer_line_create(lv_obj_t *parent);
static lv_timer_t *renderer_timer_create(lv_timer_cb_t callback, uint32_t period, void *user);

/* Inject failures only into renderer-owned object/timer creation; the complete
 * pinned LVGL runtime still owns their callbacks, tree and destruction. */
#define lv_obj_create renderer_obj_create
#define lv_label_create renderer_label_create
#define lv_line_create renderer_line_create
#define lv_timer_create renderer_timer_create
#include "../../components/mybot_platform/src/drivers/display/renderers/lvgl/pixel_glasses_view.cc"
#undef lv_obj_create
#undef lv_label_create
#undef lv_line_create
#undef lv_timer_create

static bool creation_fails() {
    return ++creation_step == fail_creation_step;
}
static lv_obj_t *renderer_obj_create(lv_obj_t *parent) {
    return creation_fails() ? nullptr : lv_obj_create(parent);
}
static lv_obj_t *renderer_label_create(lv_obj_t *parent) {
    return creation_fails() ? nullptr : lv_label_create(parent);
}
static lv_obj_t *renderer_line_create(lv_obj_t *parent) {
    return creation_fails() ? nullptr : lv_line_create(parent);
}
static lv_timer_t *renderer_timer_create(lv_timer_cb_t callback, uint32_t period, void *user) {
    return creation_fails() ? nullptr : lv_timer_create(callback, period, user);
}

extern "C" {
void *heap_caps_calloc(size_t count, size_t bytes, uint32_t caps) {
    CHECK(!state_allocation && count == 1 && bytes == sizeof(View));
    CHECK(caps == (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (fail_state_allocation)
        return nullptr;
    state_allocation = std::calloc(count, bytes);
    CHECK(state_allocation);
    state_bytes = bytes;
    ++state_allocations;
    return state_allocation;
}
void heap_caps_free(void *pointer) {
    CHECK(pointer == state_allocation && pointer);
    CHECK(lv_obj_get_child_count(lv_display_get_screen_active(display)) == 0);
    std::memset(pointer, 0xa5, state_bytes);
    std::free(pointer);
    state_allocation = nullptr;
    ++state_frees;
}
void lv_mem_init(void) {
}
void lv_mem_deinit(void) {
    CHECK(lv_allocations.empty());
}
lv_mem_pool_t lv_mem_add_pool(void *, size_t) {
    return nullptr;
}
void lv_mem_remove_pool(lv_mem_pool_t) {
}
void *lv_malloc_core(size_t bytes) {
    void *pointer = std::malloc(bytes);
    if (pointer)
        lv_allocations.insert(pointer);
    return pointer;
}
void *lv_realloc_core(void *pointer, size_t bytes) {
    if (pointer)
        CHECK(lv_allocations.erase(pointer) == 1);
    void *next = std::realloc(pointer, bytes);
    if (next)
        lv_allocations.insert(next);
    else if (pointer)
        lv_allocations.insert(pointer);
    return next;
}
void lv_free_core(void *pointer) {
    CHECK(lv_allocations.erase(pointer) == 1);
    std::free(pointer);
}
void lv_mem_monitor_core(lv_mem_monitor_t *monitor) {
    std::memset(monitor, 0, sizeof(*monitor));
}
lv_result_t lv_mem_test_core(void) {
    return LV_RESULT_OK;
}
}

static unsigned timer_count() {
    unsigned count = 0;
    for (lv_timer_t *timer = lv_timer_get_next(nullptr); timer; timer = lv_timer_get_next(timer))
        ++count;
    return count;
}
static bool in_state(const void *pointer) {
    const uintptr_t address = reinterpret_cast<uintptr_t>(pointer);
    const uintptr_t start = reinterpret_cast<uintptr_t>(state_allocation);
    return state_allocation && address >= start && address < start + state_bytes;
}
static void deleted(lv_event_t *) {
    CHECK(state_allocation == s_view);
    /* The view timer is removed before the widget callbacks can inspect banks. */
    for (lv_timer_t *timer = lv_timer_get_next(nullptr); timer; timer = lv_timer_get_next(timer))
        CHECK(timer != s_view->timer);
    CHECK(in_state(s_view->banks[0].eyes[0][0].points));
    CHECK(in_state(&s_view->banks[1].eye_polygons[0]));
    ++delete_callbacks;
}
static void assert_released(unsigned timers) {
    CHECK(!state_allocation && !s_view);
    CHECK(lv_obj_get_child_count(lv_display_get_screen_active(display)) == 0);
    CHECK(timer_count() == timers && state_allocations == state_frees);
    mybot_lvgl_view_destroy();
    mybot_lvgl_view_update(nullptr);
    CHECK(mybot_pixel_glasses_show_expression(MYBOT_EXPRESSION_NEUTRAL) < 0);
    CHECK(mybot_pixel_glasses_replay_expression(MYBOT_EXPRESSION_NEUTRAL) < 0);
}
static void tick() {
    lv_tick_inc(60);
    (void)lv_timer_handler();
}

int main() {
    lv_init();
    display = lv_display_create(320, 240);
    CHECK(display);
    /* This suite exercises timers and retained geometry, without panel flushing. */
    lv_display_delete_refr_timer(display);
    const unsigned timers = timer_count();
    CHECK(mybot_lvgl_view_create(nullptr) < 0);
    fail_state_allocation = true;
    CHECK(mybot_lvgl_view_create(display) < 0);
    assert_released(timers);
    fail_state_allocation = false;

    creation_step = 0;
    CHECK(mybot_lvgl_view_create(display) == 0);
    const unsigned steps = creation_step;
    CHECK(mybot_lvgl_view_create(display) < 0);
    mybot_lvgl_view_destroy();
    assert_released(timers);
    /* Every allocation stage includes root, nested shapes, labels and timer. */
    for (unsigned failure = 1; failure <= steps; ++failure) {
        creation_step = 0;
        fail_creation_step = failure;
        CHECK(mybot_lvgl_view_create(display) < 0);
        assert_released(timers);
        tick();
    }
    fail_creation_step = 0;
    for (unsigned cycle = 0; cycle < 12; ++cycle) {
        creation_step = 0;
        CHECK(mybot_lvgl_view_create(display) == 0);
        View *state = s_view;
        CHECK(lv_obj_add_event_cb(s_view->root, deleted, LV_EVENT_DELETE, nullptr));
        mybot_lcd_content_t content{};
        content.screen = MYBOT_LCD_SCREEN_READY;
        mybot_pixel_glasses_update(&content, nullptr, nullptr);
        for (int expression = MYBOT_EXPRESSION_NEUTRAL; expression < MYBOT_EXPRESSION_COUNT;
             ++expression) {
            CHECK(mybot_pixel_glasses_show_expression(
                      static_cast<mybot_expression_t>(expression)) == 0);
            tick();
            CHECK(s_view == state && state_allocation == state);
        }
        mybot_lvgl_view_destroy();
        assert_released(timers);
        tick(); // A destroyed view must have no timer callback left to run.
    }
    CHECK(delete_callbacks == 12);
    lv_display_delete(display);
    display = nullptr;
    lv_deinit();
    CHECK(lv_allocations.empty());
    std::puts("CoreS3 pixel renderer PSRAM lifecycle: PASS");
}

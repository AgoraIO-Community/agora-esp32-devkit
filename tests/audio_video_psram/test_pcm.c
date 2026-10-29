/* SPDX-License-Identifier: Apache-2.0 */
#include "host_runtime.h"
#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#define free host_free
#include "../../components/mybot_platform/src/services/audio/pcm_playback_buffer.c"
#undef free

static atomic_bool block_sink, entered;
static atomic_uint sink_frames;
static esp_err_t sink(void *context, const int16_t *pcm, size_t frames, size_t *written,
                      uint32_t timeout) {
    (void)context;
    (void)timeout;
    assert(pcm && frames && frames <= WRITE_FRAMES);
    atomic_store(&entered, true);
    while (atomic_load(&block_sink))
        host_real_delay(1);
    for (size_t i = 0; i < frames; ++i)
        assert(pcm[i] == 123);
    atomic_fetch_add(&sink_frames, (unsigned int)frames);
    *written = frames;
    return ESP_OK;
}
static bool sink_entered(void) {
    return atomic_load(&entered);
}
static bool all_frames_written(void) {
    return atomic_load(&sink_frames) == PREBUFFER_FRAMES;
}
static mybot_pcm_playback_buffer_t *create(void) {
    atomic_store(&entered, false);
    atomic_store(&sink_frames, 0);
    atomic_store(&block_sink, false);
    mybot_pcm_playback_buffer_t *buffer = mybot_pcm_playback_buffer_create(sink, &sink_frames);
    assert(buffer && host_tasks == 1 && host_psram_stacks == 1);
    host_expect_psram(buffer->fifo);
    host_expect_psram(buffer->scratch);
    return buffer;
}
static void enqueue(mybot_pcm_playback_buffer_t *buffer) {
    int16_t frames[PREBUFFER_FRAMES];
    for (size_t i = 0; i < PREBUFFER_FRAMES; ++i)
        frames[i] = 123;
    assert(mybot_pcm_playback_buffer_write(buffer, frames, PREBUFFER_FRAMES) == PREBUFFER_FRAMES);
}
int main(void) {
    for (int fail = 1; fail <= 4; ++fail) {
        host_reset();
        host_fail_alloc = fail;
        assert(!mybot_pcm_playback_buffer_create(sink, &sink_frames));
        host_reset();
    }
    host_fail_task = 1;
    assert(!mybot_pcm_playback_buffer_create(sink, &sink_frames));
    host_reset();
    for (int repeat = 0; repeat < 3; ++repeat) {
        mybot_pcm_playback_buffer_t *buffer = create();
        for (int restart = 0; restart < 2; ++restart) {
            atomic_store(&sink_frames, 0);
            assert(mybot_pcm_playback_buffer_start(buffer) == 0);
            enqueue(buffer);
            host_wait_until(all_frames_written);
            assert(mybot_pcm_playback_buffer_drain(buffer, 1000) == 0);
            assert(mybot_pcm_playback_buffer_stop(buffer) == 0);
            unsigned int previous = atomic_load(&sink_frames);
            host_real_delay(2);
            assert(atomic_load(&sink_frames) == previous);
        }
        assert(mybot_pcm_playback_buffer_destroy(buffer) == 0);
        host_reset();
    }
    mybot_pcm_playback_buffer_t *buffer = create();
    atomic_store(&block_sink, true);
    assert(mybot_pcm_playback_buffer_start(buffer) == 0);
    enqueue(buffer);
    host_wait_until(sink_entered);
    assert(mybot_pcm_playback_buffer_stop(buffer) < 0);
    assert(mybot_pcm_playback_buffer_destroy(buffer) < 0);
    assert(host_tasks == 1 && host_allocations == 4 && host_semaphores == 4);
    atomic_store(&block_sink, false);
    assert(mybot_pcm_playback_buffer_destroy(buffer) == 0);
    host_reset();
    buffer = create();
    __atomic_store_n(&host_hold_suspend, 1, __ATOMIC_RELEASE);
    assert(mybot_pcm_playback_buffer_destroy(buffer) < 0);
    assert(host_tasks == 1 && host_allocations == 4);
    __atomic_store_n(&host_hold_suspend, 0, __ATOMIC_RELEASE);
    assert(mybot_pcm_playback_buffer_destroy(buffer) == 0);
    host_reset();
    puts("PCM PSRAM allocation, cleanup, timeout, restart and owner deletion passed");
    return 0;
}

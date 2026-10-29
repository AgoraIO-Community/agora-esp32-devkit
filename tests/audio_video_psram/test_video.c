/* SPDX-License-Identifier: Apache-2.0 */
#include "host_runtime.h"
#include <assert.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
static int camera_ioctl(int fd, unsigned long request, ...);
static int camera_open(const char *path, int flags, ...);
static int camera_close(int fd);
static void *camera_mmap(void *address, size_t length, int protection, int flags, int fd,
                         off_t offset);
#define ioctl camera_ioctl
#define open camera_open
#define close camera_close
#define mmap camera_mmap
#include "../../components/mybot_platform/src/drivers/video/cores3_camera_video.c"
#undef ioctl
#undef open
#undef close
#undef mmap

enum failure {
    FAIL_NONE,
    FAIL_JPEG,
    FAIL_RESET,
    FAIL_INIT,
    FAIL_OPEN,
    FAIL_FORMAT,
    FAIL_REQUEST,
    FAIL_MAP,
    FAIL_STREAM,
    FAIL_DEINIT
};
static enum failure failure;
static uint8_t *raw[VIDEO_BUFFER_COUNT];
static int camera_devices, camera_fds, encoders, queued_index;
static bool queued;
static atomic_bool block_callback, callback_active;
static atomic_uint callback_count;
void *mybot_cores3_i2c_bus_handle(void) {
    return &camera_devices;
}
int mybot_cores3_reset_camera(void) {
    return failure == FAIL_RESET ? -1 : 0;
}
esp_err_t esp_video_init(const esp_video_init_config_t *config) {
    assert(config && config->dvp && !camera_devices);
    if (failure == FAIL_INIT)
        return ESP_FAIL;
    ++camera_devices;
    return ESP_OK;
}
esp_err_t esp_video_deinit(void) {
    if (failure == FAIL_DEINIT)
        return ESP_FAIL;
    assert(camera_devices == 1 && !camera_fds);
    --camera_devices;
    return ESP_OK;
}
int jpeg_enc_open(const jpeg_enc_config_t *config, jpeg_enc_handle_t *encoder) {
    assert(!config->task_enable && !encoders);
    if (failure == FAIL_JPEG)
        return -1;
    ++encoders;
    *encoder = &encoders;
    return JPEG_ERR_OK;
}
int jpeg_enc_close(jpeg_enc_handle_t encoder) {
    assert(encoder && encoders == 1);
    --encoders;
    return JPEG_ERR_OK;
}
int jpeg_enc_set_quality(jpeg_enc_handle_t encoder, unsigned int quality) {
    assert(encoder && quality >= 30 && quality <= 60);
    return JPEG_ERR_OK;
}
int jpeg_enc_process(jpeg_enc_handle_t encoder, const uint8_t *input, int bytes, uint8_t *output,
                     int capacity, int *written) {
    assert(encoder && input && bytes == VIDEO_RAW_BYTES && capacity >= 512);
    host_expect_psram(output);
    memset(output, 123, 512);
    *written = 512;
    return JPEG_ERR_OK;
}
static void clear_raw(void) {
    for (size_t i = 0; i < VIDEO_BUFFER_COUNT; ++i) {
        heap_caps_free(raw[i]);
        raw[i] = NULL;
    }
    queued = false;
}
static int camera_open(const char *path, int flags, ...) {
    (void)path;
    (void)flags;
    assert(camera_devices && !camera_fds);
    if (failure == FAIL_OPEN)
        return -1;
    ++camera_fds;
    return 42;
}
static int camera_close(int fd) {
    assert(fd == 42 && camera_fds == 1 && !atomic_load(&callback_active));
    clear_raw();
    --camera_fds;
    return 0;
}
static void *camera_mmap(void *address, size_t length, int protection, int flags, int fd,
                         off_t offset) {
    (void)address;
    (void)protection;
    (void)flags;
    assert(fd == 42 && length == VIDEO_RAW_BYTES);
    if (failure == FAIL_MAP)
        return MAP_FAILED;
    assert(offset < VIDEO_BUFFER_COUNT);
    return raw[offset];
}
static int camera_ioctl(int fd, unsigned long request, ...) {
    assert(fd == 42 && camera_fds == 1);
    va_list args;
    va_start(args, request);
    void *argument = va_arg(args, void *);
    va_end(args);
    switch (request) {
    case VIDIOC_G_FMT: {
        if (failure == FAIL_FORMAT)
            return -1;
        struct v4l2_format *format = argument;
        format->fmt.pix.width = VIDEO_WIDTH;
        format->fmt.pix.height = VIDEO_HEIGHT;
        format->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        return 0;
    }
    case VIDIOC_REQBUFS:
        clear_raw();
        if (failure == FAIL_REQUEST)
            return -1;
        for (size_t i = 0; i < VIDEO_BUFFER_COUNT; ++i) {
            raw[i] =
                heap_caps_aligned_alloc(16, VIDEO_RAW_BYTES, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            if (!raw[i])
                return -1;
        }
        return 0;
    case VIDIOC_QUERYBUF: {
        struct v4l2_buffer *buffer = argument;
        buffer->length = VIDEO_RAW_BYTES;
        buffer->m.offset = buffer->index;
        return 0;
    }
    case VIDIOC_QBUF:
        assert(!queued);
        queued_index = ((struct v4l2_buffer *)argument)->index;
        queued = true;
        return 0;
    case VIDIOC_STREAMON:
        return failure == FAIL_STREAM ? -1 : 0;
    case VIDIOC_STREAMOFF:
        assert(!atomic_load(&callback_active));
        queued = false;
        return 0;
    case VIDIOC_S_DQBUF_TIMEOUT:
        return 0;
    case VIDIOC_DQBUF: {
        assert(queued);
        vTaskDelay(10);
        struct v4l2_buffer *buffer = argument;
        buffer->index = queued_index;
        buffer->bytesused = VIDEO_RAW_BYTES;
        queued = false;
        return 0;
    }
    default:
        assert(!"unknown video ioctl");
        return -1;
    }
}
static int frame_handler(const mybot_video_frame_t *frame, void *context) {
    assert(context == &callback_count && frame->codec == MYBOT_VIDEO_CODEC_JPEG &&
           frame->len == 512);
    host_expect_psram((void *)frame->data);
    assert(((const uint8_t *)frame->data)[0] == 123);
    atomic_store(&callback_active, true);
    atomic_fetch_add(&callback_count, 1);
    while (atomic_load(&block_callback))
        host_real_delay(1);
    assert(((const uint8_t *)frame->data)[0] == 123);
    atomic_store(&callback_active, false);
    return 0;
}
static bool received_frame(void) {
    return atomic_load(&callback_count) > 0;
}
static bool worker_suspended(void) {
    return eTaskGetState(s_video.task) == eSuspended;
}
static void reset(void) {
    assert(!camera_devices && !camera_fds && !encoders);
    assert(s_video.fd == -1 && !s_video.initialized && !s_video.task);
    host_reset();
    failure = FAIL_NONE;
    atomic_store(&callback_count, 0);
    atomic_store(&block_callback, false);
}
static void *initialize(void) {
    void *context = NULL;
    assert(video_init(&context, frame_handler, &callback_count) == 0 && context == &s_video);
    assert(host_tasks == 0 && atomic_load(&callback_count) == 0);
    host_expect_psram(s_video.jpeg);
    return context;
}
int main(void) {
    reset();
    for (int fail = FAIL_JPEG; fail <= FAIL_MAP; ++fail) {
        failure = fail;
        void *context = (void *)1;
        assert(video_init(&context, frame_handler, &callback_count) < 0 && !context);
        reset();
    }
    for (int fail = 1; fail <= 3; ++fail) {
        host_fail_alloc = fail;
        void *context = NULL;
        assert(video_init(&context, frame_handler, &callback_count) < 0 && !context);
        reset();
    }
    void *context = initialize();
    host_fail_task = 1;
    assert(video_start(context) < 0 && !s_video.task);
    video_destroy(context);
    reset();
    context = initialize();
    failure = FAIL_STREAM;
    assert(video_start(context) == 0);
    host_wait_until(worker_suspended);
    assert(atomic_load(&callback_count) == 0 && video_stop(context) == 0);
    failure = FAIL_NONE;
    video_destroy(context);
    reset();
    for (int repeat = 0; repeat < 3; ++repeat) {
        context = initialize();
        for (int restart = 0; restart < 2; ++restart) {
            atomic_store(&callback_count, 0);
            assert(video_start(context) == 0);
            host_wait_until(received_frame);
            assert(video_stop(context) == 0 && host_tasks == 0);
            unsigned int count = atomic_load(&callback_count);
            host_real_delay(2);
            assert(atomic_load(&callback_count) == count);
        }
        video_destroy(context);
        reset();
    }
    context = initialize();
    atomic_store(&block_callback, true);
    assert(video_start(context) == 0);
    host_wait_until(received_frame);
    assert(video_stop(context) < 0);
    video_destroy(context);
    assert(s_video.initialized && host_tasks == 1 && host_allocations == 4 && encoders == 1);
    atomic_store(&block_callback, false);
    assert(video_stop(context) == 0);
    video_destroy(context);
    reset();
    context = initialize();
    __atomic_store_n(&host_hold_suspend, 1, __ATOMIC_RELEASE);
    assert(video_start(context) == 0 && video_stop(context) < 0);
    assert(host_tasks == 1 && host_allocations == 4);
    __atomic_store_n(&host_hold_suspend, 0, __ATOMIC_RELEASE);
    assert(video_stop(context) == 0);
    video_destroy(context);
    reset();
    context = initialize();
    failure = FAIL_DEINIT;
    video_destroy(context);
    assert(!s_video.initialized && camera_devices == 1 && encoders == 1 && host_allocations == 1);
    failure = FAIL_NONE;
    context = initialize();
    video_destroy(context);
    reset();
    puts(
        "Video PSRAM allocation, init faults, callback timeout, restart and owner deletion passed");
    return 0;
}

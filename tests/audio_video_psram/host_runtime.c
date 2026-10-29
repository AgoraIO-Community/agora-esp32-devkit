/* SPDX-License-Identifier: Apache-2.0 */
#include "host_runtime.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/semphr.h"
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int host_fail_alloc, host_fail_task, host_hold_suspend, host_allocations, host_tasks,
    host_semaphores, host_psram_stacks;
static pthread_mutex_t allocation_lock = PTHREAD_MUTEX_INITIALIZER;
static struct {
    void *pointer;
    unsigned int caps;
} allocations[64];
struct host_task {
    pthread_t thread;
    TaskFunction_t function;
    void *argument;
    void *stack;
    int suspended;
};
static _Thread_local struct host_task *current_task;

void host_reset(void) {
    assert(host_allocations == 0 && host_tasks == 0 && host_semaphores == 0);
    host_fail_alloc = host_fail_task = host_hold_suspend = host_psram_stacks = 0;
}
void host_log(const char *format, ...) {
    (void)format;
}
const char *esp_err_to_name(esp_err_t error) {
    (void)error;
    return "host_error";
}
int64_t esp_timer_get_time(void) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return ((int64_t)time.tv_sec * 1000000 + time.tv_nsec / 1000) * 20;
}
void host_real_delay(unsigned int milliseconds) {
    struct timespec time = {.tv_sec = milliseconds / 1000,
                            .tv_nsec = (milliseconds % 1000) * 1000000};
    nanosleep(&time, NULL);
}
void host_wait_until(bool (*condition)(void)) {
    for (int i = 0; i < 5000; ++i) {
        if (condition())
            return;
        host_real_delay(1);
    }
    assert(!"worker did not reach expected state");
}
void *heap_caps_aligned_alloc(size_t alignment, size_t bytes, uint32_t caps) {
    pthread_mutex_lock(&allocation_lock);
    if (host_fail_alloc > 0 && --host_fail_alloc == 0) {
        pthread_mutex_unlock(&allocation_lock);
        return NULL;
    }
    void *pointer = NULL;
    assert(posix_memalign(&pointer, alignment < sizeof(void *) ? sizeof(void *) : alignment,
                          bytes) == 0);
    for (size_t i = 0; i < 64; ++i)
        if (!allocations[i].pointer) {
            allocations[i].pointer = pointer;
            allocations[i].caps = caps;
            ++host_allocations;
            pthread_mutex_unlock(&allocation_lock);
            return pointer;
        }
    abort();
}
void *heap_caps_calloc(size_t count, size_t bytes, uint32_t caps) {
    void *pointer = heap_caps_aligned_alloc(sizeof(void *), count * bytes, caps);
    if (pointer)
        memset(pointer, 0, count * bytes);
    return pointer;
}
void host_free(void *pointer) {
    if (!pointer)
        return;
    pthread_mutex_lock(&allocation_lock);
    for (size_t i = 0; i < 64; ++i)
        if (allocations[i].pointer == pointer) {
            allocations[i].pointer = NULL;
            --host_allocations;
            pthread_mutex_unlock(&allocation_lock);
            free(pointer);
            return;
        }
    assert(!"free of unknown allocation");
}
void heap_caps_free(void *pointer) {
    host_free(pointer);
}
void host_expect_psram(void *pointer) {
    pthread_mutex_lock(&allocation_lock);
    for (size_t i = 0; i < 64; ++i)
        if (allocations[i].pointer == pointer) {
            assert((allocations[i].caps & MALLOC_CAP_SPIRAM) != 0);
            assert((allocations[i].caps & MALLOC_CAP_INTERNAL) == 0);
            pthread_mutex_unlock(&allocation_lock);
            return;
        }
    assert(!"unknown PSRAM pointer");
}
size_t heap_caps_get_free_size(uint32_t caps) {
    (void)caps;
    return 1024 * 1024;
}
size_t heap_caps_get_largest_free_block(uint32_t caps) {
    return heap_caps_get_free_size(caps);
}
static SemaphoreHandle_t semaphore_init(StaticSemaphore_t *storage, unsigned int count) {
    assert(pthread_mutex_init(&storage->lock, NULL) == 0);
    assert(pthread_cond_init(&storage->ready, NULL) == 0);
    storage->count = count;
    storage->dynamic = 0;
    ++host_semaphores;
    return storage;
}
SemaphoreHandle_t xSemaphoreCreateMutexStatic(StaticSemaphore_t *storage) {
    return semaphore_init(storage, 1);
}
SemaphoreHandle_t xSemaphoreCreateBinaryStatic(StaticSemaphore_t *storage) {
    return semaphore_init(storage, 0);
}
SemaphoreHandle_t xSemaphoreCreateBinary(void) {
    StaticSemaphore_t *storage = calloc(1, sizeof(*storage));
    semaphore_init(storage, 0);
    storage->dynamic = 1;
    return storage;
}
BaseType_t xSemaphoreTake(SemaphoreHandle_t semaphore, TickType_t ticks) {
    assert(semaphore);
    pthread_mutex_lock(&semaphore->lock);
    struct timespec deadline;
    clock_gettime(CLOCK_REALTIME, &deadline);
    uint64_t ns = (uint64_t)deadline.tv_nsec + (uint64_t)ticks * 1000000 / 20;
    deadline.tv_sec += ns / 1000000000;
    deadline.tv_nsec = ns % 1000000000;
    while (!semaphore->count) {
        if (!ticks) {
            pthread_mutex_unlock(&semaphore->lock);
            return pdFALSE;
        }
        int result = ticks == portMAX_DELAY
                         ? pthread_cond_wait(&semaphore->ready, &semaphore->lock)
                         : pthread_cond_timedwait(&semaphore->ready, &semaphore->lock, &deadline);
        if (result) {
            pthread_mutex_unlock(&semaphore->lock);
            return pdFALSE;
        }
    }
    semaphore->count = 0;
    pthread_mutex_unlock(&semaphore->lock);
    return pdTRUE;
}
BaseType_t xSemaphoreGive(SemaphoreHandle_t semaphore) {
    assert(semaphore);
    pthread_mutex_lock(&semaphore->lock);
    semaphore->count = 1;
    pthread_cond_signal(&semaphore->ready);
    pthread_mutex_unlock(&semaphore->lock);
    return pdTRUE;
}
void vSemaphoreDelete(SemaphoreHandle_t semaphore) {
    assert(semaphore);
    int dynamic = semaphore->dynamic;
    assert(pthread_cond_destroy(&semaphore->ready) == 0);
    assert(pthread_mutex_destroy(&semaphore->lock) == 0);
    --host_semaphores;
    if (dynamic)
        free(semaphore);
}
void vTaskDelay(TickType_t ticks) {
    struct timespec time = {.tv_nsec = ticks ? (long)ticks * 1000000 / 20 : 1000};
    time.tv_sec = time.tv_nsec / 1000000000;
    time.tv_nsec %= 1000000000;
    nanosleep(&time, NULL);
}
void vTaskSuspend(TaskHandle_t task) {
    assert(!task && current_task);
    while (__atomic_load_n(&host_hold_suspend, __ATOMIC_ACQUIRE))
        host_real_delay(1);
    __atomic_store_n(&current_task->suspended, 1, __ATOMIC_RELEASE);
    for (;;) {
        host_real_delay(1000);
    }
}
static void *task_entry(void *opaque) {
    struct host_task *task = opaque;
    current_task = task;
    task->function(task->argument);
    assert(!"task returned");
    return NULL;
}
BaseType_t xTaskCreateWithCaps(TaskFunction_t function, const char *name, unsigned int stack_bytes,
                               void *argument, unsigned int priority, TaskHandle_t *task,
                               unsigned int caps) {
    (void)name;
    (void)priority;
    assert(task && (caps & MALLOC_CAP_SPIRAM) && !(caps & MALLOC_CAP_INTERNAL));
    if (host_fail_task)
        return pdFAIL;
    struct host_task *created = calloc(1, sizeof(*created));
    created->stack = heap_caps_calloc(stack_bytes, 1, caps);
    if (!created->stack) {
        free(created);
        return pdFAIL;
    }
    created->function = function;
    created->argument = argument;
    *task = created;
    ++host_tasks;
    ++host_psram_stacks;
    assert(pthread_create(&created->thread, NULL, task_entry, created) == 0);
    return pdPASS;
}
eTaskState eTaskGetState(TaskHandle_t task) {
    return __atomic_load_n(&task->suspended, __ATOMIC_ACQUIRE) ? eSuspended : eRunning;
}
void vTaskDeleteWithCaps(TaskHandle_t task) {
    assert(task && !pthread_equal(pthread_self(), task->thread));
    assert(eTaskGetState(task) == eSuspended);
    assert(pthread_cancel(task->thread) == 0);
    assert(pthread_join(task->thread, NULL) == 0);
    heap_caps_free(task->stack);
    free(task);
    --host_tasks;
}

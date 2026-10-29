/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "esp_heap_caps.h"
#include "freertos/idf_additions.h"
extern int host_fail_alloc;
extern int host_fail_task;
extern int host_hold_suspend;
extern int host_allocations;
extern int host_tasks;
extern int host_semaphores;
extern int host_psram_stacks;
void host_reset(void);
void host_wait_until(bool (*condition)(void));
void host_real_delay(unsigned int milliseconds);
void host_free(void *pointer);
void host_expect_psram(void *pointer);

/* SPDX-License-Identifier: MIT */
#pragma once
#include <stddef.h>
#include <stdint.h>
#define MALLOC_CAP_8BIT (1U << 2)
#define MALLOC_CAP_SPIRAM (1U << 10)
#ifdef __cplusplus
extern "C" {
#endif
void *heap_caps_calloc(size_t count, size_t bytes, uint32_t caps);
void heap_caps_free(void *pointer);
#ifdef __cplusplus
}
#endif

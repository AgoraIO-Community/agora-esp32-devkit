/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
#include <stddef.h>
#include <stdint.h>
#define MALLOC_CAP_INTERNAL 1
#define MALLOC_CAP_8BIT 2
#define MALLOC_CAP_SPIRAM 4
#define MALLOC_CAP_DMA 8
void *heap_caps_calloc(size_t count, size_t bytes, uint32_t caps);
void *heap_caps_aligned_alloc(size_t alignment, size_t bytes, uint32_t caps);
void heap_caps_free(void *pointer);
size_t heap_caps_get_free_size(uint32_t caps);
size_t heap_caps_get_largest_free_block(uint32_t caps);

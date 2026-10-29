/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
#include "task.h"
BaseType_t xTaskCreateWithCaps(TaskFunction_t function, const char *name, unsigned int stack_bytes,
                               void *argument, unsigned int priority, TaskHandle_t *task,
                               unsigned int caps);
void vTaskDeleteWithCaps(TaskHandle_t task);

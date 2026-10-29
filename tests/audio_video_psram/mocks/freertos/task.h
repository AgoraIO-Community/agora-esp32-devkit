/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
#include "FreeRTOS.h"
typedef struct host_task *TaskHandle_t;
typedef void (*TaskFunction_t)(void *);
typedef enum { eRunning, eSuspended } eTaskState;
eTaskState eTaskGetState(TaskHandle_t task);
void vTaskDelay(TickType_t ticks);
void vTaskSuspend(TaskHandle_t task);

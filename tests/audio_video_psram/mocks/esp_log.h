/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
void host_log(const char *format, ...);
#define ESP_LOGI(tag, ...) host_log(__VA_ARGS__)
#define ESP_LOGW(tag, ...) host_log(__VA_ARGS__)
#define ESP_LOGE(tag, ...) host_log(__VA_ARGS__)

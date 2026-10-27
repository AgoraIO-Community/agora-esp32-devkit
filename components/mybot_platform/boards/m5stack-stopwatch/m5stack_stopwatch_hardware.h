/* SPDX-License-Identifier: Apache-2.0 */
#ifndef MYBOT_M5STACK_STOPWATCH_HARDWARE_H_
#define MYBOT_M5STACK_STOPWATCH_HARDWARE_H_

#include "driver/i2c_master.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int mybot_m5stack_stopwatch_hardware_init(i2c_master_bus_handle_t bus);
int mybot_m5stack_stopwatch_set_speaker_pa(bool enabled);
int mybot_m5stack_stopwatch_hardware_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* MYBOT_M5STACK_STOPWATCH_HARDWARE_H_ */

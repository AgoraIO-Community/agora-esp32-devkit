/* SPDX-License-Identifier: Apache-2.0 */
#ifndef MYBOT_AMOLED18_HARDWARE_H_
#define MYBOT_AMOLED18_HARDWARE_H_

#include "driver/i2c_master.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int mybot_amoled18_hardware_init(void);
int mybot_amoled18_hardware_deinit(void);
i2c_master_bus_handle_t mybot_amoled18_i2c_bus_handle(void);
int mybot_amoled18_set_speaker_power(bool enabled);

#ifdef __cplusplus
}
#endif

#endif /* MYBOT_AMOLED18_HARDWARE_H_ */

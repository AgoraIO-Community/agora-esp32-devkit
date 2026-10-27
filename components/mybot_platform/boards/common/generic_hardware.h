/* SPDX-License-Identifier: Apache-2.0 */
#ifndef MYBOT_GENERIC_HARDWARE_H_
#define MYBOT_GENERIC_HARDWARE_H_

#include "driver/i2c_master.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int mybot_generic_hardware_init(void);
int mybot_generic_hardware_deinit(void);
i2c_master_bus_handle_t mybot_generic_audio_i2c_bus(void);
i2c_master_bus_handle_t mybot_generic_touch_i2c_bus(void);
int mybot_generic_set_speaker_power(bool enabled);
int mybot_board_set_display_backlight(unsigned int percent);

#ifdef __cplusplus
}
#endif

#endif /* MYBOT_GENERIC_HARDWARE_H_ */

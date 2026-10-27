/* SPDX-License-Identifier: Apache-2.0 */
#ifndef MYBOT_LCD185_HARDWARE_H_
#define MYBOT_LCD185_HARDWARE_H_

#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

int mybot_lcd185_hardware_init(void);
int mybot_lcd185_hardware_deinit(void);
i2c_master_bus_handle_t mybot_lcd185_touch_i2c_bus_handle(void);

#ifdef __cplusplus
}
#endif

#endif /* MYBOT_LCD185_HARDWARE_H_ */

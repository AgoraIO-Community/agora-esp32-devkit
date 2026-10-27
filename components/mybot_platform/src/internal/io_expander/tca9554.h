/* SPDX-License-Identifier: Apache-2.0 */
#ifndef MYBOT_TCA9554_H_
#define MYBOT_TCA9554_H_

#include "driver/i2c_master.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t device;
    uint8_t output_state;
} mybot_tca9554_t;

int mybot_tca9554_init(mybot_tca9554_t *expander, i2c_master_bus_handle_t bus, uint8_t address,
                       uint32_t speed_hz);
int mybot_tca9554_deinit(mybot_tca9554_t *expander);
int mybot_tca9554_configure_outputs(mybot_tca9554_t *expander, uint8_t output_mask,
                                    uint8_t high_mask);
int mybot_tca9554_set_levels(mybot_tca9554_t *expander, uint8_t mask, uint8_t high_mask);

#ifdef __cplusplus
}
#endif

#endif /* MYBOT_TCA9554_H_ */

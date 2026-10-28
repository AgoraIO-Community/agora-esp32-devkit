/* SPDX-License-Identifier: Apache-2.0 */
#ifndef MYBOT_AW9523B_H_
#define MYBOT_AW9523B_H_

#include "driver/i2c_master.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t device;
    uint16_t output_state;
} mybot_aw9523b_t;

int mybot_aw9523b_init(mybot_aw9523b_t *expander, i2c_master_bus_handle_t bus, uint8_t address,
                       uint32_t speed_hz);
int mybot_aw9523b_deinit(mybot_aw9523b_t *expander);
int mybot_aw9523b_configure_outputs(mybot_aw9523b_t *expander, uint16_t output_mask,
                                    uint16_t high_mask);
int mybot_aw9523b_set_levels(mybot_aw9523b_t *expander, uint16_t mask, uint16_t high_mask);

#ifdef __cplusplus
}
#endif

#endif /* MYBOT_AW9523B_H_ */

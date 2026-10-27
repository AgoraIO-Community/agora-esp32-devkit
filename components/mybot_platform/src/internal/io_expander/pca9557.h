/* SPDX-License-Identifier: Apache-2.0 */
#ifndef MYBOT_PCA9557_H_
#define MYBOT_PCA9557_H_

#include "driver/i2c_master.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t device;
    uint8_t output_state;
} mybot_pca9557_t;

int mybot_pca9557_init(mybot_pca9557_t *expander, i2c_master_bus_handle_t bus, uint8_t address,
                       uint32_t speed_hz);
int mybot_pca9557_deinit(mybot_pca9557_t *expander);
int mybot_pca9557_configure_outputs(mybot_pca9557_t *expander, uint8_t output_mask,
                                    uint8_t high_mask);
int mybot_pca9557_set_levels(mybot_pca9557_t *expander, uint8_t mask, uint8_t high_mask);

#ifdef __cplusplus
}
#endif

#endif /* MYBOT_PCA9557_H_ */

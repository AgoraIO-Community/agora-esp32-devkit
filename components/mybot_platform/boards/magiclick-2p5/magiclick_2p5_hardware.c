/* SPDX-License-Identifier: Apache-2.0 */
#include "board_config.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

#define TAG "magiclick_2p5"
#define MYBOT_COMPACT_PANEL_GC9107 1
#define MYBOT_COMPACT_PANEL_ST7735 2

int mybot_board_compact_panel_profile(void) {
    adc_oneshot_unit_handle_t adc = NULL;
    const adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = ADC_UNIT_1,
    };
    if (adc_oneshot_new_unit(&unit_config, &adc) != ESP_OK) {
        ESP_LOGW(TAG, "event=panel action=detect result=fallback reason=adc_init");
        return MYBOT_COMPACT_PANEL_ST7735;
    }
    const adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    if (adc_oneshot_config_channel(adc, ADC_CHANNEL_2, &channel_config) != ESP_OK) {
        (void)adc_oneshot_del_unit(adc);
        ESP_LOGW(TAG, "event=panel action=detect result=fallback reason=adc_channel");
        return MYBOT_COMPACT_PANEL_ST7735;
    }

    int total = 0;
    int raw = 0;
    for (int i = 0; i < 10; ++i) {
        if (adc_oneshot_read(adc, ADC_CHANNEL_2, &raw) != ESP_OK) {
            (void)adc_oneshot_del_unit(adc);
            return MYBOT_COMPACT_PANEL_ST7735;
        }
        total += raw;
    }
    (void)adc_oneshot_del_unit(adc);

    const int millivolts = (total / 10) * 3300 / 4095;
    if (millivolts < 100) {
        ESP_LOGI(TAG, "event=panel action=detect profile=gc9107 millivolts=%d", millivolts);
        return MYBOT_COMPACT_PANEL_GC9107;
    }
    ESP_LOGI(TAG, "event=panel action=detect profile=st7735 millivolts=%d", millivolts);
    return MYBOT_COMPACT_PANEL_ST7735;
}

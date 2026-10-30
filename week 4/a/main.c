

#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "ADC_EXAMPLE";

void app_main(void) {
    // 1. Declare handles and variables
    adc_oneshot_unit_handle_t handle;
    int raw = 0;

    // 2. Configure ADC Unit (Using ADC_UNIT_1 for GPIO 15 / CH3)
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_config, &handle);

    // 3. Configure channel (GPIO 15 = ADC1 Channel 3)
    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12, // Updated from DB_11 for 3.3V range
    };
    adc_oneshot_config_channel(handle, ADC_CHANNEL_3, &chan_cfg);

    // 4. Continuous read loop
    while (1) {
        adc_oneshot_read(handle, ADC_CHANNEL_3, &raw);
        ESP_LOGI(TAG, "Raw ADC value: %d", raw);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
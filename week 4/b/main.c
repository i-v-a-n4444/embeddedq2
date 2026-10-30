
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

static const char *TAG = "ADC_LAB4B";

#define POT_CHANNEL ADC_CHANNEL_3   //D15
#define LDR_CHANNEL ADC_CHANNEL_2   //D2

void app_main(void)
{
    adc_oneshot_unit_handle_t adc_handle;
    int pot_value = 0;
    int ldr_value = 0;

    // 1. Create ADC unit 
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    // 2. Configure ADC channels
    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };

    //saying to use same resolution & voltage
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, POT_CHANNEL, &chan_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, LDR_CHANNEL, &chan_config));

    // 3. Read and compare values
    while (1) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, POT_CHANNEL, &pot_value));
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, LDR_CHANNEL, &ldr_value));


        if (ldr_value < pot_value) {
            ESP_LOGI(TAG, "DAYTIME (LDR=%d, POT=%d)", ldr_value, pot_value);
        } else {
            ESP_LOGI(TAG, "NIGHTTIME (LDR=%d, POT=%d)", ldr_value, pot_value);
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second
    }
}
          



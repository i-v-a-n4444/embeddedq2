
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "aht.h"
#include "i2cdev.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define AHT20_ADDR 0x38

void app_main(void)
{
    i2cdev_init();

    // Force slow speed (important)
    i2cdev_set_speed(I2C_NUM_0, 100000);

    aht_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    ESP_ERROR_CHECK(aht_init_desc(&sensor, AHT20_ADDR, I2C_NUM_0, SDA_PIN, SCL_PIN));
    ESP_ERROR_CHECK(aht_init(&sensor));

    float temp, hum;

    while (1) {
        if (aht_get_data(&sensor, &temp, &hum) == ESP_OK) {
            printf("Temp: %.1f C | Hum: %.1f %%\n", temp, hum);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}



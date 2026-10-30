#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED 22
#define BUTTON 23

void app_main()
{
    gpio_reset_pin(LED);
    gpio_reset_pin(BUTTON);
    
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);

    int led_state = 0;
    int last_button = 1;

    while (true)
    {
        int button = gpio_get_level(BUTTON);

        if (button == 0 && last_button == 1)
        {
            led_state = !led_state;
            gpio_set_level(LED, led_state);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }

        last_button = button;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
    
    
    
  

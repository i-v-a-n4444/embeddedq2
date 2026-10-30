 //Name:Ivan Valko
 //Student number: 25121502

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED 22
#define BUTTON 23

void app_main()
{
    // Reset the pins
    gpio_reset_pin(LED);
    gpio_reset_pin(BUTTON);
        
    // Set LED as output
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    // Set button as input with pullup
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);

    // Track the LED and button states
    int led_state = 0;
    int last_button = 1;

    while (true)
    {
        // Check if button is pressed
        int button = gpio_get_level(BUTTON);
        // If button just got pressed, toggle the LED
        if (button == 0 && last_button == 1)
        {
            led_state = !led_state;
            gpio_set_level(LED, led_state);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }

        last_button = button; // Update previous button state
        vTaskDelay(10 / portTICK_PERIOD_MS); //  delay 
    }
}
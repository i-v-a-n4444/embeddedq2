#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#define LED     22
#define BUTTON  23
#define DEBOUNCE_TIME_MS 200

static const char *TAG = "INTERRUPT_BUTTON"; //name tag for logging 
static volatile uint32_t last_interrupt_time = 0; // last time pressed
static volatile int led_state = 0;


void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t current_time = esp_timer_get_time() / 1000; 
    if ((current_time - last_interrupt_time) > DEBOUNCE_TIME_MS) {
        led_state = !led_state;
        gpio_set_level(LED, led_state);


        last_interrupt_time = current_time;   
    }
}

void app_main(void) {
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    
    gpio_config_t button_conf = {
        .pin_bit_mask = (1ULL << BUTTON),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&button_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON, gpio_isr_handler, NULL);

    ESP_LOGI(TAG, "Interrupt-based button handling started.");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}
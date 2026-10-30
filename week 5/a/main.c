#include <stdio.h>
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

#define TAG "BLINK_TIMER"
#define LED_PIN GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_0 // Boot button

static gptimer_handle_t gptimer = NULL;
static bool led_on = false;

static bool IRAM_ATTR timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    // Toggle LED when 5 seconds expire
    led_on = !led_on;
    gpio_set_level(LED_PIN, led_on);
    return false;
}

void app_main(void) {
    // Configure LED
    gpio_config_t led_conf = {
        .pin_bit_mask = 1ULL << LED_PIN,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&led_conf);
    gpio_set_level(LED_PIN, 0);

    // Configure Button
    gpio_config_t btn_conf = {
        .pin_bit_mask = 1ULL << BUTTON_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&btn_conf);

    // Timer Config (1 MHz resolution -> 1 tick = 1 microsecond)
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000 * 1000,
    };
    gptimer_new_timer(&timer_config, &gptimer);

    // Callback config
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_callback,
    };
    gptimer_register_event_callbacks(gptimer, &cbs, NULL);

    // 5-second One-shot Alarm Configuration
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 5 * 1000 * 1000, // 5 seconds
        .reload_count = 0,
        .flags.auto_reload_on_alarm = false, // false = One-shot
    };
    gptimer_set_alarm_action(gptimer, &alarm_config);
    gptimer_enable(gptimer);

    ESP_LOGI(TAG, "Timer Initialized");

    while (1) {
        if (gpio_get_level(BUTTON_PIN) == 0) { // Pressed
            ESP_LOGI(TAG, "Button pressed, starting 5-second one-shot timer");
            gptimer_start(gptimer);

            vTaskDelay(pdMS_TO_TICKS(300)); // Debounce delay
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
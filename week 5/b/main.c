#include <stdio.h>
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

#define TAG "BLINK_TIMER"
#define LED_PIN GPIO_NUM_2

static gptimer_handle_t gptimer = NULL;
static bool led_on = false;

static bool IRAM_ATTR timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    // Toggle LED
    led_on = !led_on;
    gpio_set_level(LED_PIN, led_on);
    return false; // return true to yield after ISR
}

void app_main(void)
{
    // timer configuration structure
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000 * 1000, // 1mhz = 1 microsecond
    };
    gptimer_new_timer(&timer_config, &gptimer);
    // GPIO configuration of pin
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << LED_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);
    gpio_set_level(LED_PIN, 0); // off

    // Timer callback config
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_callback,
    }; // function to call on trigger
    gptimer_register_event_callbacks(gptimer, &cbs, NULL);

    // Set 500 ms alarm
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 500 * 1000, // 500,000 μs = 500 ms
        .reload_count = 0,         // Start value, usually 0
        .flags.auto_reload_on_alarm = true,
    }; // true or false; see below:
    // true → timer resets (to reload_count) and restarts counting automatically
    // false → one-shot: you need to manually restart the timer or reset the counter
    gptimer_set_alarm_action(gptimer, &alarm_config);
    gptimer_enable(gptimer);
    gptimer_start(gptimer);

    ESP_LOGI(TAG, "Blink started (1 Hz)");

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#include <stdio.h>
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#define TAG "HC_SR04P_LAB5C"
#define PIN_TRIG GPIO_NUM_23
#define PIN_ECHO GPIO_NUM_21

static gptimer_handle_t periodic_timer = NULL;
static gptimer_handle_t oneshot_timer = NULL;

// === ONE-SHOT TIMER #2 (10us) ===
static bool IRAM_ATTR oneshot_cb(gptimer_handle_t timer,
                                const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    gpio_set_level(PIN_TRIG, 0);
    return false;
}

// === PERIODIC TIMER #1 (500ms) ===
static bool IRAM_ATTR periodic_cb(gptimer_handle_t timer,
                                 const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    gpio_set_level(PIN_TRIG, 1);
    gptimer_set_raw_count(oneshot_timer, 0);
    gptimer_start(oneshot_timer);
    return true;
}

static void init_pins(void)
{
    gpio_config_t o = {
        .pin_bit_mask = 1ULL << PIN_TRIG,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = true,
        .pull_up_en = false,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&o);
    gpio_set_level(PIN_TRIG, 0);
   
    gpio_config_t i = {
        .pin_bit_mask = 1ULL << PIN_ECHO,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = false,
        .pull_up_en = true,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&i);
}

void app_main(void)
{
    // === DISABLE ALL WATCHDOGS ===
    esp_task_wdt_deinit();
   
    ESP_LOGI(TAG, "Lab 5c - 3 Timers (WDT DISABLED)");
   
    init_pins();
   
    // === TIMER #1: PERIODIC 500ms ===
    gptimer_config_t periodic_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&periodic_config, &periodic_timer));
    gptimer_event_callbacks_t periodic_cbs = {.on_alarm = periodic_cb};
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(periodic_timer, &periodic_cbs, NULL));
    gptimer_alarm_config_t periodic_alarm = {
        .alarm_count = 500 * 1000,
        .flags.auto_reload_on_alarm = true
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(periodic_timer, &periodic_alarm));
    ESP_ERROR_CHECK(gptimer_enable(periodic_timer));
    gptimer_start(periodic_timer);
   
    // === TIMER #2: ONE-SHOT 10us ===
    gptimer_config_t oneshot_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&oneshot_config, &oneshot_timer));
    gptimer_event_callbacks_t oneshot_cbs = {.on_alarm = oneshot_cb};
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(oneshot_timer, &oneshot_cbs, NULL));
    gptimer_alarm_config_t oneshot_alarm = {
        .alarm_count = 10,
        .flags.auto_reload_on_alarm = false
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(oneshot_timer, &oneshot_alarm));
    ESP_ERROR_CHECK(gptimer_enable(oneshot_timer));
   
    // === TIMER #3: esp_timer_get_time() ===
    ESP_LOGI(TAG, "✓ 3 TIMERS ACTIVE - Hand 20-50cm from sensor");
   
    // === YOUR PROVEN MEASUREMENT (500ms intervals) ===
    while (1) {
        // EXACTLY your working debug code
        gpio_set_level(PIN_TRIG, 1);
        esp_rom_delay_us(10);
        gpio_set_level(PIN_TRIG, 0);
       
        int64_t t_now = esp_timer_get_time();
        int64_t deadline_rise = t_now + 3000;
       
        while (gpio_get_level(PIN_ECHO) == 0) {
            if (esp_timer_get_time() > deadline_rise) goto next;
            esp_rom_delay_us(2);
        }
       
        int64_t t_start = esp_timer_get_time();
        int64_t deadline_fall = t_start + 25000;
       
        while (gpio_get_level(PIN_ECHO) == 1) {
            if (esp_timer_get_time() > deadline_fall) goto next;
            esp_rom_delay_us(2);
        }
       
        int64_t t_end = esp_timer_get_time();
        int64_t dt_us = t_end - t_start;
       
        if (dt_us > 100 && dt_us <= 25000) {
            float dist_cm = (float)dt_us * 0.01715f;
            ESP_LOGI(TAG, "✓ Distance: %.1f cm (echo=%lld us)", dist_cm, dt_us);
        }
       
    next:
        vTaskDelay(pdMS_TO_TICKS(450));  // 500ms total cycle
    }
}
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include <math.h>

#define POT_ADC_CHANNEL ADC_CHANNEL_3 // D15
#define RED_GPIO GPIO_NUM_23
#define BLUE_GPIO GPIO_NUM_21
#define GREEN_GPIO GPIO_NUM_22

static const char *TAG = "ADC_POLLING";

// Convert hue (0 <---> 8192) to RGB (0 <---> 8192), assuming Saturation = 1, Value = 1
void hue_to_rgb(int hue_, int *r, int *g, int *b)
{
    float hue = (float)hue_ / 11.3777777778;
    float c = 1.0f; // chroma = V * S = 1 * 1 = 1
    float x = 1.0f - fabsf(fmodf(hue / 60.0f, 2) - 1.0f);
    float m = 0.0f; // since V = 1, m = V - c = 0
    float rf = 0, gf = 0, bf = 0;
    if (hue < 0)
        hue = 0;
    if (hue >= 360)
        hue = fmodf(hue, 360.0f);
    if (hue < 60)
    {
        rf = c;
        gf = x;
        bf = 0;
    }
    else if (hue < 120)
    {
        rf = x;
        gf = c;
        bf = 0;
    }
    else if (hue < 180)
    {
        rf = 0;
        gf = c;
        bf = x;
    }
    else if (hue < 240)
    {
        rf = 0;
        gf = x;
        bf = c;
    }
    else if (hue < 300)
    {
        rf = x;
        gf = 0;
        bf = c;
    }
    else
    {
        rf = c;
        gf = 0;
        bf = x;
    }
    // Scale to 0–8192
    *r = (int)((rf + m) * 8192);
    *g = (int)((gf + m) * 8192);
    *b = (int)((bf + m) * 8192);
}

void app_main(void)
{
    // PWM set up

    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);
    // RED
    ledc_channel_config_t ledc_channel_red = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = RED_GPIO,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel_red);
    // BLUE
    ledc_channel_config_t ledc_channel_blue = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = BLUE_GPIO,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel_blue);
    // GREEN
    ledc_channel_config_t ledc_channel_green = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_2,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = GREEN_GPIO,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel_green);

    int duty = 0;
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);

    // ADC set up
    adc_oneshot_unit_handle_t adc_handle;
    int adc_raw;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    adc_oneshot_config_channel(adc_handle, POT_ADC_CHANNEL, &config);

    int r, g, b;

    while (1)
    {
        adc_oneshot_read(adc_handle, POT_ADC_CHANNEL, &adc_raw);

        int hue = adc_raw * 2;

        hue_to_rgb(hue, &r, &g, &b);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, r); // red
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, g); // green
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, b); // blue
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

static const char *TAG = "UART_CHAT";

#define UART_PORT     UART_NUM_0
#define UART_CHAT     UART_NUM_1
#define CHAT_TX       17
#define CHAT_RX       16
#define BUF_SIZE      1024

static void uart_init(void) {
    // Install driver for UART0 (USB serial)
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0));

    // Configure UART1
    uart_config_t cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_CHAT, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(UART_CHAT, CHAT_TX, CHAT_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_CHAT, BUF_SIZE * 2, 0, 0, NULL, 0));
}

void app_main(void) {
    uart_init();

    ESP_LOGI(TAG, "Type something and press Enter:");

    char outgoing_buffer[BUF_SIZE];
    char incoming_buffer[BUF_SIZE];
    int line_index = 0;

    while (true) {
        uint8_t ch;
        // Read 1 byte from keyboard (UART0)
        int len = uart_read_bytes(UART_PORT, &ch, 1, 20 / portTICK_PERIOD_MS);

        if (len > 0) {
            if (ch == '\n' || ch == '\r') {
                if (line_index > 0) {
                    outgoing_buffer[line_index] = '\0';

                    // 1. Send text out pin 17 (UART1 TX)
                    uart_write_bytes(UART_CHAT, outgoing_buffer, strlen(outgoing_buffer));

                    // 2. Read text back from pin 16 (UART1 RX)
                    int rx_len = uart_read_bytes(UART_CHAT, incoming_buffer, line_index, 100 / portTICK_PERIOD_MS);
                    if (rx_len > 0) {
                        incoming_buffer[rx_len] = '\0';
                        ESP_LOGI(TAG, "Received on UART1: \"%s\"", incoming_buffer);
                    }

                    // Clear buffer for next line
                    line_index = 0;
                }
            } else {
                if (line_index < BUF_SIZE - 1) {
                    outgoing_buffer[line_index] = (char)ch;
                    line_index++;
                }
            }
        }
    }
}

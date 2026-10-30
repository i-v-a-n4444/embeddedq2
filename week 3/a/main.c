#include "esp_log.h"
#include "driver/uart.h"
#include <string.h>

#define TAG "UART_ECHO"
#define UART_PORT UART_NUM_0
#define RX_BUF_SIZE 256     
#define LINE_BUF_SIZE 256   


static void uart0_init(void) {
    
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
   
}

void app_main(void)
{
   
    uart0_init();
    
    
    ESP_LOGI(TAG, "Type and press Enter");
    
   
    char line[LINE_BUF_SIZE];
    int line_index = 0; 
    
    
    while (true) {
        
        uint8_t data;  
        int len = uart_read_bytes(UART_PORT, &data, 1, 20 / portTICK_PERIOD_MS);
        
        
        if (len > 0) {
            
            if (data == '\n' || data == '\r') {
                
                if (line_index > 0) {
                    
                    line[line_index] = '\0';
                    
                    
                    ESP_LOGI(TAG, "Echo: \"%s\"", line);
                    
                    
                    line_index = 0;
                }
            } 
            else {
                
                
                
                if (line_index < LINE_BUF_SIZE - 1) {
                    line[line_index] = data;
                    line_index++;
                } else {
                
                    line[line_index] = '\0';
                    ESP_LOGI(TAG, "Echo (buffer full): \"%s\"", line);
                    line_index = 0;
                }
            }
        }
        
    }
}
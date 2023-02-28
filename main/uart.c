#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"

#include "uart_commands.h"
#include "memory.h"

static const char *TAG = "zcam:uart";

static void uart_event_task(void *pv_parameters)
{
    initialize_console();
    register_system_common();
    const char *prompt = "> ";
    while (true)
    {
        char* line = linenoise(prompt);
        if (line == NULL)
        {
            break;
        }
        if (strlen(line) > 0) 
        {
            linenoiseHistoryAdd(line);
        }

        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "unrecognized command");
        }
        else if (err == ESP_ERR_INVALID_ARG)
        {
            // cmd empty
        }
        else if (err == ESP_OK && ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret));
        }
        else if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Internal error: %s\n", esp_err_to_name(err));
        }
        linenoiseFree(line);
    }

    ESP_LOGE(TAG, "error or end-of-input, terminating console");
    esp_console_deinit();
}

void start_uart()
{
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
}


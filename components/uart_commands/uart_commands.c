#include <string.h>
#include <unistd.h>
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "esp_console.h"
#include "esp_system.h"

#include "uart_commands.h"
#include "memory.h"

#define BUF_SIZE 1024
#define UART_BAUD_RATE CONFIG_CONSOLE_UART_BAUDRATE
#define UART_PORT_NUM CONFIG_CONSOLE_UART_NUM

static char* TAG = "znet:uart_commands";

static struct {
    struct arg_str *key;
    struct arg_str *value; 
    struct arg_end *end;
} set_args;

void initialize_console(void)
{
    fflush(stdout);
    fsync(fileno(stdout));

    setvbuf(stdin, NULL, _IONBF, 0);

    esp_vfs_dev_uart_port_set_rx_line_endings(UART_PORT_NUM, ESP_LINE_ENDINGS_CR);
    esp_vfs_dev_uart_port_set_tx_line_endings(UART_PORT_NUM, ESP_LINE_ENDINGS_CRLF);

    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    esp_vfs_dev_uart_use_driver(UART_PORT_NUM);

    esp_console_config_t console_config = {
        .max_cmdline_args = 8,
        .max_cmdline_length = 256
    };

    ESP_ERROR_CHECK(esp_console_init(&console_config));

    linenoiseHistorySetMaxLen(10);
    linenoiseSetMaxLineLen(console_config.max_cmdline_length);
    linenoiseAllowEmpty(false);
}

static int restart(int argc, char* argv[])
{
    ESP_LOGI(TAG, "restarting");
    esp_restart();
}

static void register_restart(void)
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "software reset of the chip",
        .hint = NULL,
        .func = &restart
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int set(int argc, char* argv[])
{
    int nerrors = arg_parse(argc, argv, (void **) &set_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_args.end, argv[0]);
        return 1;
    }

    mem_store_str(set_args.key->sval[0], set_args.value->sval[0]);
    return 0;
}

static void register_set(void)
{
    set_args.key = arg_str1(NULL, NULL, "<key>", "key of data to be stored");
    set_args.value = arg_str1(NULL, NULL, "<value>", "value of data to be stored");
    set_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "set",
        .help = "set a configuration option",
        .hint = NULL,
        .func = &set,
        .argtable = &set_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void register_system_common(void)
{
    register_restart();
    register_set();
}
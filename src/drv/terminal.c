#include <stdint.h>
#include <string.h>
#ifndef RUUVI_ESP
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#else
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/projdefs.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <ctype.h>
#include <time.h>
#include "ruuvi_board_gwesp.h"
#endif
#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_ca_uart.h"

/***USER_INCLUDES***/
/*start*/
#include "debug.h"
#include "terminal.h"
#include "parser.h"
/*end*/

/**USER_TYPES***/
/*start*/
#pragma pack(push, 1)
typedef struct __terminal_struct
{
#ifndef RUUVI_ESP
    pthread_t thread_id;
    pthread_t thread_id_call;
    int       fd;
#else
    TaskHandle_t rx_parse_task_manager;
    TaskHandle_t rx_task_manager;
#endif
    re_ca_uart_parser_obj_t parser_obj;
} terminal_struct_t;
#pragma pack(pop)
/*end*/

/**USER_VARIABLES***/
/*start*/
terminal_struct_t terminal;
/*end*/

static re_ca_uart_parser_message_len_t
wait_message_from_uart(
    terminal_struct_t *const    p_terminal,
    const uint32_t              timeout,
    re_ca_uart_message_t *const p_message);

static int
send_msg(uint8_t *data, uint8_t size);

static int
send_msg(uint8_t *data, uint8_t size)
{
    int res = 0;
    print_dbgmsgnofuncnoarg("TX: ");
#ifndef RUUVI_ESP
    for (int i = 0; i < size; i++)
    {
        print_dbgmsgnofunc("0x%02x ", data[i]);
    }
#else
    print_dbgmsghexdump((char *)&data[0], size);
#endif
    print_dbgmsgnofuncnoarg("\n");
#ifndef RUUVI_ESP
    if (size != write(terminal.fd, &data[0], size))
#else
    if (size != uart_write_bytes(UART_NUM_1, (char *)&data[0], size))
#endif
    {
        print_errmsgnoarg("Write size incorrect\n");
        res = (-1);
    }

    return res;
}

static re_ca_uart_parser_message_len_t
wait_message_from_uart(
    terminal_struct_t *const    p_terminal,
    const uint32_t              timeout,
    re_ca_uart_message_t *const p_message)
{
    int rx_size_it;

    while (1)
    {
        uint8_t byte = 0;
#ifndef RUUVI_ESP
        rx_size_it = read(p_terminal->fd, &byte, 1);
#else
        rx_size_it = uart_read_bytes(UART_NUM_1, &byte, 1, timeout / portTICK_RATE_MS);
#endif
        if (1 != rx_size_it)
        {
            parser_handle_timeout(&p_terminal->parser_obj);
            continue;
        }

        if (parser_handle_byte(&p_terminal->parser_obj, byte))
        {
            return parser_get_message(&p_terminal->parser_obj, p_message);
        }
    }
}

int
terminal_send_msg(uint8_t *data, uint8_t size)
{
    int res = 0;
    print_dbgmsgnoarg("Enter\n");
    if (send_msg((uint8_t *)data, (uint8_t)size) != 0)
    {
        res = (-1);
    }
    print_dbgmsgnoarg("End\n");
    return res;
}

#ifndef RUUVI_ESP
void *
th_ctrl(void *vargp)
#else
static void
rx_task(void *arg)
#endif
{
    parser_init(&terminal.parser_obj);

#ifndef RUUVI_ESP
    while (terminal.fd < 0)
        ;
#endif
    re_ca_uart_message_t message;
    while (1)
    {
        re_ca_uart_parser_message_len_t len = wait_message_from_uart(&terminal, RX_ASK_TIMEOUT, &message);
        (void)len;
        print_dbgmsgnofuncnoarg("RX: ");

#ifndef RUUVI_ESP
        for (int i = 0; i < len; i++)
        {
            print_dbgmsgnofunc("0x%02x ", (*(uint8_t *)&message.buffer[0]));
        }
#else
        print_dbgmsghexdump((char *)&message.buffer[0], len);
#endif
        if ((-1) == parse((uint8_t *)&message.buffer[0]))
        {
            print_dbgmsgnofuncnoarg("RX: parsing failed");
        }
    }
}

int
terminal_open(char *device_address, bool rx_enable, int task_priority)
{
    print_dbgmsgnoarg("Enter\n");
    memset(&terminal, 0, sizeof(terminal_struct_t));
#ifndef RUUVI_ESP
    print_dbgmsg("Open UART: %s\n", device_address);
    terminal.fd = open(device_address, O_RDWR);
    if (terminal.fd < 0)
    {
        return (-1);
    }
    else
    {
        if (true == rx_enable)
        {
            struct termios settings;
            speed_t        baud = DEFAULT_BAUDRATE;
            tcgetattr(terminal.fd, &settings);
            cfsetospeed(&settings, baud); /* baud rate */
            settings.c_cflag &= ~PARENB;  /* no parity */
            settings.c_cflag &= ~CSTOPB;  /* 1 stop bit */
            settings.c_cflag &= ~CSIZE;
            settings.c_cflag |= CS8 | CLOCAL; /* 8 bits */
            settings.c_lflag = ICANON;        /* canonical mode */
            settings.c_oflag &= ~OPOST;       /* raw output */

            settings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
            settings.c_oflag &= ~OPOST;
            settings.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
            settings.c_cflag &= ~(CSIZE | PARENB);
            settings.c_cflag |= CS8;

            tcsetattr(terminal.fd, TCSANOW, &settings); /* apply the settings */
            tcflush(terminal.fd, TCOFLUSH);

            pthread_create(&terminal.thread_id, NULL, th_ctrl, NULL);
        }
    }
#else
    terminal.rx_parse_task_manager = NULL;
    terminal.rx_task_manager = NULL;
    const uart_config_t uart_config = {
        .data_bits = UART_DATA_8_BITS, //!< Only supported option my nRF52811
        .stop_bits = UART_STOP_BITS_1, //!< Only supported option by nRF52811
        .baud_rate = DEFAULT_BAUDRATE,
        .parity = RB_PARITY_ENABLED ? UART_PARITY_ODD : UART_PARITY_DISABLE, // XXX
        .flow_ctrl = RB_HWFC_ENABLED ? UART_HW_FLOWCTRL_CTS_RTS : UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM_1, &uart_config);
    // XXX Flow control pins not set by board definition.
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, UART_RX_BUF_SIZE * 2, 0, 0, NULL, 0);

    xTaskCreate(rx_task, "uart_rx_task", 1024 * 4, NULL, task_priority, &terminal.rx_task_manager);
#endif
    print_dbgmsgnoarg("End\n");
    return 0;
}

int
terminal_close(void)
{
    print_dbgmsgnoarg("Enter\n");
#ifndef RUUVI_ESP
    close(terminal.fd); /* cleanup */
#else
    if (true == uart_is_driver_installed(UART_NUM_1))
    {
        if (terminal.rx_task_manager)
        {
            vTaskDelete(terminal.rx_task_manager);
            terminal.rx_task_manager = NULL;
        }
        if (terminal.rx_parse_task_manager)
        {
            vTaskDelete(terminal.rx_parse_task_manager);
            terminal.rx_parse_task_manager = NULL;
        }
        uart_driver_delete(UART_NUM_1);
    }
#endif
    print_dbgmsgnoarg("End\n");
    return 0;
}

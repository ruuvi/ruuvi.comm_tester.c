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
    int     size;
    uint8_t rx_buffer[RX_BUFFER_MAX_SIZE];
    uint8_t rx_buffer_error[RX_BUFFER_MAX_SIZE << 1];
    uint8_t rx_buffer_error_index;
} terminal_struct_t;
#pragma pack(pop)
/*end*/

/**USER_VARIABLES***/
/*start*/
terminal_struct_t terminal;
/*end*/

static void
wait(terminal_struct_t *p_terminal, uint32_t timeout);
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

static void
wait(terminal_struct_t *p_terminal, uint32_t timeout)
{
    int rx_size = 0;
    int rx_size_it;

    static uint8_t rcv_data[RX_BUFFER_MAX_SIZE];
    memset(rcv_data, 0, RX_BUFFER_MAX_SIZE);
    while (p_terminal->size != 0)
    {
#ifndef RUUVI_ESP
#else
        vTaskDelay(1);
#endif
    }
    while (1)
    {
#ifndef RUUVI_ESP
        rx_size_it = read(p_terminal->fd, &rcv_data[rx_size], 1);
#else
        rx_size_it = uart_read_bytes(UART_NUM_1, &rcv_data[rx_size], 1, timeout / portTICK_RATE_MS);
#endif

        if ((rx_size + rx_size_it) > RX_BUFFER_MAX_SIZE)
        {
            break;
        }

        if (rx_size_it > 0)
        {
            rx_size += rx_size_it;
            for (int i = (rx_size - rx_size_it); i < rx_size; i++)
            {
                if ((*(uint8_t *)&rcv_data[i]) == RE_CA_UART_ETX)
                {
                    memcpy(p_terminal->rx_buffer, rcv_data, rx_size);
                    p_terminal->size = rx_size;
                    break;
                }
            }

            if (p_terminal->size)
            {
                break;
            }
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
th_ctrl_call(void *vargp)
#else
static void
rx_parse_task(void *arg)
#endif
{
#ifndef RUUVI_ESP
    while (terminal.fd < 0)
        ;
#endif
    while (1)
    {
        if (terminal.size)
        {
            print_dbgmsgnofuncnoarg("RX: ");

#ifndef RUUVI_ESP
            for (int i = 0; i < terminal.size; i++)
            {
                print_dbgmsgnofunc("0x%02x ", (*(uint8_t *)&terminal.rx_buffer[i]));
            }
#else
            print_dbgmsghexdump((char *)&terminal.rx_buffer[0], terminal.size);
#endif

            print_dbgmsgnofuncnoarg("\n");
            if ((-1) == parse((uint8_t *)&terminal.rx_buffer[0]))
            {
                memcpy((terminal.rx_buffer_error + terminal.rx_buffer_error_index), terminal.rx_buffer, terminal.size);
                terminal.rx_buffer_error_index += terminal.size;
                if (0 == parse((uint8_t *)&terminal.rx_buffer_error[0]))
                {
                    memset(terminal.rx_buffer_error, 0, RX_BUFFER_MAX_SIZE << 1);
                    terminal.rx_buffer_error_index = 0;
                }
            }
            else
            {
                memset(terminal.rx_buffer_error, 0, RX_BUFFER_MAX_SIZE << 1);
                terminal.rx_buffer_error_index = 0;
            }
            terminal.size = 0;
        }
#ifdef RUUVI_ESP
        vTaskDelay(1);
#endif
    }
}

#ifndef RUUVI_ESP
void *
th_ctrl(void *vargp)
#else
static void
rx_task(void *arg)
#endif
{
#ifndef RUUVI_ESP
    while (terminal.fd < 0)
        ;
#endif
    while (1)
    {
        wait(&terminal, RX_ASK_TIMEOUT);
#ifndef RUUVI_ESP
        // usleep(100000);
#else
        vTaskDelay(1);
#endif
    }
}

int
terminal_open(char *device_address, bool rx_enable)
{

    print_dbgmsgnoarg("Enter\n");
    memset(&terminal, 0, sizeof(terminal_struct_t));
#ifndef RUUVI_ESP
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

            terminal.size                  = 0;
            terminal.rx_buffer_error_index = 0;
            tcsetattr(terminal.fd, TCSANOW, &settings); /* apply the settings */
            tcflush(terminal.fd, TCOFLUSH);

            pthread_create(&terminal.thread_id, NULL, th_ctrl, NULL);
            pthread_create(&terminal.thread_id_call, NULL, th_ctrl_call, NULL);
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

    xTaskCreate(rx_task, "uart_rx_task", 1024 * 6, NULL, 1, &terminal.rx_task_manager);
    xTaskCreate(rx_parse_task, "rx_parse_task", 1024 * 4, NULL, 1, &terminal.rx_parse_task_manager);
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

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>

/***USER_INCLUDES***/
/*start*/
#include "types_def.h"
#include "terminal.h"
#include "api.h"
#include "debug.h"
#include "dbuscontroller.h"
#include "ruuvi_endpoint_ca_uart.h"
/*end*/

#define DEFAULT_DEVICE_COM  "/dev/ttyUSB0"
#define PAYLOAD_INPUT_YES   1
#define PAYLOAD_INPUT_NO    0
#define INVALID_PARAM_NUM   255
#define MAX_PARAMS_NUM_SIZE 255
#define IS_PAYLOAD_SET(x)   (x->is_set)

#define DEFAULT_FLTR_ID_NUM     0
#define DEFAULT_FLTR_TAGS_NUM   1
#define DEFAULT_CODED_PHY_NUM   2
#define DEFAULT_EXT_PAYLOAD_NUM 3
#define DEFAULT_SCAN_PHY_NUM    4
#define DEFAULT_CH_37_NUM       5
#define DEFAULT_CH_38_NUM       6
#define DEFAULT_CH_39_NUM       7

static void
help(void)
{
    print_logmsgnofuncnoarg("Help:\n");
    print_logmsgnofuncnoarg("-d : device COM port\n");
    print_logmsgnofuncnoarg("-i : set manufacturer ID filter value\n");
    print_logmsgnofuncnoarg("-f : set filter tags value\n");
    print_logmsgnofuncnoarg("-p : set coded PHY value\n");
    print_logmsgnofuncnoarg("-s : set scan 1MBbit/PHY value\n");
    print_logmsgnofuncnoarg("-e : set extended payload value\n");
    print_logmsgnofuncnoarg("-c7 : set channel 37 value\n");
    print_logmsgnofuncnoarg("-c8 : set channel 38 value\n");
    print_logmsgnofuncnoarg("-c9 : set channel 39 value\n");
    print_logmsgnofuncnoarg("-gi : get device id\n");
    print_logmsgnofuncnoarg("-l <ms> : turn on LED for <ms>\n");
    print_logmsgnofuncnoarg("-o : disable report output\n");
    print_logmsgnofuncnoarg("-r : run receiver mode\n");
    print_logmsgnofuncnoarg("-b : run receiver in backgroung mode\n");
    print_logmsgnofuncnoarg("-h : help\n");
}

static void
signalHandlerShutdown(int sig)
{
    terminal_close();
    exit(0);
}

typedef struct
{
    uint32_t payload;
    uint8_t  is_set;
} comm_tester_param_input_t;

typedef struct
{
    comm_tester_param_input_t fltr_id;
    comm_tester_param_input_t fltr_tags_state;
    comm_tester_param_input_t coded_phy_state;
    comm_tester_param_input_t ext_payload_state;
    comm_tester_param_input_t scan_phy_state;
    comm_tester_param_input_t ch_37_state;
    comm_tester_param_input_t ch_38_state;
    comm_tester_param_input_t ch_39_state;
    uint32_t                  all_state;
} comm_tester_input_t;

comm_tester_input_t        in;
comm_tester_param_input_t *in_array[] = {
    &in.fltr_id,        &in.fltr_tags_state, &in.coded_phy_state, &in.ext_payload_state,
    &in.scan_phy_state, &in.ch_37_state,     &in.ch_38_state,     &in.ch_39_state,
};

uint32_t cmd_array[] = {
    RE_CA_UART_SET_FLTR_ID,      RE_CA_UART_SET_FLTR_TAGS, RE_CA_UART_SET_CODED_PHY, RE_CA_UART_SET_EXT_PAYLOAD,
    RE_CA_UART_SET_SCAN_1MB_PHY, RE_CA_UART_SET_CH_37,     RE_CA_UART_SET_CH_38,     RE_CA_UART_SET_CH_39,
};

static void
set_param(comm_tester_param_input_t *p_in, char *argv)
{
    p_in->payload = atoi(argv);
    p_in->is_set  = PAYLOAD_INPUT_YES;
}

int
main(int argc, char *argv[])
{
    int      res           = 0;
    uint32_t i             = 0;
    char    *deviceCom     = DEFAULT_DEVICE_COM;
    uint8_t  mode          = 0;
    uint8_t  rx            = 0;
    uint8_t  get_device_id = 0;
    int      led_ctrl      = -1;
    uint32_t param_num;
    bool     dbus_create   = false;
    uint8_t  ignore_report = 0;
    print_dbgmsgnoarg("Enter\n");

    while (i < argc)
    {
        if (argv[i][0] == '-')
        {
            param_num = INVALID_PARAM_NUM;
            switch (argv[i][1])
            {
                case 'D':
                case 'd':
                    if ((argv[i + 1]) != NULL)
                        deviceCom = argv[i + 1];
                    break;
                case 'b':
                case 'B':
                    mode = 1;
                    break;
                case 'r':
                case 'R':
                    rx          = 1;
                    dbus_create = true;
                    break;
                case 'o':
                case 'O':
                    ignore_report = 1;
                    break;
                case 'f':
                case 'F':
                    param_num = DEFAULT_FLTR_TAGS_NUM;
                    break;
                case 'i':
                case 'I':
                    param_num = DEFAULT_FLTR_ID_NUM;
                    break;
                case 'p':
                case 'P':
                    param_num = DEFAULT_CODED_PHY_NUM;
                    break;
                case 's':
                case 'S':
                    param_num = DEFAULT_SCAN_PHY_NUM;
                    break;
                case 'e':
                case 'E':
                    param_num = DEFAULT_EXT_PAYLOAD_NUM;
                    break;
                case 'l':
                case 'L':
                    led_ctrl = atoi(argv[i + 1]);
                case 'g':
                case 'G':
                    switch (argv[i][2])
                    {
                        case 'I':
                        case 'i':
                            get_device_id = 1;
                            break;
                        default:
                            break;
                    }
                    break;
                case 'c':
                case 'C':
                    switch (argv[i][2])
                    {
                        case '7':
                            param_num = DEFAULT_CH_37_NUM;
                            break;
                        case '8':
                            param_num = DEFAULT_CH_38_NUM;
                            break;
                        case '9':
                            param_num = DEFAULT_CH_39_NUM;
                            break;
                        default:
                            break;
                    }
                    break;
                case 'H':
                case 'h':
                default:
                    help();
                    return (-1);
            }
            if ((param_num != INVALID_PARAM_NUM) && ((argv[i + 1]) != NULL))
            {
                in.all_state |= (1U << param_num);
                set_param(in_array[param_num], argv[i + 1]);
            }
        }
        i++;
    }

    if (0 == dbus_connect(dbus_create))
    {

        if (rx)
        {
            if (mode)
            {
                daemon(0, 1);
            }
            signal(SIGTERM, signalHandlerShutdown);
            signal(SIGHUP, signalHandlerShutdown);
            signal(SIGUSR1, signalHandlerShutdown);
            signal(SIGQUIT, signalHandlerShutdown);
            signal(SIGINT, signalHandlerShutdown);
            signal(SIGKILL, signalHandlerShutdown);

            if (terminal_open(deviceCom, true) == 0)
            {
                res = api_process(ignore_report);
                terminal_close();
            }
            else
            {
                print_errmsgnoarg("No device\n");
                res = (-1);
            }
        }
        else
        {
            if (terminal_open(deviceCom, false) == 0)
            {

                if (in.all_state >= MAX_PARAMS_NUM_SIZE)
                {
                    res = api_send_all(
                        RE_CA_UART_SET_ALL,
                        (uint16_t)in_array[DEFAULT_FLTR_ID_NUM]->payload,
                        (uint8_t)in_array[DEFAULT_FLTR_TAGS_NUM]->payload,
                        (uint8_t)in_array[DEFAULT_CODED_PHY_NUM]->payload,
                        (uint8_t)in_array[DEFAULT_EXT_PAYLOAD_NUM]->payload,
                        (uint8_t)in_array[DEFAULT_SCAN_PHY_NUM]->payload,
                        (uint8_t)in_array[DEFAULT_CH_37_NUM]->payload,
                        (uint8_t)in_array[DEFAULT_CH_38_NUM]->payload,
                        (uint8_t)in_array[DEFAULT_CH_39_NUM]->payload);
                }
                else
                {
                    for (uint8_t ii = 0; ii < (sizeof(in_array) / sizeof(in_array[0])); ii++)
                    {
                        if (IS_PAYLOAD_SET(in_array[ii]))
                        {
                            if (cmd_array[ii] == RE_CA_UART_SET_FLTR_ID)
                            {
                                res = api_send_fltr_id(cmd_array[ii], (uint16_t)in_array[ii]->payload);
                            }
                            else
                            {
                                res = api_send_bool_payload(cmd_array[ii], (uint8_t)in_array[ii]->payload);
                            }
                        }
                    }
                }

                if (get_device_id)
                {
                    res = api_send_get_device_id(RE_CA_UART_GET_DEVICE_ID);
                }

                if (led_ctrl >= 0)
                {
                    res = api_send_led_ctrl((uint16_t)led_ctrl);
                }

                terminal_close();
            }
            else
            {
                print_errmsgnoarg("No device\n");
                res = (-1);
            }
        }
    }
    print_dbgmsgnoarg("End\n");
    return res;
}

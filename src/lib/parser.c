#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

/***USER_INCLUDES***/
/*start*/
#include "debug.h"
#include "types_def.h"
#include "parser.h"
#include "ruuvi_endpoint_ca_uart.h"
/*end*/

/***USER_VARIABLES***/
/*start*/
api_callbacks_fn_t parser_callback_func_tbl_null = {
    .ApiAckCallback    = NULL,
    .ApiReportCallback = NULL,
    .ApiIdCallback     = NULL,
    .ApiGetAllCallback = NULL,
};

api_callbacks_fn_t *p_parser_callback_func_tbl = &parser_callback_func_tbl_null;
/*end*/

/***USER_STATIC_FUNCTIONS***/
/*start*/
/*end*/
int
parse_callbacks_unreg()
{
    print_dbgmsgnoarg("Enter\n");
    p_parser_callback_func_tbl = &parser_callback_func_tbl_null;
    print_dbgmsgnoarg("End\n");
    return 0;
}

int
parse_callbacks_reg(void *p_callback)
{
    int res = 0;
    print_dbgmsgnoarg("Enter\n");
    if ((api_callbacks_fn_t *)p_callback != NULL)
    {
        p_parser_callback_func_tbl = (api_callbacks_fn_t *)p_callback;
    }
    else
    {
        print_errmsgnofuncnoarg("Nullptr\n");
        res = (-1);
    }
    print_dbgmsgnoarg("End\n");
    return res;
}

int
parse(uint8_t *buffer)
{
    int res = -1;
    switch (buffer[RE_CA_UART_CMD_INDEX])
    {
        case RE_CA_UART_ACK:
            if (NULL != p_parser_callback_func_tbl->ApiAckCallback)
            {
                res = p_parser_callback_func_tbl->ApiAckCallback(buffer);
            }
            break;
        case RE_CA_UART_ADV_RPRT:
            if (NULL != p_parser_callback_func_tbl->ApiAckCallback)
            {
                res = p_parser_callback_func_tbl->ApiReportCallback(buffer);
            }
            break;
        case RE_CA_UART_DEVICE_ID:
            if (NULL != p_parser_callback_func_tbl->ApiAckCallback)
            {
                res = p_parser_callback_func_tbl->ApiIdCallback(buffer);
            }
            break;
        case RE_CA_UART_GET_ALL:
            if (NULL != p_parser_callback_func_tbl->ApiGetAllCallback)
            {
                res = p_parser_callback_func_tbl->ApiGetAllCallback(buffer);
            }
            break;
        default:
            break;
    }
    return res;
}

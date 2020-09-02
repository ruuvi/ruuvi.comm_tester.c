#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/***USER_INCLUDES***/
/*start*/
#include "types_def.h"
#include "debug.h"
#include "terminal.h"
#include "parser.h"
#include "time.h"
#include "ruuvi_endpoint_ca_uart.h"
#ifdef RUUVI_ESP
#include "adv_post.h"
#endif
/*end*/

/***USER_DEFINES***/
/*start*/
#define BUFFER_PAYLOAD_SIZE 204
/*end*/

/***USER_TYPES**/
/*start*/
/*end*/

/***USER_STATIC_FUNCTIONS**/
/*start*/
static int
api_ack_callback(const uint8_t *const buffer);
static int
api_report_callback(const uint8_t *const buffer);
/*end*/

/***USER_VARIABLES***/
/*start*/
api_callbacks_fn_t parser_callback_func_tbl = {
    .ApiAckCallback    = api_ack_callback,
    .ApiReportCallback = api_report_callback,
};

__u8 report_state = 0;

/*end*/

__s8
api_send_bool_payload(__u32 cmd, __u8 state)
{
    int8_t               res          = 0;
    re_ca_uart_payload_t uart_payload = { 0 };
    uint8_t              data[BUFFER_PAYLOAD_SIZE];
    uint8_t              data_length;

    print_dbgmsgnoarg("Enter\n");

    uart_payload.cmd = (re_ca_uart_cmd_t)cmd;
    if ((bool)state == true)
    {
        uart_payload.params.bool_param.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.bool_param.state = RE_CA_BOOL_DISABLE;
    }
    data_length = sizeof(data);

    if (RE_SUCCESS != re_ca_uart_encode(data, &data_length, &uart_payload))
    {
        res = (-1);
    }
    else
    {
        terminal_send_msg((uint8_t *)data, data_length);
    }
    print_dbgmsgnoarg("End\n");
    return (__s8)res;
}

__s8
api_send_fltr_id(__u32 cmd, __u16 id)
{
    int8_t               res          = 0;
    re_ca_uart_payload_t uart_payload = { 0 };
    uint8_t              data[BUFFER_PAYLOAD_SIZE];
    uint8_t              data_length;

    print_dbgmsgnoarg("Enter\n");

    uart_payload.cmd                     = (re_ca_uart_cmd_t)cmd;
    uart_payload.params.fltr_id_param.id = id;
    data_length                          = sizeof(data);

    if (RE_SUCCESS != re_ca_uart_encode(data, &data_length, &uart_payload))
    {
        res = (-1);
    }
    else
    {
        terminal_send_msg((uint8_t *)data, data_length);
    }
    print_dbgmsgnoarg("End\n");
    return (__s8)res;
}

__s8
api_send_all(
    __u32 cmd,
    __u16 fltr_id,
    __u8  fltr_tags_state,
    __u8  coded_phy_state,
    __u8  ext_payload_state,
    __u8  scan_phy_state,
    __u8  ch_37_state,
    __u8  ch_38_state,
    __u8  ch_39_state)
{
    int8_t               res          = 0;
    re_ca_uart_payload_t uart_payload = { 0 };
    uint8_t              data[BUFFER_PAYLOAD_SIZE];
    uint8_t              data_length;

    print_dbgmsgnoarg("Enter\n");

    uart_payload.cmd                          = (re_ca_uart_cmd_t)cmd;
    uart_payload.params.all_params.fltr_id.id = fltr_id;

    if ((bool)fltr_tags_state == true)
    {
        uart_payload.params.all_params.bools.fltr_tags.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.all_params.bools.fltr_tags.state = RE_CA_BOOL_DISABLE;
    }

    if ((bool)coded_phy_state == true)
    {
        uart_payload.params.all_params.bools.coded_phy.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.all_params.bools.coded_phy.state = RE_CA_BOOL_DISABLE;
    }

    if ((bool)ext_payload_state == true)
    {
        uart_payload.params.all_params.bools.ext_payload.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.all_params.bools.ext_payload.state = RE_CA_BOOL_DISABLE;
    }

    if ((bool)scan_phy_state == true)
    {
        uart_payload.params.all_params.bools.scan_phy.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.all_params.bools.scan_phy.state = RE_CA_BOOL_DISABLE;
    }

    if ((bool)ch_37_state == true)
    {
        uart_payload.params.all_params.bools.ch_37.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.all_params.bools.ch_37.state = RE_CA_BOOL_DISABLE;
    }

    if ((bool)ch_38_state == true)
    {
        uart_payload.params.all_params.bools.ch_38.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.all_params.bools.ch_38.state = RE_CA_BOOL_DISABLE;
    }

    if ((bool)ch_39_state == true)
    {
        uart_payload.params.all_params.bools.ch_39.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.all_params.bools.ch_39.state = RE_CA_BOOL_DISABLE;
    }

    data_length = sizeof(data);

    if (RE_SUCCESS != re_ca_uart_encode(data, &data_length, &uart_payload))
    {
        res = (-1);
    }
    else
    {
        terminal_send_msg((uint8_t *)data, data_length);
    }
    print_dbgmsgnoarg("End\n");
    return (__s8)res;
}

/***USER_CALLBACKS**/
/*start*/
static int
api_ack_callback(const __u8 *const buffer)
{
    int                  res                   = -1;
    re_ca_uart_payload_t uart_payload          = { 0 };
    char                 ack_state_name[2][32] = {
        "RE_CA_ACK_OK",
        "RE_CA_ACK_ERROR",
    };
    char ack_cmd_name[33][32] = {
        "RE_CA_UART_NOT_CODED",
        "RE_CA_UART_SET_FLTR",
        "RE_CA_UART_CLR_FLTR",
        "RE_CA_UART_SET_CH",
        "RE_CA_UART_SET_PHY",
        "RE_CA_UART_SET_FLTR_TAGS",
        "RE_CA_UART_SET_FLTR_ID",
        "RE_CA_UART_SET_CODED_PHY",
        "RE_CA_UART_SET_SCAN_1MB_PHY",
        "RE_CA_UART_SET_EXT_PAYLOAD",
        "RE_CA_UART_SET_CH_37",
        "RE_CA_UART_SET_CH_38",
        "RE_CA_UART_SET_CH_39",
        "",
        "",
        "RE_CA_UART_SET_ALL",
        "RE_CA_UART_ADV_RPRT",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "RE_CA_UART_ACK",
    };

    if (RE_SUCCESS == re_ca_uart_decode((uint8_t *)buffer, &uart_payload))
    {
        res = 0;
        print_logmsgnofuncnoarg("-----ACK-----\n");
        print_logmsgnofunc("CMD: %s\n", ack_cmd_name[(uint8_t)uart_payload.params.ack.cmd]);
        print_logmsgnofunc("ACK state: %s\n", ack_state_name[(uint8_t)uart_payload.params.ack.ack_state.state]);
    }
    return res;
}

static int
api_report_callback(const __u8 *const buffer)
{
    int                  res          = -1;
    re_ca_uart_payload_t uart_payload = { 0 };
    if (RE_SUCCESS == re_ca_uart_decode((uint8_t *)buffer, &uart_payload))
    {
        res = 0;
#ifdef RUUVI_ESP
        adv_post_send((void *)&uart_payload);
#endif
        if (!report_state)
        {
            print_logmsgnofuncnoarg("-----REPORT-----\n");
            print_logmsgnofunc(
                "MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                uart_payload.params.adv.mac[0],
                uart_payload.params.adv.mac[1],
                uart_payload.params.adv.mac[2],
                uart_payload.params.adv.mac[3],
                uart_payload.params.adv.mac[4],
                uart_payload.params.adv.mac[5]);

            print_logmsgnofuncnoarg("ADV: ");
#ifndef RUUVI_ESP
            for (int i = 0; i < uart_payload.params.adv.adv_len; i++)
            {
                print_logmsgnofunc("%02x ", uart_payload.params.adv.adv[i]);
            }
#else
            print_logmsghexdump((char *)&uart_payload.params.adv.adv[0], uart_payload.params.adv.adv_len);
#endif
            print_logmsgnofuncnoarg("\n");
            print_logmsgnofunc("RSSI: %d db\n", uart_payload.params.adv.rssi_db);
        }
    }
    return res;
}

/*end*/

__s8
api_process(__u8 state)
{
    print_dbgmsgnoarg("Enter\n");
    report_state = state;
    parse_callbacks_reg((void *)&parser_callback_func_tbl);
#ifndef RUUVI_ESP
    while (1)
    {
    }
    parse_callbacks_unreg();
#endif
    print_dbgmsgnoarg("End\n");
    return 0;
}

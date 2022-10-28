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
#include "formated_output.h"
#include "ruuvi_endpoint_ca_uart.h"
/*end*/

/***USER_VARIABLES***/
/*start*/
/*end*/

/***USER_STATIC_FUNCTIONS***/
/*start*/
/*end*/

void
formated_output_ack(void *p_uart_payload)
{
    re_ca_uart_payload_t *uart_payload          = (re_ca_uart_payload_t *)p_uart_payload;
    char                  ack_state_name[2][32] = {
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
    print_logmsgnofuncnoarg("-----ACK-----\n");
    print_logmsgnofunc("CMD: %s\n", ack_cmd_name[(uint8_t)uart_payload->params.ack.cmd]);
    print_logmsgnofunc("ACK state: %s\n", ack_state_name[(uint8_t)uart_payload->params.ack.ack_state.state]);
}

void
formated_output_report(void *p_uart_payload)
{
    re_ca_uart_payload_t *uart_payload = (re_ca_uart_payload_t *)p_uart_payload;
    print_logmsgnofuncnoarg("-----REPORT-----\n");
    print_logmsgnofunc(
        "MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
        uart_payload->params.adv.mac[0],
        uart_payload->params.adv.mac[1],
        uart_payload->params.adv.mac[2],
        uart_payload->params.adv.mac[3],
        uart_payload->params.adv.mac[4],
        uart_payload->params.adv.mac[5]);

    print_logmsgnofuncnoarg("ADV: ");
#ifndef RUUVI_ESP
    for (int i = 0; i < uart_payload->params.adv.adv_len; i++)
    {
        print_logmsgnofunc("%02x ", uart_payload->params.adv.adv[i]);
    }
#else
    print_logmsghexdump((char *)&uart_payload->params.adv.adv[0], uart_payload->params.adv.adv_len);
#endif
    print_logmsgnofuncnoarg("\n");
    print_logmsgnofunc("RSSI: %d db\n", uart_payload->params.adv.rssi_db);
}

void
formated_output_device_id(void *p_uart_payload)
{
    re_ca_uart_payload_t *uart_payload = (re_ca_uart_payload_t *)p_uart_payload;
    print_logmsgnofuncnoarg("-----DEVICE_ID-----\n");
    print_logmsgnofunc(
        "ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
        (*((uint8_t *)&uart_payload->params.device_id.id)),
        (*((uint8_t *)&uart_payload->params.device_id.id + 1)),
        (*((uint8_t *)&uart_payload->params.device_id.id + 2)),
        (*((uint8_t *)&uart_payload->params.device_id.id + 3)),
        (*((uint8_t *)&uart_payload->params.device_id.id + 4)),
        (*((uint8_t *)&uart_payload->params.device_id.id + 5)),
        (*((uint8_t *)&uart_payload->params.device_id.id + 6)),
        (*((uint8_t *)&uart_payload->params.device_id.id + 7)));
    print_logmsgnofunc(
        "ADDR: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
        (*((uint8_t *)&uart_payload->params.device_id.addr)),
        (*((uint8_t *)&uart_payload->params.device_id.addr + 1)),
        (*((uint8_t *)&uart_payload->params.device_id.addr + 2)),
        (*((uint8_t *)&uart_payload->params.device_id.addr + 3)),
        (*((uint8_t *)&uart_payload->params.device_id.addr + 4)),
        (*((uint8_t *)&uart_payload->params.device_id.addr + 5)),
        (*((uint8_t *)&uart_payload->params.device_id.addr + 6)),
        (*((uint8_t *)&uart_payload->params.device_id.addr + 7)));
}

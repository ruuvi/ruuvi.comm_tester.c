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
#include "formated_output.h"
#include "dbuscontroller.h"
#include "ruuvi_endpoint_ca_uart.h"
/*end*/

/***USER_DEFINES***/
/*start*/
/*end*/

/***USER_TYPES**/
/*start*/
/*end*/

/***USER_STATIC_FUNCTIONS**/
/*start*/
static int
api_ack_callback(const uint8_t *const buffer);
static int
api_id_callback(const uint8_t *const buffer);
static int
api_report_callback(const uint8_t *const buffer);
static int
api_get_all_callback(const uint8_t *const buffer);
/*end*/

/***USER_VARIABLES***/
/*start*/
api_callbacks_fn_t parser_callback_func_tbl = {
    .ApiAckCallback    = api_ack_callback,
    .ApiReportCallback = api_report_callback,
    .ApiIdCallback     = api_id_callback,
    .ApiGetAllCallback = api_get_all_callback,
};

adv_callbacks_fn_t adv_callback_func_tbl_null = {
    .AdvAckCallback    = NULL,
    .AdvReportCallback = NULL,
    .AdvIdCallback     = NULL,
    .AdvGetAllCallback = NULL,
};

adv_callbacks_fn_t *p_adv_callback_func_tbl = &adv_callback_func_tbl_null;

static bool g_flag_dont_print_output_report = false;

/*end*/

int8_t
api_send_bool_payload(uint32_t cmd, uint8_t state)
{
    int8_t                                     res          = 0;
    re_ca_uart_payload_t                       uart_payload = { 0 };
    re_ca_uart_mosi_payload_buf_encoded_bool_t data;
    uint8_t                                    data_length = sizeof(data.buf);

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

    if (RE_SUCCESS != re_ca_uart_encode(data.buf, &data_length, &uart_payload))
    {
        res = (-1);
    }
    else
    {
        terminal_send_msg((uint8_t *)data.buf, data_length);
#ifndef RUUVI_ESP
        dbus_check_new_messages();
#endif
    }
    print_dbgmsgnoarg("End\n");
    return (int8_t)res;
}

int8_t
api_send_get_device_id(uint32_t cmd)
{
    int8_t                                              res          = 0;
    re_ca_uart_payload_t                                uart_payload = { 0 };
    re_ca_uart_mosi_payload_buf_encoded_get_device_id_t data;
    uint8_t                                             data_length = sizeof(data.buf);

    print_dbgmsgnoarg("Enter\n");
    uart_payload.cmd = (re_ca_uart_cmd_t)cmd;

    if (RE_SUCCESS != re_ca_uart_encode(data.buf, &data_length, &uart_payload))
    {
        res = (-1);
    }
    else
    {
        terminal_send_msg((uint8_t *)data.buf, data_length);
#ifndef RUUVI_ESP
        dbus_check_new_messages();
#endif
    }
    print_dbgmsgnoarg("End\n");
    return (int8_t)res;
}

int8_t
api_send_fltr_id(uint32_t cmd, uint16_t id)
{
    int8_t                                        res          = 0;
    re_ca_uart_payload_t                          uart_payload = { 0 };
    re_ca_uart_mosi_payload_buf_encoded_set_fltr_id_t data;
    uint8_t                                       data_length = sizeof(data.buf);

    print_dbgmsgnoarg("Enter\n");

    uart_payload.cmd                     = (re_ca_uart_cmd_t)cmd;
    uart_payload.params.fltr_id_param.id = id;

    if (RE_SUCCESS != re_ca_uart_encode(data.buf, &data_length, &uart_payload))
    {
        res = (-1);
    }
    else
    {
        terminal_send_msg((uint8_t *)data.buf, data_length);
#ifndef RUUVI_ESP
        dbus_check_new_messages();
#endif
    }
    print_dbgmsgnoarg("End\n");
    return (int8_t)res;
}

int8_t
api_send_all(
    uint32_t cmd,
    uint16_t fltr_id,
    uint8_t  fltr_tags_state,
    uint8_t  use_coded_phy,
    uint8_t  use_2m_phy,
    uint8_t  use_1m_phy,
    uint8_t  ch_37_state,
    uint8_t  ch_38_state,
    uint8_t  ch_39_state,
    uint8_t  max_adv_len)
{
    int8_t               res          = 0;
    re_ca_uart_payload_t uart_payload = { 0 };
    re_ca_uart_mosi_payload_buf_encoded_all_params_t data;
    uint8_t              data_length = sizeof(data.buf);

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

    if ((bool)use_coded_phy == true)
    {
        uart_payload.params.all_params.bools.use_coded_phy.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.all_params.bools.use_coded_phy.state = RE_CA_BOOL_DISABLE;
    }

    if ((bool)use_2m_phy == true)
    {
        uart_payload.params.all_params.bools.use_2m_phy.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.all_params.bools.use_2m_phy.state = RE_CA_BOOL_DISABLE;
    }

    if ((bool)use_1m_phy == true)
    {
        uart_payload.params.all_params.bools.use_1m_phy.state = RE_CA_BOOL_ENABLE;
    }
    else
    {
        uart_payload.params.all_params.bools.use_1m_phy.state = RE_CA_BOOL_DISABLE;
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

    uart_payload.params.all_params.max_adv_len = max_adv_len;

    if (RE_SUCCESS != re_ca_uart_encode(data.buf, &data_length, &uart_payload))
    {
        res = (-1);
    }
    else
    {
        terminal_send_msg((uint8_t *)data.buf, data_length);
#ifndef RUUVI_ESP
        dbus_check_new_messages();
#endif
    }
    print_dbgmsgnoarg("End\n");
    return (int8_t)res;
}

int8_t
api_send_led_ctrl(const uint16_t time_interval_ms)
{
    int8_t                                         res          = 0;
    re_ca_uart_payload_t                           uart_payload = { 0 };
    re_ca_uart_mosi_payload_buf_encoded_led_ctrl_t data;
    uint8_t                                        data_length = sizeof(data.buf);

    print_dbgmsgnoarg("Enter\n");

    uart_payload.cmd                                    = RE_CA_UART_LED_CTRL;
    uart_payload.params.led_ctrl_param.time_interval_ms = time_interval_ms;

    if (RE_SUCCESS != re_ca_uart_encode(data.buf, &data_length, &uart_payload))
    {
        res = (-1);
    }
    else
    {
        terminal_send_msg((uint8_t *)data.buf, data_length);
#ifndef RUUVI_ESP
        dbus_check_new_messages();
#endif
    }
    print_dbgmsgnoarg("End\n");
    return (int8_t)res;
}

/***USER_CALLBACKS**/
/*start*/
static int
api_ack_callback(const uint8_t *const buffer)
{
    int                  res          = -1;
    re_ca_uart_payload_t uart_payload = { 0 };

    if (RE_SUCCESS == re_ca_uart_decode((uint8_t *)buffer, &uart_payload))
    {
        res = 0;
        if (NULL != p_adv_callback_func_tbl->AdvAckCallback)
        {
            p_adv_callback_func_tbl->AdvAckCallback((void *)&uart_payload);
        }
        formated_output_ack((void *)&uart_payload);
    }
    return res;
}

static int
api_id_callback(const uint8_t *const buffer)
{
    int                  res          = -1;
    re_ca_uart_payload_t uart_payload = { 0 };

    if (RE_SUCCESS == re_ca_uart_decode((uint8_t *)buffer, &uart_payload))
    {
        res = 0;
        if (NULL != p_adv_callback_func_tbl->AdvIdCallback)
        {
            p_adv_callback_func_tbl->AdvIdCallback((void *)&uart_payload);
        }
        formated_output_device_id((void *)&uart_payload);
    }
    return res;
}

static int
api_report_callback(const uint8_t *const buffer)
{
    int                  res           = -1;
    re_ca_uart_payload_t uart_payload  = { 0 };
    const re_status_t    decode_status = re_ca_uart_decode((uint8_t *)buffer, &uart_payload);
    if (RE_SUCCESS == decode_status)
    {
        res = 0;
        if (NULL != p_adv_callback_func_tbl->AdvReportCallback)
        {
            p_adv_callback_func_tbl->AdvReportCallback((void *)&uart_payload);
        }
        if (!g_flag_dont_print_output_report)
        {
            formated_output_report((void *)&uart_payload);
        }
    }
    else
    {
        print_dbgmsg("re_ca_uart_decode failed, status=%d\n", decode_status);
    }
    return res;
}

static int
api_get_all_callback(const uint8_t *const buffer)
{
    int                  res          = -1;
    re_ca_uart_payload_t uart_payload = { 0 };

    if (RE_SUCCESS == re_ca_uart_decode((uint8_t *)buffer, &uart_payload))
    {
        res = 0;
        if (NULL != p_adv_callback_func_tbl->AdvGetAllCallback)
        {
            p_adv_callback_func_tbl->AdvGetAllCallback((void *)&uart_payload);
        }
        print_logmsgnofuncnoarg("-----GET_ALL-----\n");
    }
    return res;
}

/*end*/

int8_t
api_process(const bool flag_dont_print_output_report)
{
    print_dbgmsgnoarg("Enter\n");
    g_flag_dont_print_output_report = flag_dont_print_output_report;
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

int
api_callbacks_reg(void *p_callback)
{
    int res = 0;
    print_dbgmsgnoarg("Enter\n");
    if ((adv_callbacks_fn_t *)p_callback != NULL)
    {
        p_adv_callback_func_tbl = (adv_callbacks_fn_t *)p_callback;
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
api_callbacks_unreg()
{
    print_dbgmsgnoarg("Enter\n");
    p_adv_callback_func_tbl = &adv_callback_func_tbl_null;
    print_dbgmsgnoarg("End\n");
    return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
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

#define U16_LSB_MASK   0x00FF
#define U16_MSB_MASK   0xFF00
#define U16_MSB_OFFSET 8
#define U8_HALF_OFFSET 4

static bool g_flag_ready = false;

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
parse(const uint8_t *const p_buffer)
{
    int res = -1;
    switch (p_buffer[RE_CA_UART_CMD_INDEX])
    {
        case RE_CA_UART_ACK:
            if (NULL != p_parser_callback_func_tbl->ApiAckCallback)
            {
                res = p_parser_callback_func_tbl->ApiAckCallback(p_buffer);
            }
            break;
        case RE_CA_UART_ADV_RPRT:
            if (NULL != p_parser_callback_func_tbl->ApiReportCallback)
            {
                res = p_parser_callback_func_tbl->ApiReportCallback(p_buffer);
            }
            break;
        case RE_CA_UART_DEVICE_ID:
            if (NULL != p_parser_callback_func_tbl->ApiIdCallback)
            {
                res = p_parser_callback_func_tbl->ApiIdCallback(p_buffer);
            }
            break;
        case RE_CA_UART_GET_ALL:
            if (NULL != p_parser_callback_func_tbl->ApiGetAllCallback)
            {
                res = p_parser_callback_func_tbl->ApiGetAllCallback(p_buffer);
            }
            break;
        default:
            break;
    }
    return res;
}

void
parser_init(re_ca_uart_parser_obj_t *const p_state)
{
    p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_STX;
    p_state->first_idx        = 0;
    p_state->last_idx         = 0;
    p_state->handled_idx      = 0;
    p_state->data_len         = 0;
    p_state->accum_len        = 0;
    memset(&p_state->message, 0, sizeof(p_state->message));
}

static inline re_ca_uart_parser_buffer_offset_t
re_ca_uart_parser_buffer_next_idx(const re_ca_uart_parser_buffer_offset_t idx)
{
    return (idx < (RE_CA_UART_PARSER_BUFFER_SIZE - 1)) ? (idx + 1) : 0;
}

static uint16_t
parser_calc_crc(re_ca_uart_parser_obj_t *const p_state)
{
    uint16_t crc = RE_CA_CRC_DEFAULT;
    for (re_ca_uart_parser_buffer_offset_t idx = re_ca_uart_parser_buffer_next_idx(p_state->first_idx);
         idx != p_state->handled_idx;
         idx = re_ca_uart_parser_buffer_next_idx(idx))
    {
        crc = (unsigned char)(crc >> U16_MSB_OFFSET) | (crc << U16_MSB_OFFSET);
        crc ^= p_state->message.buffer[idx];
        crc ^= (unsigned char)(crc & U16_LSB_MASK) >> U8_HALF_OFFSET;
        crc ^= (crc << U16_MSB_OFFSET) << U8_HALF_OFFSET;
        crc ^= ((crc & U16_LSB_MASK) << U8_HALF_OFFSET) << 1;
    }
    return crc;
}

bool
parser_handle_next(re_ca_uart_parser_obj_t *const p_state)
{
    const uint8_t byte = p_state->message.buffer[p_state->handled_idx];
    switch (p_state->re_ca_uart_state)
    {
        case RE_CA_UART_PARSER_STATE_WAIT_STX:
            if (RE_CA_UART_STX == byte)
            {
                p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_LEN;
                p_state->data_len         = 0;
                p_state->accum_len        = 0;
                p_state->handled_idx      = re_ca_uart_parser_buffer_next_idx(p_state->handled_idx);
            }
            else
            {
                p_state->first_idx   = re_ca_uart_parser_buffer_next_idx(p_state->first_idx);
                p_state->handled_idx = p_state->first_idx;
                if (g_flag_ready)
                {
                    print_dbgmsg("WAIT_STX: byte=0x%02x, move first idx to %u\n", byte, p_state->first_idx);
                }
            }
            break;
        case RE_CA_UART_PARSER_STATE_WAIT_LEN:
            if (byte <= RE_CA_UART_PARSER_RI_COMM_MESSAGE_MAX_LENGTH)
            {
                p_state->data_len         = byte;
                p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_CMD;
                p_state->handled_idx      = re_ca_uart_parser_buffer_next_idx(p_state->handled_idx);
            }
            else
            {
                p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_STX;
                p_state->first_idx        = re_ca_uart_parser_buffer_next_idx(p_state->first_idx);
                p_state->handled_idx      = p_state->first_idx;
                if (g_flag_ready)
                {
                    print_dbgmsg("WAIT_LEN: len=%u", byte);
                }
            }
            break;
        case RE_CA_UART_PARSER_STATE_WAIT_CMD:
            p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_DATA;
            p_state->handled_idx      = re_ca_uart_parser_buffer_next_idx(p_state->handled_idx);
            break;
        case RE_CA_UART_PARSER_STATE_WAIT_DATA:
        {
            const re_ca_uart_parser_buffer_offset_t buf_len             = (p_state->last_idx >= p_state->handled_idx)
                                                                              ? (p_state->last_idx - p_state->handled_idx)
                                                                              : (RE_CA_UART_PARSER_BUFFER_SIZE + p_state->last_idx
                                                                     - p_state->handled_idx);
            const re_ca_uart_parser_buffer_offset_t remained_len        = p_state->data_len - p_state->accum_len;
            const re_ca_uart_parser_buffer_offset_t num_bytes_to_handle = (buf_len < remained_len) ? buf_len
                                                                                                   : remained_len;
            p_state->accum_len += num_bytes_to_handle;
            p_state->handled_idx += num_bytes_to_handle;
            if (p_state->handled_idx >= RE_CA_UART_PARSER_BUFFER_SIZE)
            {
                p_state->handled_idx -= RE_CA_UART_PARSER_BUFFER_SIZE;
            }
            if (p_state->accum_len == p_state->data_len)
            {
                p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_CRC1;
                p_state->calc_crc         = parser_calc_crc(p_state);
            }
            break;
        }

        case RE_CA_UART_PARSER_STATE_WAIT_CRC1:
            p_state->crc              = byte;
            p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_CRC2;
            p_state->handled_idx      = re_ca_uart_parser_buffer_next_idx(p_state->handled_idx);
            break;

        case RE_CA_UART_PARSER_STATE_WAIT_CRC2:
            p_state->crc |= (uint16_t)((uint16_t)byte << 8U);
            if (p_state->calc_crc != p_state->crc)
            {
                p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_STX;
                p_state->first_idx        = re_ca_uart_parser_buffer_next_idx(p_state->first_idx);
                p_state->handled_idx      = p_state->first_idx;
                if (g_flag_ready)
                {
                    print_dbgmsg("WAIT_CRC2: exp=0x%04x, act=0x%04x", p_state->calc_crc, p_state->crc);
                }
            }
            else
            {
                p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_ETX;
                p_state->handled_idx      = re_ca_uart_parser_buffer_next_idx(p_state->handled_idx);
            }
            break;

        case RE_CA_UART_PARSER_STATE_WAIT_ETX:
            if (RE_CA_UART_ETX == byte)
            {
                p_state->handled_idx      = re_ca_uart_parser_buffer_next_idx(p_state->handled_idx);
                p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_FINISHED;
                return true;
            }
            p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_STX;
            p_state->first_idx        = re_ca_uart_parser_buffer_next_idx(p_state->first_idx);
            p_state->handled_idx      = p_state->first_idx;
            if (g_flag_ready)
            {
                print_dbgmsg("WAIT_ETX: byte=0x%02x", byte);
            }
            break;

        case RE_CA_UART_PARSER_STATE_FINISHED:
            assert(0);
            break;
    }
    return false;
}

bool
parser_handle_accumulated_data(re_ca_uart_parser_obj_t *const p_state)
{
    while (p_state->handled_idx != p_state->last_idx)
    {
        if (parser_handle_next(p_state))
        {
            return true;
        }
    }
    return false;
}

bool
parser_handle_byte(re_ca_uart_parser_obj_t *const p_state, const uint8_t byte)
{
    p_state->message.buffer[p_state->last_idx] = byte;
    p_state->last_idx                          = re_ca_uart_parser_buffer_next_idx(p_state->last_idx);
    return parser_handle_accumulated_data(p_state);
}

re_ca_uart_parser_message_len_t
parser_get_message(re_ca_uart_parser_obj_t *const p_state, re_ca_uart_message_t *const p_message)
{
    assert(RE_CA_UART_PARSER_STATE_FINISHED == p_state->re_ca_uart_state);
    re_ca_uart_parser_buffer_offset_t idx = p_state->first_idx;
    re_ca_uart_parser_message_len_t   len = 0;
    while (idx != p_state->handled_idx)
    {
        p_message->buffer[len++] = p_state->message.buffer[idx];
        idx                      = re_ca_uart_parser_buffer_next_idx(idx);
    }

    p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_STX;
    p_state->first_idx        = p_state->handled_idx;

    g_flag_ready = true;

    return len;
}

void
parser_handle_timeout(re_ca_uart_parser_obj_t *const p_state)
{
    if (g_flag_ready)
    {
        print_dbgmsg("parser_handle_timeout");
    }
    p_state->re_ca_uart_state = RE_CA_UART_PARSER_STATE_WAIT_STX;
    p_state->first_idx        = 0;
    p_state->last_idx         = 0;
    p_state->handled_idx      = 0;
    p_state->data_len         = 0;
    p_state->accum_len        = 0;
}

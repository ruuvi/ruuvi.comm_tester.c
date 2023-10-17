/**
 * Parser helper.
 * Parse incomming data from linux terminal
 *
 * License: BSD-3
 * Author: Oleg Protasevich
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef RUUVI_ESP
#include <linux/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parser state
 */
typedef enum parser_state_e
{
    RE_CA_UART_PARSER_STATE_WAIT_STX,  //!< Wait for STX
    RE_CA_UART_PARSER_STATE_WAIT_LEN,  //!< Wait for LEN
    RE_CA_UART_PARSER_STATE_WAIT_CMD,  //!< Wait for CMD
    RE_CA_UART_PARSER_STATE_WAIT_DATA, //!< Wait for data
    RE_CA_UART_PARSER_STATE_WAIT_CRC1, //!< Wait for the first byte of CRC
    RE_CA_UART_PARSER_STATE_WAIT_CRC2, //!< Wait for the second byte of CRC
    RE_CA_UART_PARSER_STATE_WAIT_ETX,  //!< Wait for ETX
    RE_CA_UART_PARSER_STATE_FINISHED,  //!< Finished
} re_ca_uart_parser_state_e;

/**
 * @brief Standard BLE Broadcast manufacturer specific data payload maximum length
 */
#define RE_CA_UART_PARSER_RI_COMM_MESSAGE_MAX_LENGTH 230

/**
 * @brief Parser buffer size
 */
#define RE_CA_UART_PARSER_BUFFER_SIZE (3 + RE_CA_UART_PARSER_RI_COMM_MESSAGE_MAX_LENGTH + 2 + 1 + 1)

/**
 * @brief Parser message buffer
 */
typedef struct re_ca_uart_message_t
{
    uint8_t buffer[RE_CA_UART_PARSER_BUFFER_SIZE];
} re_ca_uart_message_t;

typedef uint32_t re_ca_uart_parser_buffer_offset_t; //!< Offset in the Parser buffer
typedef uint32_t re_ca_uart_parser_message_len_t;   //!< Length of the message
typedef uint8_t  re_ca_uart_parser_data_len_t;      //!< Length of the data

/**
 * @brief Parser object
 */
typedef struct re_ca_uart_parser_obj_t
{
    re_ca_uart_parser_state_e         re_ca_uart_state;
    re_ca_uart_message_t              message;
    re_ca_uart_parser_buffer_offset_t first_idx;
    re_ca_uart_parser_buffer_offset_t last_idx;
    re_ca_uart_parser_buffer_offset_t handled_idx;
    re_ca_uart_parser_data_len_t      data_len;
    re_ca_uart_parser_data_len_t      accum_len;
    uint16_t                          calc_crc;
    uint16_t                          crc;
} re_ca_uart_parser_obj_t;

/***USER_FUNCTIONS***/
/*start*/

int
parse_callbacks_reg(void *p_callback);

int
parse_callbacks_unreg(void);

int
parse(const uint8_t *const p_buffer);

void
parser_init(re_ca_uart_parser_obj_t *const p_state);

bool
parser_handle_byte(re_ca_uart_parser_obj_t *const p_state, const uint8_t byte);

void
parser_handle_timeout(re_ca_uart_parser_obj_t *const p_state);

re_ca_uart_parser_message_len_t
parser_get_message(re_ca_uart_parser_obj_t *const p_state, re_ca_uart_message_t *const p_message);

/*end*/

#ifdef __cplusplus
}
#endif

#endif /* PARSER_H_ */

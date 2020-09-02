/**
 * Terminal helper.
 * Defines some params to use linux terminal.
 *
 * License: BSD-3
 * Author: Oleg Protasevich
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_
#ifndef RUUVI_ESP
#include <linux/types.h>
#else
#include "ruuvi_gateway.h"
#endif
/***USER_DEFINES***/
/*start*/
#ifndef RUUVI_ESP
#define ONE_SYMBOL_DELAY   1 // us
#define RX_BUFFER_MAX_SIZE 1024
#define STEP_RX_TIMEOUT    1000    // us
#define RX_ASK_TIMEOUT     5000    // in 1000us
#define DEFAULT_BAUDRATE   B115200 // in 1000us
#else
#define ONE_SYMBOL_DELAY   1 // us
#define RX_BUFFER_MAX_SIZE 1024
#define STEP_RX_TIMEOUT    1000   // us
#define RX_ASK_TIMEOUT     1000   // in 1000us
#define DEFAULT_BAUDRATE   115200 // in 1000us
#define UART_RX_BUF_SIZE   1024
#define BUF_MAX            1000
#endif
/*end*/

/**USER FUNCTIONS***/
/*start*/
int
terminal_open(char *device_address);
int
terminal_close(void);
int
terminal_send_msg(__u8 *data, __u8 size);
/*end*/

#endif /* TERMINAL_H_ */

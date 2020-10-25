/**
 * Formated output helper.
 * Format output data for linux terminal
 *
 * License: BSD-3
 * Author: Oleg Protasevich
 */

#ifndef FORMATED_OUTPUT_H_
#define FORMATED_OUTPUT_H_

#include <stdint.h>

#ifndef RUUVI_ESP
#include <linux/types.h>
#endif

/***USER_FUNCTIONS***/
/*start*/
void
formated_output_ack(void *p_uart_payload);
void
formated_output_device_id(void *p_uart_payload);
void
formated_output_report(void *p_uart_payload);
/*end*/

#endif /* FORMATED_OUTPUT_H_ */

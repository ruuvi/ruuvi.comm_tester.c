/**
 * Api helper.
 * Api to use ruuvi_endpoints_ca_uart with linux terminal
 *
 * License: BSD-3
 * Author: Oleg Protasevich
 */

#ifndef API_H_
#define API_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef RUUVI_ESP
#include <linux/types.h>
#endif

/***USER_FUNCTIONS**/
/*start*/

int8_t
api_process(const bool flag_dont_print_output_report);

int8_t
api_send_get_device_id(uint32_t cmd);

int8_t
api_send_fltr_id(uint32_t cmd, uint16_t id);

int8_t
api_send_bool_payload(uint32_t cmd, uint8_t state);

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
    uint8_t  max_adv_len);

int8_t
api_send_led_ctrl(const uint16_t time_interval_ms);

int
api_callbacks_reg(void *p_callback);

int
api_callbacks_unreg();

/*end*/

#endif /* API_H_ */

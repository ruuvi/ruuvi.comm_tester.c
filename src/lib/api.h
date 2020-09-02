/**
 * Api helper.
 * Api to use ruuvi_endpoints_ca_uart with linux terminal
 *
 * License: BSD-3
 * Author: Oleg Protasevich
 */

#ifndef API_H_
#define API_H_

#ifndef RUUVI_ESP
#include <linux/types.h>
#else
#include "ruuvi_gateway.h"
#endif

/***USER_FUNCTIONS**/
/*start*/
__s8
api_process(__u8 state);
__s8
api_send_fltr_id(__u32 cmd, __u16 id);
__s8
api_send_bool_payload(__u32 cmd, __u8 state);
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
    __u8  ch_39_state);
/*end*/

#endif /* API_H_ */

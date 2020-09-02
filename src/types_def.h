/**
 * Types helper.
 * Defines specific types for api
 *
 * License: BSD-3
 * Author: Oleg Protasevich
 */

#ifndef TYPES_DEF_H_
#define TYPES_DEF_H_

#ifndef RUUVI_ESP
#include <linux/types.h>
#else
#include "ruuvi_gateway.h"
#endif

typedef struct __api_callbacks_fn_t
{
    int (*ApiAckCallback)(const __u8 *const buffer);
    int (*ApiReportCallback)(const __u8 *const buffer);
} api_callbacks_fn_t;

#endif /* TYPES_DEF_H_ */

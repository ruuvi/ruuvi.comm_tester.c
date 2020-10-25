/**
 * Types helper.
 * Defines specific types for api
 *
 * License: BSD-3
 * Author: Oleg Protasevich
 */

#ifndef TYPES_DEF_H_
#define TYPES_DEF_H_

#include <stdint.h>

#ifndef RUUVI_ESP
#include <linux/types.h>
#endif

// DBus comm_tester settings
#define COMM_TESTER_DBUS_SERVICE   "com.ruuvi.comm.tester.c"
#define COMM_TESTER_DBUS_OBJECT    "/com/ruuvi/comm/tester/c"
#define COMM_TESTER_DBUS_INTERFACE COMM_TESTER_DBUS_SERVICE

// DBus signals
#define COMM_TESTER_DBUS_SEND_DEVICE_ID "sendDeviceId"
#define COMM_TESTER_DBUS_SEND_ACK       "sendAck"
#define COMM_TESTER_DBUS_SEND_REPORT    "sendReport"

typedef struct __api_callbacks_fn_t
{
    int (*ApiAckCallback)(const uint8_t *const buffer);
    int (*ApiReportCallback)(const uint8_t *const buffer);
    int (*ApiIdCallback)(const uint8_t *const buffer);
} api_callbacks_fn_t;

typedef struct __adv_callbacks_fn_t
{
    void (*AdvAckCallback)(void *arg);
    void (*AdvReportCallback)(void *arg);
    void (*AdvIdCallback)(void *arg);
} adv_callbacks_fn_t;

#endif /* TYPES_DEF_H_ */

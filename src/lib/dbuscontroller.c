#ifndef RUUVI_ESP
#include <stdio.h>
#include <time.h>
#include <dbus/dbus.h>
#include "types_def.h"
#include "debug.h"
#include "api.h"
#include "dbuscontroller.h"
#include "formated_output.h"
#include "ruuvi_endpoint_ca_uart.h"

#define DBUS_WAIT_TIME 5.0f // seconds

static void
dbus_send_ack(void *arg);
static void
dbus_send_device_id(void *arg);

static DBusConnection *conn;

adv_callbacks_fn_t adv_callback_func_tbl = {
    .AdvAckCallback    = dbus_send_ack,
    .AdvReportCallback = NULL,
    .AdvIdCallback     = dbus_send_device_id,
    .AdvGetAllCallback = NULL,
};

int
dbus_connect(bool new)
{
    int       ret = 0;
    DBusError err;
    char      match[256];
    char      match2[256];
    print_dbgmsgnoarg("Enter\n");

    dbus_error_init(&err);
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err))
    {
        print_errmsg("Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }

    if (conn == NULL)
    {
        ret = (-1);
    }
    else
    {
        if (true == new)
        {
            ret = dbus_bus_request_name(conn, COMM_TESTER_DBUS_SERVICE, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
            if (dbus_error_is_set(&err))
            {
                print_errmsg("Name Error (%s)\n", err.message);
                dbus_error_free(&err);
            }

            if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
            {
                ret = (-1);
            }
            else
            {
                ret = 0;
            }
        }
        else
        {
            snprintf(
                match,
                sizeof(match),
                "type='signal',interface='%s',member='%s'",
                COMM_TESTER_DBUS_INTERFACE,
                COMM_TESTER_DBUS_SEND_ACK);
            dbus_bus_add_match(conn, match, &err);
            //            dbus_connection_flush(conn);
            if (dbus_error_is_set(&err))
            {
                print_errmsg("Connection Error (%s)\n", err.message);
                ret = (-1);
            }
            // memset(match,0, sizeof(match));
            snprintf(
                match2,
                sizeof(match2),
                "type='signal',interface='%s',member='%s'",
                COMM_TESTER_DBUS_INTERFACE,
                COMM_TESTER_DBUS_SEND_DEVICE_ID);
            dbus_bus_add_match(conn, match2, &err);
            dbus_connection_flush(conn);
            if (dbus_error_is_set(&err))
            {
                print_errmsg("Connection Error (%s)\n", err.message);
                ret = (-1);
            }
        }
    }

    api_callbacks_reg((void *)&adv_callback_func_tbl);

    print_dbgmsgnoarg("End\n");
    return ret;
}

static void
dbus_send_device_id(void *arg)
{
    int                   err = 0;
    DBusMessage          *msg;
    DBusMessageIter       args;
    dbus_uint32_t         serial = 0;
    DBusMessageIter       iter_struct;
    re_ca_uart_payload_t *u_payload = (re_ca_uart_payload_t *)arg;

    print_dbgmsgnoarg("Enter\n");

    msg = dbus_message_new_signal(
        COMM_TESTER_DBUS_OBJECT,          // object name of the signal
        COMM_TESTER_DBUS_INTERFACE,       // interface name of the signal
        COMM_TESTER_DBUS_SEND_DEVICE_ID); // name of the signal

    if (msg == NULL)
    {
        print_errmsgnoarg("Message Null\n");
    }
    else
    {
        dbus_message_iter_init_append(msg, &args);

        if (!dbus_message_iter_open_container(&args, DBUS_TYPE_STRUCT, NULL, &iter_struct))
        {
            print_errmsgnoarg("Error open iteration container");
            return;
        }

        if (!dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_UINT32, (uint32_t *)&u_payload->cmd))
        {
            print_errmsgnoarg("Out Of Memory!");
            err = 1;
        }
        if (!dbus_message_iter_append_basic(
                &iter_struct,
                DBUS_TYPE_UINT64,
                (uint64_t *)&u_payload->params.device_id.id))
        {
            print_errmsgnoarg("Out Of Memory!");
            err = 1;
        }

        if (!dbus_message_iter_append_basic(
                &iter_struct,
                DBUS_TYPE_UINT64,
                (uint64_t *)&u_payload->params.device_id.addr))
        {
            print_errmsgnoarg("Out Of Memory!");
            err = 1;
        }

        if (!dbus_message_iter_close_container(&args, &iter_struct))
        {
            print_errmsgnoarg("Error close iteration container");
            err = 1;
        }

        if (!err)
        {
            if (!dbus_connection_send(conn, msg, &serial))
            {
                print_errmsgnoarg("Out Of Memory!\n");
            }
            else
            {
                dbus_connection_flush(conn);
                dbus_message_unref(msg);
            }
        }
    }
    print_dbgmsgnoarg("End\n");
}

static void
dbus_send_ack(void *arg)
{
    int                   err = 0;
    DBusMessage          *msg;
    DBusMessageIter       args;
    dbus_uint32_t         serial = 0;
    DBusMessageIter       iter_struct;
    re_ca_uart_payload_t *u_payload = (re_ca_uart_payload_t *)arg;
    uint8_t               bit_value = u_payload->params.ack.ack_state.state & 1U;

    print_dbgmsgnoarg("Enter\n");

    msg = dbus_message_new_signal(
        COMM_TESTER_DBUS_OBJECT,    // object name of the signal
        COMM_TESTER_DBUS_INTERFACE, // interface name of the signal
        COMM_TESTER_DBUS_SEND_ACK); // name of the signal

    if (msg == NULL)
    {
        print_errmsgnoarg("Message Null\n");
    }
    else
    {
        dbus_message_iter_init_append(msg, &args);

        if (!dbus_message_iter_open_container(&args, DBUS_TYPE_STRUCT, NULL, &iter_struct))
        {
            print_errmsgnoarg("Error open iteration container");
            return;
        }

        if (!dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_UINT32, (uint32_t *)&u_payload->cmd))
        {
            print_errmsgnoarg("Out Of Memory!");
            err = 1;
        }
        if (!dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_UINT32, (uint32_t *)&u_payload->params.ack.cmd))
        {
            print_errmsgnoarg("Out Of Memory!");
            err = 1;
        }

        if (!dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_BYTE, (uint8_t *)&bit_value))
        {
            print_errmsgnoarg("Out Of Memory!");
            err = 1;
        }

        if (!dbus_message_iter_close_container(&args, &iter_struct))
        {
            print_errmsgnoarg("Error close iteration container");
            err = 1;
        }

        if (!err)
        {
            if (!dbus_connection_send(conn, msg, &serial))
            {
                print_errmsgnoarg("Out Of Memory!\n");
            }
            else
            {
                dbus_connection_flush(conn);
                dbus_message_unref(msg);
            }
        }
    }
    print_dbgmsgnoarg("End\n");
}

static void
handle_ack(DBusMessage *msg, uint8_t *payload)
{
    int                   err = 0;
    DBusMessageIter       args;
    DBusMessageIter       iter_struct;
    uint8_t               bit_value;
    re_ca_uart_payload_t *u_payload = (re_ca_uart_payload_t *)payload;

    print_dbgmsgnoarg("Enter\n");

    if (!dbus_message_iter_init(msg, &args))
    {
        print_dbgmsgnoarg("Message has no arguments!");
    }
    else
    {
        if (DBUS_TYPE_STRUCT != dbus_message_iter_get_arg_type(&args))
        {
            print_dbgmsgnoarg("Argument is not struct!");
            err = 1;
        }

        if (!err)
        {
            dbus_message_iter_recurse(&args, &iter_struct);
        }

        if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&iter_struct))
        {
            print_dbgmsgnoarg("Argument is not integer!");
            err = 1;
        }

        if (!err)
        {
            dbus_message_iter_get_basic(&iter_struct, &u_payload->cmd);
        }

        if (!dbus_message_iter_next(&iter_struct))
        {
            print_dbgmsgnoarg("Message has too few arguments!");
            err = 1;
        }

        if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&iter_struct))
        {
            print_dbgmsgnoarg("Argument is not integer!");
            err = 1;
        }

        if (!err)
        {
            dbus_message_iter_get_basic(&iter_struct, &u_payload->params.ack.cmd);
        }

        if (!dbus_message_iter_next(&iter_struct))
        {
            print_dbgmsgnoarg("Message has too few arguments!");
            err = 1;
        }

        if (DBUS_TYPE_BYTE != dbus_message_iter_get_arg_type(&iter_struct))
        {
            print_dbgmsgnoarg("Argument is not integer!");
            err = 1;
        }

        if (!err)
        {
            dbus_message_iter_get_basic(&iter_struct, &bit_value);
            u_payload->params.ack.ack_state.state = bit_value & 1U;
        }
    }
    print_dbgmsgnoarg("End\n");
}

static void
handle_device_id(DBusMessage *msg, uint8_t *payload)
{
    int                   err = 0;
    DBusMessageIter       args;
    DBusMessageIter       iter_struct;
    re_ca_uart_payload_t *u_payload = (re_ca_uart_payload_t *)payload;

    print_dbgmsgnoarg("Enter\n");

    if (!dbus_message_iter_init(msg, &args))
    {
        print_dbgmsgnoarg("Message has no arguments!");
    }
    else
    {
        if (DBUS_TYPE_STRUCT != dbus_message_iter_get_arg_type(&args))
        {
            print_dbgmsgnoarg("Argument is not struct!");
            err = 1;
        }

        if (!err)
        {
            dbus_message_iter_recurse(&args, &iter_struct);
        }

        if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&iter_struct))
        {
            print_dbgmsgnoarg("Argument is not integer!");
            err = 1;
        }

        if (!err)
        {
            dbus_message_iter_get_basic(&iter_struct, &u_payload->cmd);
        }

        if (!dbus_message_iter_next(&iter_struct))
        {
            print_dbgmsgnoarg("Message has too few arguments!");
            err = 1;
        }

        if (DBUS_TYPE_UINT64 != dbus_message_iter_get_arg_type(&iter_struct))
        {
            print_dbgmsgnoarg("Argument is not integer!");
            err = 1;
        }

        if (!err)
        {
            dbus_message_iter_get_basic(&iter_struct, &u_payload->params.device_id.id);
        }

        if (!dbus_message_iter_next(&iter_struct))
        {
            print_dbgmsgnoarg("Message has too few arguments!");
            err = 1;
        }

        if (DBUS_TYPE_UINT64 != dbus_message_iter_get_arg_type(&iter_struct))
        {
            print_dbgmsgnoarg("Argument is not integer!");
            err = 1;
        }

        if (!err)
        {
            dbus_message_iter_get_basic(&iter_struct, &u_payload->params.device_id.addr);
        }
    }
    print_dbgmsgnoarg("End\n");
}

void
dbus_check_new_messages(void)
{
    DBusMessage         *msg = NULL;
    re_ca_uart_payload_t uart_payload;
    time_t               startTime, endTime;

    print_dbgmsgnoarg("Enter\n");
    startTime = time(NULL);
    do
    {
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);
        if (msg != NULL)
        {
            if (dbus_message_is_signal(msg, COMM_TESTER_DBUS_INTERFACE, COMM_TESTER_DBUS_SEND_ACK))
            {
                handle_ack(msg, (uint8_t *)&uart_payload);
                formated_output_ack((void *)&uart_payload);
                break;
            }
            else if (dbus_message_is_signal(msg, COMM_TESTER_DBUS_INTERFACE, COMM_TESTER_DBUS_SEND_DEVICE_ID))
            {
                handle_device_id(msg, (uint8_t *)&uart_payload);
                formated_output_device_id((void *)&uart_payload);
                break;
            }

            dbus_message_unref(msg);
        }
        endTime = time(NULL);
    } while (difftime(endTime, startTime) < DBUS_WAIT_TIME);
    print_dbgmsgnoarg("End\n");
}
#endif

/**
 * Parser helper.
 * Parse incomming data from linux terminal
 *
 * License: BSD-3
 * Author: Oleg Protasevich
 */

#ifndef DBUSCONTROLLER_H_
#define DBUSCONTROLLER_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef RUUVI_ESP
#include <linux/types.h>
#endif

int
dbus_connect(bool new);
void
dbus_check_new_messages(void);

#endif /* DBUSCONTROLLER_H_ */

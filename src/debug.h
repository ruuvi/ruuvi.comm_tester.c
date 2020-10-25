/**
 * Debug helper.
 * Defines functions to show log
 *
 * License: BSD-3
 * Author: Oleg Protasevich
 */

#ifndef DEBUG_H
#define DEBUG_H

#ifndef RUUVI_ESP
#include <linux/types.h>
#else
#include "esp_log.h"
#endif
#include <stdio.h>

#ifndef UPDATE_DEBUG_MODE
#define UPDATE_DEBUG_MODE 0
#endif

#if (UPDATE_DEBUG_MODE == 1)
#ifndef RUUVI_ESP
#define print_dbgmsg(str, ...)       fprintf(stderr, "%s: " str, __func__, ##__VA_ARGS__)
#define print_dbgmsgnoarg(str)       fprintf(stderr, "%s in %s: ", str, __func__)
#define print_dbgmsgnofunc(str, ...) fprintf(stderr, "" str, ##__VA_ARGS__)
#define print_dbgmsgnofuncnoarg(str) fprintf(stderr, "%s", str)
#else
#define print_dbgmsg(str, args...)          ESP_LOGI("terminal_dbg", str, __func__, ##args)
#define print_dbgmsgnoarg(str)              ESP_LOGI("terminal_dbg", "%s in %s: ", str, __func__)
#define print_dbgmsgnofunc(str, args...)    ESP_LOGI("terminal_dbg", str, ##args)
#define print_dbgmsgnofuncnoarg(str)        ESP_LOGI("terminal_dbg", str)
#define print_dbgmsghexdump(data, numBytes) ESP_LOG_BUFFER_HEXDUMP("terminal_dbg", data, numBytes, ESP_LOG_INFO);
#endif
#else
#define print_dbgmsg(str, ...)
#define print_dbgmsgnoarg(str)
#define print_dbgmsgnofunc(str, ...)
#define print_dbgmsgnofuncnoarg(str)
#ifdef RUUVI_ESP
#define print_dbgmsghexdump(data, numBytes)
#endif
#endif

#ifndef RUUVI_ESP
#define print_logmsg(str, ...)       fprintf(stderr, "%s: " str, __func__, ##__VA_ARGS__)
#define print_logmsgnoarg(str)       fprintf(stderr, "%s in %s: ", str, __func__)
#define print_logmsgnofunc(str, ...) fprintf(stderr, "" str, ##__VA_ARGS__)
#define print_logmsgnofuncnoarg(str) fprintf(stderr, "%s", str)
#define print_errmsg(str, ...)       fprintf(stderr, "%s: " str, __func__, ##__VA_ARGS__)
#define print_errmsgnoarg(str)       fprintf(stderr, "%s in %s: ", str, __func__)
#define print_errmsgnofunc(str, ...) fprintf(stderr, "" str, ##__VA_ARGS__)
#define print_errmsgnofuncnoarg(str) fprintf(stderr, "%s", str)
#else
#define print_logmsg(str, args...)          ESP_LOGI("terminal_log", str, __func__, ##args)
#define print_logmsgnoarg(str)              ESP_LOGI("termial_log", "%s in %s: ", str, __func__)
#define print_logmsgnofunc(str, args...)    ESP_LOGI("terminal_log", str, ##args)
#define print_logmsgnofuncnoarg(str)        ESP_LOGI("terminal_log", str)
#define print_logmsghexdump(data, numBytes) ESP_LOG_BUFFER_HEXDUMP("terminal_log", data, numBytes, ESP_LOG_INFO);
#define print_errmsg(str, args...)          ESP_LOGE("terminal_err", str, __func__, ##args)
#define print_errmsgnoarg(str)              ESP_LOGE("terminal_err", "%s in %s: ", str, __func__)
#define print_errmsgnofunc(str, args...)    ESP_LOGE("terminal_err", str, ##args)
#define print_errmsgnofuncnoarg(str)        ESP_LOGE("terminal_err", str)
#define print_errmsghexdump(data, numBytes) ESP_LOG_BUFFER_HEXDUMP("terminal_err", data, numBytes, ESP_LOG_ERROR)
#endif

#endif /* DEBUG_H */

/*
 * Copyright 2025 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file
 * @brief Websocket debug logging specific functionality declarations
 */

#ifndef SRC_SHIP_WEBSOCKET_WEBSOCKET_DEBUG_H_
#define SRC_SHIP_WEBSOCKET_WEBSOCKET_DEBUG_H_

#include <libwebsockets.h>
#include "src/common/debug.h"

/** Set WEBSOCKET_DEBUG 1 to enable debug prints
 *  Set WEBSOCKET_DEBUG 2 to enable extra debug prints
 */
#ifndef WEBSOCKET_DEBUG
#define WEBSOCKET_DEBUG 0
#endif

/**
 * @defgroup WebsocketDebug Websocket debug printf() and hexdump,
 * enabled whith WEBSOCKET_DEBUG = 1
 * (Set WEBSOCKET_DEBUG 1 in websocket_debug.h to enable debug prints)
 * @{
 */
#if WEBSOCKET_DEBUG
#define WEBSOCKET_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#define WEBSOCKET_DEBUG_HEXDUMP(in, len) DEBUG_HEXDUMP(WEBSOCKET_DEBUG, in, len)
#else
#define WEBSOCKET_DEBUG_PRINTF(fmt, ...)
#define WEBSOCKET_DEBUG_HEXDUMP(in, len)
#endif  // WEBSOCKET_DEBUG

/**@}*/

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Convert the LWS callback reason to string
 * @param reason Reason identifier to be converted
 * @return String representation of reason on success
 * or empty string (not NULL) on failure
 */
const char* WebsocketLwsReasonToString(enum lws_callback_reasons reason);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_WEBSOCKET_WEBSOCKET_DEBUG_H_

/*
 * Copyright 2025 bgewehr (bg-hems branch)
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
 * @file websocket_server_esp32.h
 * @brief ESP32 WebSocket Server wrapper for openeebus SHIP layer.
 *
 * The creator wraps an already-accepted httpd WebSocket connection (identified
 * by server handle + socket fd).  It does NOT start a new httpd instance —
 * the httpd is owned and started by http_server_esp32.c.
 *
 * Usage (internal, called from http_server_esp32.c):
 *   1. Accept TLS connection in http_server_esp32.c.
 *   2. Call WebsocketServerCreatorEsp32Create(server_handle, sockfd).
 *   3. Pass the creator to conn_establish_cb (ShipNode).
 *   4. Retrieve the created WebsocketObject* with
 *      WebsocketServerCreatorEsp32GetCreatedWs() and store it as the
 *      per-session context so subsequent data frames can be delivered.
 */

#ifndef PORT_ESP32_WEBSOCKET_WEBSOCKET_SERVER_ESP32_H_
#define PORT_ESP32_WEBSOCKET_WEBSOCKET_SERVER_ESP32_H_

#include <esp_http_server.h>
#include <stddef.h>
#include <stdint.h>

#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/websocket_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a server-side WebSocket creator for an already-accepted
 *        httpd connection.
 *
 * @param server  httpd instance that accepted the incoming connection
 * @param sockfd  socket fd of the accepted WebSocket connection
 * @return        WebsocketCreatorObject* or NULL on OOM
 */
WebsocketCreatorObject* WebsocketServerCreatorEsp32Create(
    httpd_handle_t server,
    int            sockfd);

/**
 * @brief Retrieve the WebsocketObject created by the last
 *        WEBSOCKET_CREATOR_CREATE_WEBSOCKET() call.
 *
 * Returns NULL if create_websocket has not been called yet or failed.
 * Used by http_server_esp32.c to obtain the ws pointer for session-context
 * storage so subsequent data frames can be delivered to the SHIP layer.
 */
WebsocketObject* WebsocketServerCreatorEsp32GetCreatedWs(
    WebsocketCreatorObject* creator);

/**
 * @brief Deliver a received binary frame to the SHIP state machine.
 *
 * Called from the httpd URI handler for data frames.  Invokes the
 * WebsocketCallback registered by the SHIP layer.
 */
void WebsocketServerEsp32DeliverData(WebsocketObject* ws,
                                     const uint8_t* data, size_t len);

/**
 * @brief Notify the SHIP state machine that the WebSocket has closed.
 *
 * Called from the httpd URI handler when a close frame is received or
 * the session context free-callback fires (unexpected disconnect).
 */
void WebsocketServerEsp32NotifyClose(WebsocketObject* ws);

#ifdef __cplusplus
}
#endif

#endif  /* PORT_ESP32_WEBSOCKET_WEBSOCKET_SERVER_ESP32_H_ */

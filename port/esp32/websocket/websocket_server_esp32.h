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
 * @brief ESP32 WebSocket Server for openeebus SHIP layer.
 *
 * Creates a WebsocketCreatorObject that spawns an httpd-based TLS WebSocket
 * server on the SHIP path (/ship/). The Westnetz CLS-Steuerbox (acting as
 * EG / Energy Guard) connects to this server to send LPC limits.
 */

#ifndef PORT_ESP32_WEBSOCKET_WEBSOCKET_SERVER_ESP32_H_
#define PORT_ESP32_WEBSOCKET_WEBSOCKET_SERVER_ESP32_H_

#include <stddef.h>
#include <stdint.h>

#include "src/ship/api/websocket_creator_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create an ESP32 WebSocket server creator.
 *
 * The creator starts an httpd TLS server with mutual authentication on the
 * given port. Call WEBSOCKET_CREATOR_CREATE_WEBSOCKET() to obtain the
 * WebsocketObject and hand it to the openeebus SHIP layer.
 *
 * @param port          TCP port (SHIP default: 4712)
 * @param cert_der      Server certificate in DER format
 * @param cert_der_len  Length of cert_der in bytes
 * @param key_der       Private key in DER format
 * @param key_der_len   Length of key_der in bytes
 * @return              WebsocketCreatorObject* or NULL on failure
 */
WebsocketCreatorObject* WebsocketServerCreatorEsp32Create(
    uint16_t    port,
    const void* cert_der,
    size_t      cert_der_len,
    const void* key_der,
    size_t      key_der_len);

#ifdef __cplusplus
}
#endif

#endif  /* PORT_ESP32_WEBSOCKET_WEBSOCKET_SERVER_ESP32_H_ */

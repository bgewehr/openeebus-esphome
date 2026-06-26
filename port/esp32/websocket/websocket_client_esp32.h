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
 * @file websocket_client_esp32.h
 * @brief ESP32 WebSocket Client for openeebus SHIP layer.
 *
 * Creates a WebsocketCreatorObject backed by esp_websocket_client.
 * Used when the HEMS initiates an outbound SHIP connection (e.g. to a
 * Vaillant heat pump or during pairing with the CLS-Steuerbox).
 */

#ifndef PORT_ESP32_WEBSOCKET_WEBSOCKET_CLIENT_ESP32_H_
#define PORT_ESP32_WEBSOCKET_WEBSOCKET_CLIENT_ESP32_H_

#include <stddef.h>
#include <stdint.h>

#include "src/ship/api/websocket_creator_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create an ESP32 WebSocket client creator.
 *
 * @param uri           Target URI, e.g. "wss://192.168.1.50:4712/ship/"
 * @param cert_der      Client certificate in DER format (our identity)
 * @param cert_der_len  Length of cert_der
 * @param key_der       Private key in DER format
 * @param key_der_len   Length of key_der
 * @param ca_der        Remote server/CA certificate for verification (DER),
 *                      may be NULL to skip server cert verification
 * @param ca_der_len    Length of ca_der (0 if ca_der is NULL)
 * @return              WebsocketCreatorObject* or NULL on failure
 */
WebsocketCreatorObject* WebsocketClientCreatorEsp32Create(
    const char* uri,
    const void* cert_der,
    size_t      cert_der_len,
    const void* key_der,
    size_t      key_der_len,
    const void* ca_der,
    size_t      ca_der_len);

#ifdef __cplusplus
}
#endif

#endif  /* PORT_ESP32_WEBSOCKET_WEBSOCKET_CLIENT_ESP32_H_ */

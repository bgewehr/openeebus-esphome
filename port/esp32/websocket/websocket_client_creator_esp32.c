/*
 * Copyright 2025 bgewehr (bg-hems branch)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */
/**
 * @file websocket_client_creator_esp32.c
 * @brief Bridges the openeebus websocket_creator_interface to the ESP32
 *        esp_websocket_client-based implementation.
 *
 * Used when the HEMS initiates an outbound SHIP connection.
 */

#include <stdio.h>

#include "src/ship/websocket/websocket_client_creator.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "websocket_client_esp32.h"

/**
 * @brief Create a WebSocket client creator for the ESP32 platform.
 *
 * Replaces the upstream libwebsockets-based implementation with one
 * based on esp_websocket_client.
 *
 * @param uri           WebSocket URI (e.g. "wss://host:port/ship/")
 * @param tls_cert      Our TLS client certificate
 * @param remote_ski    Expected remote SKI for verification
 * @return              WebsocketCreatorObject*
 */
WebsocketCreatorObject* WebsocketClientCreatorCreate(
    const char*                  uri,
    const TlsCertificateObject*  tls_cert,
    const char*                  remote_ski)
{
  /* Pass NULL as CA cert — K40rf (and all EEBus devices) use self-signed certs
   * that cannot be chain-verified.  Identity is established at the SHIP layer
   * via SKI comparison, not via TLS PKI.  With no CA, mbedtls uses
   * MBEDTLS_SSL_VERIFY_NONE and the TLS handshake completes. */
  return WebsocketClientCreatorEsp32Create(
      uri,
      TLS_CERTIFICATE_GET_CERTIFICATE(tls_cert),
      TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(tls_cert),
      TLS_CERTIFICATE_GET_PRIVATE_KEY(tls_cert),
      TLS_CERTIFICATE_GET_PRIVATE_KEY_SIZE(tls_cert),
      NULL, 0);
}

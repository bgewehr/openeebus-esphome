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
 * @file websocket_server_creator_esp32.c
 * @brief Bridges the openeebus websocket_creator_interface to the ESP32
 *        httpd-based WebSocket server.
 *
 * openeebus calls WebsocketCreatorCreate() to obtain a server creator.
 * This file provides that factory using the ESP32 implementation.
 */

#include "src/ship/websocket/websocket_creator.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "websocket_server_esp32.h"

/**
 * @brief Create a WebSocket server creator for the ESP32 platform.
 *
 * Called by openeebus SHIP layer when initialising as a server (CS role).
 *
 * @param port          TCP port to listen on (SHIP default 4712)
 * @param tls_cert      openeebus TLS certificate object
 * @return              WebsocketCreatorObject* for use by ShipNode
 */
WebsocketCreatorObject* WebsocketServerCreatorCreate(
    uint16_t                     port,
    const TlsCertificateObject*  tls_cert)
{
  return WebsocketServerCreatorEsp32Create(
      port,
      TLS_CERTIFICATE_GET_CERTIFICATE(tls_cert),
      TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(tls_cert),
      TLS_CERTIFICATE_GET_PRIVATE_KEY(tls_cert),
      TLS_CERTIFICATE_GET_PRIVATE_KEY_SIZE(tls_cert));
}

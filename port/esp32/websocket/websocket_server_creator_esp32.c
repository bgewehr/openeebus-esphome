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
 * @brief Stub for the openeebus WebsocketServerCreatorCreate factory.
 *
 * In the ESP32 port, server-side WebSocket creators are created directly
 * inside http_server_esp32.c (ShipWsHandler) when an incoming connection
 * is accepted — WebsocketServerCreatorCreate() is never called from the
 * ESP32 code path.  This stub satisfies the linker symbol requirement.
 */

#include "src/ship/websocket/websocket_creator.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "websocket_server_esp32.h"

WebsocketCreatorObject* WebsocketServerCreatorCreate(
    uint16_t                     port,
    const TlsCertificateObject*  tls_cert)
{
  (void)port;
  (void)tls_cert;
  /* Not used in ESP32 path — server creators are built in ShipWsHandler. */
  return NULL;
}

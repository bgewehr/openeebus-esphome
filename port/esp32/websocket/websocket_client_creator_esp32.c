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

#include "src/ship/websocket/websocket_creator.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/remote_service.h"
#include "websocket_client_esp32.h"

/**
 * @brief Create a WebSocket client creator for the ESP32 platform.
 *
 * @param remote        Remote SHIP service description (host, port, path)
 * @param tls_cert      Our TLS client certificate
 * @return              WebsocketCreatorObject*
 */
WebsocketCreatorObject* WebsocketClientCreatorCreate(
    const RemoteService*         remote,
    const TlsCertificateObject*  tls_cert)
{
  /* Build wss://host:port/ship/ URI */
  char uri[256];
  snprintf(uri, sizeof(uri), "wss://%s:%d%s",
           remote->host,
           remote->port,
           remote->path ? remote->path : "/ship/");

  return WebsocketClientCreatorEsp32Create(
      uri,
      TLS_CERTIFICATE_GET_CERTIFICATE(tls_cert),
      TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(tls_cert),
      TLS_CERTIFICATE_GET_PRIVATE_KEY(tls_cert),
      TLS_CERTIFICATE_GET_PRIVATE_KEY_SIZE(tls_cert),
      /* CA cert: use our own cert as root (self-signed / TOFU model) */
      TLS_CERTIFICATE_GET_CERTIFICATE(tls_cert),
      TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(tls_cert));
}

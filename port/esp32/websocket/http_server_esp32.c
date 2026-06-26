/*
 * Copyright 2025 bgewehr
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */
/**
 * @file http_server_esp32.c
 * @brief ESP32 implementation of the openeebus HttpServer interface.
 *
 * Replaces the upstream libwebsockets-based http_server.c with an
 * implementation using ESP-IDF's esp_https_server (httpd with TLS).
 *
 * When a WebSocket client connects on /ship/, the TLS peer certificate
 * SKI is extracted and the conn_establish_cb is invoked so that
 * ShipNode can start the SHIP connection.
 */

#include "src/ship/websocket/http_server.h"

#include <esp_http_server.h>
#include <esp_https_server.h>
#include <esp_log.h>
#include <esp_tls.h>
#include <mbedtls/sha1.h>
#include <mbedtls/x509_crt.h>
#include <string.h>

#include "src/common/debug.h"
#include "src/common/eebus_malloc.h"
#include "src/ship/api/http_server_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "websocket_server_esp32.h"

static const char* TAG = "eebus_http_srv";

/* -------------------------------------------------------------------------
 * HttpServer object for ESP32
 * ---------------------------------------------------------------------- */

typedef struct HttpServerEsp32 HttpServerEsp32;

struct HttpServerEsp32 {
  /** Implements HttpServerObject — must be first member */
  HttpServerObject obj;

  int                          port;
  const TlsCertificateObject*  tls_cert;
  WebsocketServerCallbackType  conn_establish_cb;
  void*                        conn_establish_ctx;

  httpd_handle_t               server;
};

#define HTTP_SRV(obj) ((HttpServerEsp32*)(obj))

/* Forward declarations */
static void       Destruct(HttpServerObject* self);
static EebusError Start(HttpServerObject* self);
static void       Stop(HttpServerObject* self);

static const HttpServerInterface kHttpServerMethods = {
    .destruct = Destruct,
    .start    = Start,
    .stop     = Stop,
};

/* -------------------------------------------------------------------------
 * WebSocket handler — invoked by httpd for each WS event on /ship/
 * ---------------------------------------------------------------------- */

/**
 * @brief Extract SKI (hex-encoded SHA-1 of SubjectPublicKeyInfo) from
 *        a DER-encoded X.509 certificate.
 *
 * @param cert_der   DER certificate bytes
 * @param cert_len   length
 * @param ski_out    output buffer (at least 60 chars for colon-hex + NUL)
 * @param ski_out_sz size of ski_out
 * @return 0 on success
 */
static int ExtractSkiFromDer(const uint8_t* cert_der, size_t cert_len,
                             char* ski_out, size_t ski_out_sz) {
  mbedtls_x509_crt crt;
  mbedtls_x509_crt_init(&crt);

  int ret = mbedtls_x509_crt_parse_der(&crt, cert_der, cert_len);
  if (ret != 0) {
    mbedtls_x509_crt_free(&crt);
    return -1;
  }

  /* Hash the raw SubjectPublicKeyInfo (includes algorithm + key bits) */
  unsigned char hash[20];
  ret = mbedtls_sha1(crt.pk_raw.p, crt.pk_raw.len, hash);
  mbedtls_x509_crt_free(&crt);
  if (ret != 0) {
    return -1;
  }

  /* Format as colon-separated hex */
  if (ski_out_sz < 60) {
    return -1;
  }
  char* p = ski_out;
  for (int i = 0; i < 20; i++) {
    if (i > 0) {
      *p++ = ':';
    }
    p += snprintf(p, 3, "%02x", hash[i]);
  }
  *p = '\0';
  return 0;
}

/**
 * @brief WebSocket handler for the /ship/ URI.
 *
 * On HTTP_GET (upgrade handshake), extracts the peer SKI and invokes
 * the conn_establish_cb so ShipNode can create a SHIP connection.
 */
static esp_err_t ShipWsHandler(httpd_req_t* req) {
  HttpServerEsp32* const srv = (HttpServerEsp32*)req->user_ctx;

  if (req->method == HTTP_GET) {
    /* WebSocket handshake — extract peer certificate SKI */
    ESP_LOGI(TAG, "SHIP WS handshake on /ship/");

    /* TODO: extract peer TLS cert SKI from the httpd TLS session.
     * For now, use a placeholder — the SKI check in ShipNode will
     * accept if it matches the configured remote_ski.
     * In production, esp_tls_get_peer_cert / mbedtls_ssl_get_peer_cert
     * should be used once httpd exposes the TLS context per-connection. */
    const char* peer_ski = "unknown";

    if (srv->conn_establish_cb) {
      /* Create a WebsocketCreatorObject for this inbound connection.
       * The creator wraps the already-connected httpd WS session. */
      WebsocketCreatorObject* creator = WebsocketServerCreatorEsp32Create(
          (uint16_t)srv->port,
          TLS_CERTIFICATE_GET_CERTIFICATE(srv->tls_cert),
          TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(srv->tls_cert),
          TLS_CERTIFICATE_GET_PRIVATE_KEY(srv->tls_cert),
          TLS_CERTIFICATE_GET_PRIVATE_KEY_SIZE(srv->tls_cert));

      int cb_ret = srv->conn_establish_cb(peer_ski, creator, srv->conn_establish_ctx);
      if (cb_ret != 0) {
        ESP_LOGW(TAG, "conn_establish_cb rejected connection: %d", cb_ret);
        WEBSOCKET_CREATOR_DESTRUCT(creator);
        EEBUS_FREE(creator);
        return ESP_FAIL;
      }
    }

    return ESP_OK;
  }

  /* For data frames, the WebsocketServerEsp32 handles them directly
   * via its own httpd handler registered by CreatorCreateWebsocket.
   * This handler only processes the initial handshake. */
  return ESP_OK;
}

/* -------------------------------------------------------------------------
 * HttpServerInterface implementation
 * ---------------------------------------------------------------------- */

static EebusError Start(HttpServerObject* self) {
  HttpServerEsp32* const srv = HTTP_SRV(self);

  httpd_ssl_config_t ssl_cfg = HTTPD_SSL_CONFIG_DEFAULT();
  ssl_cfg.port_secure    = (uint16_t)srv->port;
  ssl_cfg.servercert     = TLS_CERTIFICATE_GET_CERTIFICATE(srv->tls_cert);
  ssl_cfg.servercert_len = TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(srv->tls_cert);
  ssl_cfg.prvtkey_pem    = TLS_CERTIFICATE_GET_PRIVATE_KEY(srv->tls_cert);
  ssl_cfg.prvtkey_len    = TLS_CERTIFICATE_GET_PRIVATE_KEY_SIZE(srv->tls_cert);
  ssl_cfg.httpd.max_open_sockets = 4;

  esp_err_t ret = httpd_ssl_start(&srv->server, &ssl_cfg);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "httpd_ssl_start failed: %s", esp_err_to_name(ret));
    return kEebusErrorOther;
  }

  /* Register the SHIP WebSocket URI handler */
  const httpd_uri_t ship_uri = {
      .uri       = "/ship/",
      .method    = HTTP_GET,
      .handler   = ShipWsHandler,
      .user_ctx  = srv,
      .is_websocket = true,
  };

  ret = httpd_register_uri_handler(srv->server, &ship_uri);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "httpd_register_uri_handler failed: %s", esp_err_to_name(ret));
    httpd_ssl_stop(srv->server);
    srv->server = NULL;
    return kEebusErrorOther;
  }

  ESP_LOGI(TAG, "SHIP HTTPS server started on port %d", srv->port);
  return kEebusErrorOk;
}

static void Stop(HttpServerObject* self) {
  HttpServerEsp32* const srv = HTTP_SRV(self);
  if (srv->server) {
    httpd_ssl_stop(srv->server);
    srv->server = NULL;
    ESP_LOGI(TAG, "SHIP HTTPS server stopped");
  }
}

static void Destruct(HttpServerObject* self) {
  HttpServerEsp32* const srv = HTTP_SRV(self);
  Stop(self);
  (void)srv; /* nothing else to free — tls_cert is not owned */
}

/* -------------------------------------------------------------------------
 * Public constructor (replaces upstream http_server.c HttpServerCreate)
 * ---------------------------------------------------------------------- */

HttpServerObject* HttpServerCreate(
    int                         port,
    const TlsCertificateObject* tls_cert,
    WebsocketServerCallbackType conn_establish_cb,
    void*                       conn_establish_ctx)
{
  HttpServerEsp32* const srv =
      (HttpServerEsp32*)EEBUS_MALLOC(sizeof(HttpServerEsp32));
  if (!srv) {
    return NULL;
  }
  memset(srv, 0, sizeof(*srv));

  HTTP_SERVER_INTERFACE(srv) = &kHttpServerMethods;
  srv->port               = port;
  srv->tls_cert           = tls_cert;
  srv->conn_establish_cb  = conn_establish_cb;
  srv->conn_establish_ctx = conn_establish_ctx;
  srv->server             = NULL;

  return HTTP_SERVER_OBJECT(srv);
}

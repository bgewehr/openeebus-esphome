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
 * Architecture
 * ============
 * ONE httpd_ssl instance per SHIP port handles all WebSocket lifecycle:
 *
 *   HTTP_GET (WS upgrade handshake)
 *     → extract peer certificate SKI (or use "unknown" if none)
 *     → call conn_establish_cb → ShipNode calls creator->create_websocket
 *     → store resulting WebsocketObject* in session context (req->sess_ctx)
 *
 *   Subsequent binary frames
 *     → retrieve WebsocketObject* from req->sess_ctx
 *     → call WebsocketServerEsp32DeliverData() → SHIP state machine
 *
 *   Close frame / session teardown
 *     → call WebsocketServerEsp32NotifyClose() → SHIP state machine
 *
 * websocket_server_esp32.c wraps the existing connection (server handle +
 * sockfd) and must NOT start its own httpd instance.
 */

#include "src/ship/websocket/http_server.h"

#include <esp_http_server.h>
#include <esp_https_server.h>
#include <esp_log.h>
#include <esp_tls.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509_crt.h>
#include <string.h>

#include "src/common/debug.h"
#include "src/common/eebus_malloc.h"
#include "src/ship/api/http_server_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/websocket_interface.h"
#include "src/ship/tls_certificate/tls_certificate.h"
#include "websocket_server_esp32.h"

static const char* TAG = "eebus_http_srv";

/* -------------------------------------------------------------------------
 * HttpServer object for ESP32
 * ---------------------------------------------------------------------- */

typedef struct HttpServerEsp32 HttpServerEsp32;

struct HttpServerEsp32 {
  HttpServerObject            obj;   /* must be first */

  int                         port;
  const TlsCertificateObject* tls_cert;
  WebsocketServerCallbackType conn_establish_cb;
  void*                       conn_establish_ctx;

  httpd_handle_t              server;
};

#define HTTP_SRV(obj) ((HttpServerEsp32*)(obj))

static void       Destruct(HttpServerObject* self);
static EebusError Start(HttpServerObject* self);
static void       Stop(HttpServerObject* self);

static const HttpServerInterface kHttpServerMethods = {
    .destruct = Destruct,
    .start    = Start,
    .stop     = Stop,
};

/* -------------------------------------------------------------------------
 * SKI extraction from peer TLS certificate
 * ---------------------------------------------------------------------- */

/* Private copy of esp_https_server's internal transport context layout.
 * Must match httpd_ssl_transport_ctx_t in esp-idf/components/esp_https_server/src/https_server.c.
 * httpd_sess_get_transport_ctx() returns this struct, NOT esp_tls_t* directly. */
typedef struct {
  esp_tls_t *tls;
  void      *global_ctx; /* httpd_ssl_ctx_t* — not used here */
} ship_ssl_transport_ctx_t;

/* Extract SHIP SKI from the TLS peer certificate for this connection.
 * Returns ski_out on success (40-char lowercase hex), NULL if cert unavailable.
 *
 * MBEDTLS_SSL_KEEP_PEER_CERTIFICATE is hardcoded in ESP-IDF's mbedtls_config.h,
 * so mbedtls_ssl_get_peer_cert() is always available after the handshake. */
static const char* ExtractSkiFromPeerCert(httpd_handle_t server, int sockfd,
                                          char* ski_out, size_t ski_out_sz) {
  if (ski_out_sz < 41) return NULL;

  ship_ssl_transport_ctx_t* tctx =
      (ship_ssl_transport_ctx_t*)httpd_sess_get_transport_ctx(server, sockfd);
  if (!tctx) {
    ESP_LOGE(TAG, "SKI: transport ctx NULL for fd=%d", sockfd);
    return NULL;
  }
  if (!tctx->tls) {
    ESP_LOGE(TAG, "SKI: esp_tls_t NULL in transport ctx for fd=%d", sockfd);
    return NULL;
  }

  mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)esp_tls_get_ssl_context(tctx->tls);
  if (!ssl) {
    ESP_LOGE(TAG, "SKI: esp_tls_get_ssl_context returned NULL for fd=%d", sockfd);
    return NULL;
  }

  const mbedtls_x509_crt* peer_cert = mbedtls_ssl_get_peer_cert(ssl);
  if (!peer_cert || peer_cert->raw.len == 0) {
    /* Peer did not send a certificate. SHIP requires mutual TLS — check that
     * CONFIG_ESP_TLS_SERVER_MIN_AUTH_MODE_OPTIONAL=y is set so that the server
     * sends a Certificate Request and the peer responds with its cert. */
    ESP_LOGE(TAG, "SKI: peer cert NULL/empty for fd=%d — K40rf may not have sent its cert", sockfd);
    return NULL;
  }

  const char* ski = TlsCertificateCalcPublicKeySki(peer_cert->raw.p, peer_cert->raw.len);
  if (!ski) {
    ESP_LOGE(TAG, "SKI: TlsCertificateCalcPublicKeySki failed for fd=%d (cert len=%u)",
             sockfd, (unsigned)peer_cert->raw.len);
    return NULL;
  }

  strncpy(ski_out, ski, ski_out_sz - 1);
  ski_out[ski_out_sz - 1] = '\0';
  EEBUS_FREE((void*)ski);
  return ski_out;
}

/* -------------------------------------------------------------------------
 * Session context free callback — notifies SHIP layer on disconnect
 * ---------------------------------------------------------------------- */

static void WsSessionFreeCtx(void* ctx) {
  WebsocketObject* ws = (WebsocketObject*)ctx;
  if (ws) {
    WebsocketServerEsp32NotifyClose(ws);
  }
}

/* -------------------------------------------------------------------------
 * WebSocket URI handler — handles upgrade handshake + all data frames
 * ---------------------------------------------------------------------- */

static esp_err_t ShipWsHandler(httpd_req_t* req) {
  HttpServerEsp32* const srv = (HttpServerEsp32*)req->user_ctx;

  /* ---- WebSocket upgrade handshake ---- */
  if (req->method == HTTP_GET) {
    ESP_LOGW(TAG, "SHIP WS handshake on /ship/ (port %d)", srv->port);

    int sockfd = httpd_req_to_sockfd(req);

    char peer_ski_buf[41] = {0};
    const char* peer_ski = "unknown";
    {
      const char* s = ExtractSkiFromPeerCert(req->handle, sockfd,
                                             peer_ski_buf, sizeof(peer_ski_buf));
      if (s) {
        peer_ski = peer_ski_buf;
        ESP_LOGW(TAG, "Peer SKI: %s (fd=%d)", peer_ski, sockfd);
      }
      /* Detailed error already logged inside ExtractSkiFromPeerCert */
    }

    if (!srv->conn_establish_cb) {
      return ESP_FAIL;
    }

    /* Create a server-side WebSocket creator wrapping this connection */
    WebsocketCreatorObject* creator =
        WebsocketServerCreatorEsp32Create(srv->server, sockfd);
    if (!creator) {
      ESP_LOGE(TAG, "WebsocketServerCreatorEsp32Create failed");
      return ESP_FAIL;
    }

    /* Let ShipNode accept/reject the connection and create the WebSocket */
    int cb_ret = srv->conn_establish_cb(peer_ski, creator, srv->conn_establish_ctx);

    if (cb_ret == 0) {
      /* Store the created WebsocketObject* as session context so subsequent
       * data frames can be delivered to the SHIP state machine.
       * WsSessionFreeCtx is called by httpd on unexpected disconnect. */
      WebsocketObject* ws = WebsocketServerCreatorEsp32GetCreatedWs(creator);
      if (ws) {
        req->sess_ctx    = ws;
        req->free_ctx    = WsSessionFreeCtx;
        req->ignore_sess_ctx_changes = false;
      } else {
        ESP_LOGW(TAG, "conn_establish_cb succeeded but no WebSocket created");
        cb_ret = -1;
      }
    }

    WEBSOCKET_CREATOR_DESTRUCT(creator);

    if (cb_ret != 0) {
      ESP_LOGW(TAG, "SHIP conn rejected (ski=%s) err=%d", peer_ski, cb_ret);
      return ESP_FAIL;
    }

    ESP_LOGW(TAG, "SHIP WS session established (ski=%s fd=%d)", peer_ski, sockfd);
    return ESP_OK;
  }

  /* ---- Data frames ---- */
  WebsocketObject* ws = (WebsocketObject*)req->sess_ctx;
  if (!ws) {
    /* Session context not set — connection was rejected at handshake */
    return ESP_FAIL;
  }

  httpd_ws_frame_t frame = {
    .type    = HTTPD_WS_TYPE_BINARY,
    .payload = NULL,
    .len     = 0,
  };

  /* Probe frame length */
  esp_err_t ret = httpd_ws_recv_frame(req, &frame, 0);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "httpd_ws_recv_frame probe failed: %d", ret);
    return ret;
  }

  /* Close frame */
  if (frame.type == HTTPD_WS_TYPE_CLOSE) {
    ESP_LOGD(TAG, "SHIP WS close frame received");
    req->sess_ctx = NULL;   /* prevent double-notify via free_ctx */
    WebsocketServerEsp32NotifyClose(ws);
    return ESP_OK;
  }

  if (frame.len == 0) {
    return ESP_OK;
  }

  /* Allocate and receive payload */
  uint8_t* buf = (uint8_t*)EEBUS_MALLOC(frame.len);
  if (!buf) {
    ESP_LOGE(TAG, "rx alloc failed (%u bytes)", (unsigned)frame.len);
    return ESP_ERR_NO_MEM;
  }
  frame.payload = buf;

  ret = httpd_ws_recv_frame(req, &frame, frame.len);
  if (ret == ESP_OK) {
    WebsocketServerEsp32DeliverData(ws, buf, frame.len);
  } else {
    ESP_LOGE(TAG, "httpd_ws_recv_frame recv failed: %d", ret);
  }

  EEBUS_FREE(buf);
  return ret;
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
  ssl_cfg.httpd.ctrl_port        = 32768 + (uint16_t)srv->port;

  /* ESP-IDF only sets MBEDTLS_SSL_VERIFY_OPTIONAL (which sends a Certificate
   * Request to the peer) when cacert_buf != NULL. Set our own server cert as
   * the "CA" just to satisfy that check. With VERIFY_OPTIONAL the connection
   * proceeds even when the peer's self-signed cert doesn't chain to us —
   * SHIP uses SKI-based trust, not CA chain validation. Both Python (OpenSSL)
   * and K40rf (mbedTLS) send their configured cert regardless of the CA list
   * in the Certificate Request, so peer_cert will be non-NULL after the
   * handshake and SKI extraction will succeed. */
  ssl_cfg.cacert_pem = TLS_CERTIFICATE_GET_CERTIFICATE(srv->tls_cert);
  ssl_cfg.cacert_len = TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(srv->tls_cert);
#ifdef CONFIG_ESP_TLS_SERVER_MIN_AUTH_MODE_OPTIONAL
  ssl_cfg.client_cert_authmode_optional = true;
#endif

  esp_err_t ret = httpd_ssl_start(&srv->server, &ssl_cfg);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "httpd_ssl_start failed on port %d: %s",
             srv->port, esp_err_to_name(ret));
    return kEebusErrorOther;
  }

  /* Register the SHIP WebSocket URI handler.
   * supported_subprotocol = "ship" ensures the 101 response includes
   * Sec-WebSocket-Protocol: ship, required by SHIP-compliant clients. */
  const httpd_uri_t ship_uri = {
      .uri                      = "/ship/",
      .method                   = HTTP_GET,
      .handler                  = ShipWsHandler,
      .user_ctx                 = srv,
      .is_websocket             = true,
      .handle_ws_control_frames = true,  /* deliver close frames to handler */
      .supported_subprotocol    = "ship",
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
    ESP_LOGI(TAG, "SHIP HTTPS server stopped (port %d)", srv->port);
  }
}

static void Destruct(HttpServerObject* self) {
  Stop(self);
}

/* -------------------------------------------------------------------------
 * Public constructor
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

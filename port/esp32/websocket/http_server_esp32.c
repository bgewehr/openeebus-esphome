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
#include <mbedtls/pk.h>
#include <mbedtls/sha1.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509_crt.h>
#include <string.h>

#include "src/common/debug.h"
#include "src/common/eebus_malloc.h"
#include "src/ship/api/http_server_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/websocket_interface.h"
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

/* Compute SHIP SKI: SHA-1 of the raw public key bytes inside the SubjectPublicKeyInfo
 * BIT STRING (RFC 5280 method 1). Matches what openeebus CalcSubjectKeyIdString does.
 * Output: 40-char lowercase hex string without colons (e.g. "52ab299a..."). */
static int ExtractSkiFromDer(const uint8_t* cert_der, size_t cert_len,
                             char* ski_out, size_t ski_out_sz) {
  if (ski_out_sz < 41) return -1;

  mbedtls_x509_crt crt;
  mbedtls_x509_crt_init(&crt);

  int ret = mbedtls_x509_crt_parse_der(&crt, cert_der, cert_len);
  if (ret != 0) {
    mbedtls_x509_crt_free(&crt);
    return -1;
  }

  /* Write SubjectPublicKeyInfo DER into a temp buffer (written at end of buf).
   * Reference openeebus uses 2048 bytes; match that size to handle any key type. */
  uint8_t buf[2048];
  int len = mbedtls_pk_write_pubkey_der(&crt.pk, buf, sizeof(buf));
  mbedtls_x509_crt_free(&crt);
  if (len < 0) return -1;

  /* mbedtls_pk_write_pubkey_der writes at end of buf */
  const uint8_t* p = buf + sizeof(buf) - len;

  /* Skip outer SEQUENCE tag + length */
  if (len < 2 || p[0] != 0x30) return -1;
  int hdr = 2 + ((p[1] & 0x80) ? (p[1] & 0x7f) : 0);
  p += hdr; len -= hdr;

  /* Skip AlgorithmIdentifier SEQUENCE */
  if (len < 2 || p[0] != 0x30) return -1;
  int alg_len = p[1] + 2;
  p += alg_len; len -= alg_len;

  /* Skip BIT STRING tag + length */
  if (len < 2 || p[0] != 0x03) return -1;
  hdr = 2 + ((p[1] & 0x80) ? (p[1] & 0x7f) : 0);
  p += hdr; len -= hdr;

  /* Skip unused-bits byte */
  if (len < 1) return -1;
  p += 1; len -= 1;

  /* SHA-1 of the raw public key */
  unsigned char hash[20];
  ret = mbedtls_sha1(p, (size_t)len, hash);
  if (ret != 0) return -1;

  /* Format as 40-char lowercase hex without colons */
  char* out = ski_out;
  for (int i = 0; i < 20; i++) {
    out += snprintf(out, 3, "%02x", hash[i]);
  }
  *out = '\0';
  return 0;
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

    /* Extract peer certificate SKI from TLS session */
    char peer_ski_buf[60] = {0};
    const char* peer_ski = "unknown";

    esp_tls_t* tls = (esp_tls_t*)httpd_sess_get_transport_ctx(req->handle, sockfd);
    if (tls) {
      mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)esp_tls_get_ssl_context(tls);
      if (ssl) {
        const mbedtls_x509_crt* peer_cert = mbedtls_ssl_get_peer_cert(ssl);
        if (peer_cert && peer_cert->raw.len > 0) {
          if (ExtractSkiFromDer(peer_cert->raw.p, peer_cert->raw.len,
                                peer_ski_buf, sizeof(peer_ski_buf)) == 0) {
            peer_ski = peer_ski_buf;
            ESP_LOGW(TAG, "Peer SKI: %s", peer_ski);
          } else {
            ESP_LOGW(TAG, "Failed to extract SKI from peer cert");
          }
        } else {
          ESP_LOGW(TAG, "No peer certificate in TLS session (SKI will be 'unknown')");
        }
      }
    } else {
      ESP_LOGW(TAG, "No TLS transport context");
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

  /* Request client certificate with empty CA list (SHIP spec: SKI-based trust,
   * not CA chain validation). With no cacert and VERIFY_OPTIONAL auth mode,
   * mbedTLS sends Certificate Request with an empty acceptable CA list, which
   * causes SHIP devices (e.g. K40RF) to respond with their self-signed cert
   * unconditionally. Setting cacert_pem to our own cert would cause the
   * Certificate Request to list only our cert's subject — SHIP peers that
   * can't find a matching issuer would then send an empty Certificate message,
   * making peer_cert NULL and SKI extraction impossible. */
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

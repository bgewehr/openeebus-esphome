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
 * @file websocket_server_esp32.c
 * @brief ESP32 WebSocket Server implementation for openeebus SHIP layer.
 *
 * Replaces the libwebsockets-based websocket_server.c with an implementation
 * using ESP-IDF's esp_http_server (httpd) with WebSocket upgrade support.
 *
 * Architecture:
 *   ShipMdns announces the SHIP service via esp_mdns.
 *   The CLS-Steuerbox (EG) connects to us as a WebSocket client.
 *   We act as WebSocket server (CS role = controllable system).
 *
 * ESP-IDF components required:
 *   esp_http_server, mbedtls, esp_netif
 */

#include "websocket_server_esp32.h"

#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_tls.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <mbedtls/pk.h>
#include <mbedtls/x509_crt.h>
#include <string.h>

#include "src/common/debug.h"
#include "src/common/eebus_malloc.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/websocket_interface.h"

static const char* TAG = "eebus_ws_srv";

/* -------------------------------------------------------------------------
 * WebsocketServer object (implements WebsocketObject interface)
 * ---------------------------------------------------------------------- */

typedef struct WebsocketServerEsp32 WebsocketServerEsp32;

struct WebsocketServerEsp32 {
  /** Implements WebsocketObject — must be first member */
  WebsocketObject obj;

  WebsocketCallback callback;
  void*             callback_ctx;

  httpd_handle_t    server;
  int               fd;           /**< Connected client socket fd, -1 if none */
  bool              closed;
  int32_t           close_error;

  SemaphoreHandle_t write_mutex;

  /** Receive buffer (assembled across fragments) */
  uint8_t*          rx_buf;
  size_t            rx_buf_len;
  size_t            rx_buf_cap;

  /** TLS cert/key in DER format (owned) */
  uint8_t*          cert_der;
  size_t            cert_der_len;
  uint8_t*          key_der;
  size_t            key_der_len;
};

#define WS_SERVER(obj) ((WebsocketServerEsp32*)(obj))

/* -------------------------------------------------------------------------
 * Forward declarations
 * ---------------------------------------------------------------------- */
static void     Destruct(WebsocketObject* self);
static int32_t  Write(WebsocketObject* self, const uint8_t* msg, size_t msg_size);
static void     Close(WebsocketObject* self, int32_t close_code, const char* reason);
static bool     IsClosed(const WebsocketObject* self);
static int32_t  GetCloseError(const WebsocketObject* self);
static void     ScheduleWrite(WebsocketObject* self);

static const WebsocketInterface kWebsocketServerMethods = {
  .destruct        = Destruct,
  .write           = Write,
  .close           = Close,
  .is_closed       = IsClosed,
  .get_close_error = GetCloseError,
  .schedule_write  = ScheduleWrite,
};

/* -------------------------------------------------------------------------
 * httpd WebSocket handler
 * ---------------------------------------------------------------------- */

/**
 * @brief Called by httpd for every WS frame on the SHIP path.
 */
static esp_err_t ShipWsHandler(httpd_req_t* req) {
  WebsocketServerEsp32* const ws = (WebsocketServerEsp32*)req->user_ctx;

  if (req->method == HTTP_GET) {
    /* Handshake — store client fd */
    ws->fd     = httpd_req_to_sockfd(req);
    ws->closed = false;
    ESP_LOGI(TAG, "SHIP WS handshake complete, fd=%d", ws->fd);
    return ESP_OK;
  }

  /* Receive frame */
  httpd_ws_frame_t frame = {
    .type    = HTTPD_WS_TYPE_BINARY,
    .payload = NULL,
  };

  /* Probe length */
  esp_err_t ret = httpd_ws_recv_frame(req, &frame, 0);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "httpd_ws_recv_frame probe failed: %d", ret);
    return ret;
  }

  if (frame.len == 0) {
    if (frame.type == HTTPD_WS_TYPE_CLOSE) {
      ws->closed = true;
      if (ws->callback) {
        ws->callback(kWebsocketCallbackTypeClose, NULL, 0, ws->callback_ctx);
      }
    }
    return ESP_OK;
  }

  /* Grow receive buffer if needed */
  size_t needed = ws->rx_buf_len + frame.len;
  if (needed > ws->rx_buf_cap) {
    uint8_t* nb = EEBUS_MALLOC(needed);
    if (!nb) {
      ESP_LOGE(TAG, "rx_buf alloc failed");
      return ESP_ERR_NO_MEM;
    }
    if (ws->rx_buf) {
      memcpy(nb, ws->rx_buf, ws->rx_buf_len);
      EEBUS_FREE(ws->rx_buf);
    }
    ws->rx_buf     = nb;
    ws->rx_buf_cap = needed;
  }

  frame.payload = ws->rx_buf + ws->rx_buf_len;
  ret           = httpd_ws_recv_frame(req, &frame, frame.len);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "httpd_ws_recv_frame recv failed: %d", ret);
    return ret;
  }
  ws->rx_buf_len += frame.len;

  /* Deliver complete message (SHIP frames are always single-fragment binary) */
  if (frame.final) {
    if (ws->callback) {
      ws->callback(
          kWebsocketCallbackTypeRead,
          ws->rx_buf,
          ws->rx_buf_len,
          ws->callback_ctx);
    }
    ws->rx_buf_len = 0;
  }

  return ESP_OK;
}

/* -------------------------------------------------------------------------
 * WebsocketObject interface implementation
 * ---------------------------------------------------------------------- */

static void Destruct(WebsocketObject* self) {
  WebsocketServerEsp32* const ws = WS_SERVER(self);

  if (ws->server) {
    httpd_stop(ws->server);
    ws->server = NULL;
  }
  if (ws->write_mutex) {
    vSemaphoreDelete(ws->write_mutex);
    ws->write_mutex = NULL;
  }
  if (ws->rx_buf) {
    EEBUS_FREE(ws->rx_buf);
    ws->rx_buf = NULL;
  }
  if (ws->cert_der) {
    EEBUS_FREE(ws->cert_der);
    ws->cert_der = NULL;
  }
  if (ws->key_der) {
    EEBUS_FREE(ws->key_der);
    ws->key_der = NULL;
  }
  EEBUS_FREE(ws);
}

static int32_t Write(WebsocketObject* self, const uint8_t* msg, size_t msg_size) {
  WebsocketServerEsp32* const ws = WS_SERVER(self);

  if (ws->closed || ws->fd < 0) {
    return -1;
  }

  xSemaphoreTake(ws->write_mutex, portMAX_DELAY);

  httpd_ws_frame_t frame = {
    .final   = true,
    .fragmented = false,
    .type    = HTTPD_WS_TYPE_BINARY,
    .payload = (uint8_t*)msg,
    .len     = msg_size,
  };

  esp_err_t ret = httpd_ws_send_frame_async(ws->server, ws->fd, &frame);
  xSemaphoreGive(ws->write_mutex);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "httpd_ws_send_frame_async failed: %d", ret);
    return -1;
  }
  return (int32_t)msg_size;
}

static void Close(WebsocketObject* self, int32_t close_code, const char* reason) {
  WebsocketServerEsp32* const ws = WS_SERVER(self);

  (void)reason;
  ws->close_error = close_code;
  ws->closed      = true;

  if (ws->fd >= 0) {
    httpd_ws_frame_t frame = {
      .final   = true,
      .type    = HTTPD_WS_TYPE_CLOSE,
      .payload = NULL,
      .len     = 0,
    };
    httpd_ws_send_frame_async(ws->server, ws->fd, &frame);
    ws->fd = -1;
  }
}

static bool IsClosed(const WebsocketObject* self) {
  return WS_SERVER(self)->closed;
}

static int32_t GetCloseError(const WebsocketObject* self) {
  return WS_SERVER(self)->close_error;
}

static void ScheduleWrite(WebsocketObject* self) {
  /* esp_http_server is synchronous per-request; no explicit scheduling needed */
  (void)self;
}

/* -------------------------------------------------------------------------
 * WebsocketServerCreator — creates and starts the httpd TLS server
 * ---------------------------------------------------------------------- */

typedef struct WebsocketServerCreatorEsp32 WebsocketServerCreatorEsp32;

struct WebsocketServerCreatorEsp32 {
  WebsocketCreatorObject obj;

  uint16_t    port;
  const void* cert_der;
  size_t      cert_der_len;
  const void* key_der;
  size_t      key_der_len;
};

#define WS_SERVER_CREATOR(obj) ((WebsocketServerCreatorEsp32*)(obj))

static void           CreatorDestruct(WebsocketCreatorObject* self);
static WebsocketObject* CreatorCreateWebsocket(
    WebsocketCreatorObject* self,
    WebsocketCallback cb,
    void* ctx);

static const WebsocketCreatorInterface kWebsocketServerCreatorMethods = {
  .destruct          = CreatorDestruct,
  .create_websocket  = CreatorCreateWebsocket,
};

static void CreatorDestruct(WebsocketCreatorObject* self) {
  EEBUS_FREE(WS_SERVER_CREATOR(self));
}

static WebsocketObject* CreatorCreateWebsocket(
    WebsocketCreatorObject* self,
    WebsocketCallback       cb,
    void*                   ctx)
{
  WebsocketServerCreatorEsp32* const creator = WS_SERVER_CREATOR(self);

  WebsocketServerEsp32* const ws =
      (WebsocketServerEsp32*)EEBUS_MALLOC(sizeof(WebsocketServerEsp32));
  if (!ws) {
    ESP_LOGE(TAG, "Failed to allocate WebsocketServerEsp32");
    return NULL;
  }
  memset(ws, 0, sizeof(*ws));

  WEBSOCKET_INTERFACE(ws) = &kWebsocketServerMethods;
  ws->callback            = cb;
  ws->callback_ctx        = ctx;
  ws->fd                  = -1;
  ws->closed              = false;
  ws->close_error         = 0;
  ws->write_mutex         = xSemaphoreCreateMutex();

  /* Copy cert/key for TLS config lifetime */
  ws->cert_der_len = creator->cert_der_len;
  ws->cert_der     = EEBUS_MALLOC(ws->cert_der_len);
  ws->key_der_len  = creator->key_der_len;
  ws->key_der      = EEBUS_MALLOC(ws->key_der_len);

  if (!ws->cert_der || !ws->key_der) {
    ESP_LOGE(TAG, "TLS buffer alloc failed");
    Destruct(WEBSOCKET_OBJECT(ws));
    return NULL;
  }

  memcpy(ws->cert_der, creator->cert_der, ws->cert_der_len);
  memcpy(ws->key_der,  creator->key_der,  ws->key_der_len);

  /* Configure httpd with TLS (mutual auth) */
  httpd_ssl_config_t ssl_cfg = HTTPD_SSL_CONFIG_DEFAULT();
  ssl_cfg.port_secure         = creator->port;
  ssl_cfg.servercert           = ws->cert_der;
  ssl_cfg.servercert_len       = ws->cert_der_len;
  ssl_cfg.prvtkey_pem          = ws->key_der;
  ssl_cfg.prvtkey_len          = ws->key_der_len;

  /* Client cert required (mutual TLS per SHIP spec) */
  ssl_cfg.httpd_config.max_open_sockets = 4;

  esp_err_t ret = httpd_ssl_start(&ws->server, &ssl_cfg);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "httpd_ssl_start failed: %d", ret);
    Destruct(WEBSOCKET_OBJECT(ws));
    return NULL;
  }

  /* Register SHIP WebSocket URI */
  const httpd_uri_t ship_uri = {
    .uri      = "/ship/",
    .method   = HTTP_GET,
    .handler  = ShipWsHandler,
    .user_ctx = ws,
    .is_websocket = true,
  };

  ret = httpd_register_uri_handler(ws->server, &ship_uri);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "httpd_register_uri_handler failed: %d", ret);
    Destruct(WEBSOCKET_OBJECT(ws));
    return NULL;
  }

  ESP_LOGI(TAG, "SHIP WebSocket server started on port %d", creator->port);
  return WEBSOCKET_OBJECT(ws);
}

/* -------------------------------------------------------------------------
 * Public constructor
 * ---------------------------------------------------------------------- */

WebsocketCreatorObject* WebsocketServerCreatorEsp32Create(
    uint16_t    port,
    const void* cert_der,
    size_t      cert_der_len,
    const void* key_der,
    size_t      key_der_len)
{
  WebsocketServerCreatorEsp32* const creator =
      (WebsocketServerCreatorEsp32*)EEBUS_MALLOC(sizeof(WebsocketServerCreatorEsp32));
  if (!creator) {
    return NULL;
  }

  WEBSOCKET_CREATOR_INTERFACE(creator) = &kWebsocketServerCreatorMethods;
  creator->port         = port;
  creator->cert_der     = cert_der;
  creator->cert_der_len = cert_der_len;
  creator->key_der      = key_der;
  creator->key_der_len  = key_der_len;

  return WEBSOCKET_CREATOR_OBJECT(creator);
}

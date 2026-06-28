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
 * @file websocket_client_esp32.c
 * @brief ESP32 WebSocket Client implementation for openeebus SHIP layer.
 *
 * Replaces the libwebsockets-based websocket_client.c with an implementation
 * using ESP-IDF's esp_websocket_client component.
 *
 * Used when the HEMS initiates the connection to a remote SHIP device
 * (e.g. a Vaillant heat pump acting as CS, or during initial pairing flows).
 *
 * ESP-IDF components required:
 *   esp_websocket_client, mbedtls
 */

#include "websocket_client_esp32.h"

#include <esp_log.h>
#include <esp_websocket_client.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <string.h>

#include "src/common/debug.h"
#include "src/common/eebus_malloc.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/websocket_interface.h"

static const char* TAG = "eebus_ws_cli";

/* -------------------------------------------------------------------------
 * WebsocketClient object (implements WebsocketObject interface)
 * ---------------------------------------------------------------------- */

typedef struct WebsocketClientEsp32 WebsocketClientEsp32;

struct WebsocketClientEsp32 {
  /** Implements WebsocketObject — must be first member */
  WebsocketObject obj;

  WebsocketCallback cb;
  void*             cb_ctx;

  esp_websocket_client_handle_t handle;
  bool                          closed;
  int32_t                       close_error;

  SemaphoreHandle_t write_mutex;

  /** Reassembly buffer for fragmented frames */
  uint8_t* rx_buf;
  size_t   rx_len;
  size_t   rx_cap;
};

#define WS_CLIENT(obj) ((WebsocketClientEsp32*)(obj))

/* -------------------------------------------------------------------------
 * Forward declarations
 * ---------------------------------------------------------------------- */
static void     Destruct(WebsocketObject* self);
static int32_t  Write(WebsocketObject* self, const uint8_t* msg, size_t msg_size);
static void     Close(WebsocketObject* self, int32_t close_code, const char* reason);
static bool     IsClosed(const WebsocketObject* self);
static int32_t  GetCloseError(const WebsocketObject* self);
static void     ScheduleWrite(WebsocketObject* self);

static const WebsocketInterface kWebsocketClientMethods = {
  .destruct        = Destruct,
  .write           = Write,
  .close           = Close,
  .is_closed       = IsClosed,
  .get_close_error = GetCloseError,
  .schedule_write  = ScheduleWrite,
};

/* -------------------------------------------------------------------------
 * esp_websocket_client event handler
 * ---------------------------------------------------------------------- */

static void WsEventHandler(
    void*                    handler_args,
    esp_event_base_t         base,
    int32_t                  event_id,
    void*                    event_data)
{
  WebsocketClientEsp32* const ws = (WebsocketClientEsp32*)handler_args;
  esp_websocket_event_data_t* const data =
      (esp_websocket_event_data_t*)event_data;

  switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
      ESP_LOGI(TAG, "SHIP WS client connected");
      ws->closed = false;
      break;

    case WEBSOCKET_EVENT_DATA:
      if (data->op_code == 0x2 /* binary */ || data->op_code == 0x0 /* continuation */) {
        /* Grow buffer if needed */
        size_t needed = ws->rx_len + data->data_len;
        if (needed > ws->rx_cap) {
          uint8_t* nb = EEBUS_MALLOC(needed);
          if (!nb) {
            ESP_LOGE(TAG, "rx_buf alloc failed");
            break;
          }
          if (ws->rx_buf) {
            memcpy(nb, ws->rx_buf, ws->rx_len);
            EEBUS_FREE(ws->rx_buf);
          }
          ws->rx_buf = nb;
          ws->rx_cap = needed;
        }
        memcpy(ws->rx_buf + ws->rx_len, data->data_ptr, data->data_len);
        ws->rx_len += data->data_len;

        /* Deliver when final fragment received */
        if (data->fin) {
          if (ws->cb) {
            ws->cb(kWebsocketCallbackTypeRead, ws->rx_buf, ws->rx_len, ws->cb_ctx);
          }
          ws->rx_len = 0;
        }
      } else if (data->op_code == 0x8 /* close */) {
        ws->closed = true;
        if (ws->cb) {
          ws->cb(kWebsocketCallbackTypeClose, NULL, 0, ws->cb_ctx);
        }
      }
      break;

    case WEBSOCKET_EVENT_ERROR:
      ESP_LOGE(TAG, "SHIP WS client error");
      ws->close_error = -1;
      ws->closed      = true;
      if (ws->cb) {
        ws->cb(kWebsocketCallbackTypeError, NULL, 0, ws->cb_ctx);
      }
      break;

    case WEBSOCKET_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "SHIP WS client disconnected");
      ws->closed = true;
      if (ws->cb) {
        ws->cb(kWebsocketCallbackTypeClose, NULL, 0, ws->cb_ctx);
      }
      break;

    default:
      break;
  }
}

/* -------------------------------------------------------------------------
 * WebsocketObject interface implementation
 * ---------------------------------------------------------------------- */

static void Destruct(WebsocketObject* self) {
  WebsocketClientEsp32* const ws = WS_CLIENT(self);

  if (ws->handle) {
    esp_websocket_client_stop(ws->handle);
    esp_websocket_client_destroy(ws->handle);
    ws->handle = NULL;
  }
  if (ws->write_mutex) {
    vSemaphoreDelete(ws->write_mutex);
    ws->write_mutex = NULL;
  }
  if (ws->rx_buf) {
    EEBUS_FREE(ws->rx_buf);
    ws->rx_buf = NULL;
  }
  /* Do NOT free ws here — WebsocketDelete() handles that after calling Destruct */
}

static int32_t Write(WebsocketObject* self, const uint8_t* msg, size_t msg_size) {
  WebsocketClientEsp32* const ws = WS_CLIENT(self);

  if (ws->closed || !ws->handle) {
    return -1;
  }

  xSemaphoreTake(ws->write_mutex, portMAX_DELAY);
  int ret = esp_websocket_client_send_bin(ws->handle, (const char*)msg, (int)msg_size, portMAX_DELAY);
  xSemaphoreGive(ws->write_mutex);

  if (ret < 0) {
    ESP_LOGE(TAG, "esp_websocket_client_send_bin failed: %d", ret);
    return -1;
  }
  return (int32_t)msg_size;
}

static void Close(WebsocketObject* self, int32_t close_code, const char* reason) {
  WebsocketClientEsp32* const ws = WS_CLIENT(self);
  (void)reason;

  ws->close_error = close_code;
  ws->closed      = true;

  if (ws->handle) {
    esp_websocket_client_close(ws->handle, portMAX_DELAY);
  }
}

static bool IsClosed(const WebsocketObject* self) {
  return WS_CLIENT(self)->closed;
}

static int32_t GetCloseError(const WebsocketObject* self) {
  return WS_CLIENT(self)->close_error;
}

static void ScheduleWrite(WebsocketObject* self) {
  /* esp_websocket_client sends synchronously; no scheduling needed */
  (void)self;
}

/* -------------------------------------------------------------------------
 * WebsocketClientCreator — creates and connects the esp_websocket_client
 * ---------------------------------------------------------------------- */

typedef struct WebsocketClientCreatorEsp32 WebsocketClientCreatorEsp32;

struct WebsocketClientCreatorEsp32 {
  WebsocketCreatorObject obj;

  char*   uri;          /**< wss://host:port/ship/ */
  uint8_t* cert_der;    /**< Our client certificate */
  size_t   cert_der_len;
  uint8_t* key_der;     /**< Our private key */
  size_t   key_der_len;
  uint8_t* ca_der;      /**< Remote CA / server cert for verification */
  size_t   ca_der_len;
};

#define WS_CLIENT_CREATOR(obj) ((WebsocketClientCreatorEsp32*)(obj))

static void             CreatorDestruct(WebsocketCreatorObject* self);
static WebsocketObject* CreatorCreateWebsocket(
    WebsocketCreatorObject* self,
    WebsocketCallback       cb,
    void*                   ctx);

static const WebsocketCreatorInterface kWebsocketClientCreatorMethods = {
  .destruct         = CreatorDestruct,
  .create_websocket = CreatorCreateWebsocket,
};

static void CreatorDestruct(WebsocketCreatorObject* self) {
  WebsocketClientCreatorEsp32* const c = WS_CLIENT_CREATOR(self);
  if (c->uri)      { EEBUS_FREE(c->uri); }
  if (c->cert_der) { EEBUS_FREE(c->cert_der); }
  if (c->key_der)  { EEBUS_FREE(c->key_der); }
  if (c->ca_der)   { EEBUS_FREE(c->ca_der); }
  /* Do NOT free c here — WebsocketCreatorDelete() handles that after calling Destruct */
}

static WebsocketObject* CreatorCreateWebsocket(
    WebsocketCreatorObject* self,
    WebsocketCallback       cb,
    void*                   ctx)
{
  WebsocketClientCreatorEsp32* const creator = WS_CLIENT_CREATOR(self);

  WebsocketClientEsp32* const ws =
      (WebsocketClientEsp32*)EEBUS_MALLOC(sizeof(WebsocketClientEsp32));
  if (!ws) {
    return NULL;
  }
  memset(ws, 0, sizeof(*ws));

  WEBSOCKET_INTERFACE(ws) = &kWebsocketClientMethods;
  ws->cb           = cb;
  ws->cb_ctx       = ctx;
  ws->closed       = false;
  ws->close_error  = 0;
  ws->write_mutex  = xSemaphoreCreateMutex();

  esp_websocket_client_config_t cfg = {
    .uri              = creator->uri,
    .client_cert      = (const char*)creator->cert_der,
    .client_cert_len  = creator->cert_der_len,
    .client_key       = (const char*)creator->key_der,
    .client_key_len   = creator->key_der_len,
    .cert_pem         = (const char*)creator->ca_der,
    .cert_len         = (int)creator->ca_der_len,
    /* SHIP uses binary WebSocket frames */
    .subprotocol      = "ship",
    .transport        = WEBSOCKET_TRANSPORT_OVER_SSL,
    .task_stack       = 8192,
  };

  ws->handle = esp_websocket_client_init(&cfg);
  if (!ws->handle) {
    ESP_LOGE(TAG, "esp_websocket_client_init failed");
    Destruct(WEBSOCKET_OBJECT(ws));
    return NULL;
  }

  esp_websocket_register_events(
      ws->handle,
      WEBSOCKET_EVENT_ANY,
      WsEventHandler,
      ws);

  esp_err_t ret = esp_websocket_client_start(ws->handle);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "esp_websocket_client_start failed: %d", ret);
    Destruct(WEBSOCKET_OBJECT(ws));
    return NULL;
  }

  ESP_LOGI(TAG, "SHIP WS client connecting to %s", creator->uri);
  return WEBSOCKET_OBJECT(ws);
}

/* -------------------------------------------------------------------------
 * Public constructor
 * ---------------------------------------------------------------------- */

WebsocketCreatorObject* WebsocketClientCreatorEsp32Create(
    const char* uri,
    const void* cert_der,
    size_t      cert_der_len,
    const void* key_der,
    size_t      key_der_len,
    const void* ca_der,
    size_t      ca_der_len)
{
  WebsocketClientCreatorEsp32* const creator =
      (WebsocketClientCreatorEsp32*)EEBUS_MALLOC(sizeof(WebsocketClientCreatorEsp32));
  if (!creator) {
    return NULL;
  }
  memset(creator, 0, sizeof(*creator));

  WEBSOCKET_CREATOR_INTERFACE(creator) = &kWebsocketClientCreatorMethods;

  creator->uri = (char*)EEBUS_MALLOC(strlen(uri) + 1);
  if (!creator->uri) { CreatorDestruct(WEBSOCKET_CREATOR_OBJECT(creator)); return NULL; }
  strcpy(creator->uri, uri);

  creator->cert_der_len = cert_der_len;
  creator->cert_der     = (uint8_t*)EEBUS_MALLOC(cert_der_len);
  if (!creator->cert_der) { CreatorDestruct(WEBSOCKET_CREATOR_OBJECT(creator)); return NULL; }
  memcpy(creator->cert_der, cert_der, cert_der_len);

  creator->key_der_len = key_der_len;
  creator->key_der     = (uint8_t*)EEBUS_MALLOC(key_der_len);
  if (!creator->key_der) { CreatorDestruct(WEBSOCKET_CREATOR_OBJECT(creator)); return NULL; }
  memcpy(creator->key_der, key_der, key_der_len);

  if (ca_der && ca_der_len > 0) {
    creator->ca_der_len = ca_der_len;
    creator->ca_der     = (uint8_t*)EEBUS_MALLOC(ca_der_len);
    if (!creator->ca_der) { CreatorDestruct(WEBSOCKET_CREATOR_OBJECT(creator)); return NULL; }
    memcpy(creator->ca_der, ca_der, ca_der_len);
  }

  return WEBSOCKET_CREATOR_OBJECT(creator);
}

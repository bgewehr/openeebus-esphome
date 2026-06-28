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
 * @file websocket_server_esp32.c
 * @brief ESP32 WebSocket Server implementation for openeebus SHIP layer.
 *
 * Wraps an already-accepted httpd WebSocket connection (server + sockfd).
 * Does NOT start a second httpd instance — the httpd is owned by
 * http_server_esp32.c which handles all URI callbacks.
 *
 * Data frames are delivered via WebsocketServerEsp32DeliverData(), called
 * from the http_server_esp32.c URI handler for each received frame.
 */

#include "websocket_server_esp32.h"

#include <esp_http_server.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <string.h>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/websocket_interface.h"

static const char* TAG = "eebus_ws_srv";

/* -------------------------------------------------------------------------
 * WebsocketServerEsp32 — implements WebsocketObject
 * ---------------------------------------------------------------------- */

typedef struct WebsocketServerEsp32 WebsocketServerEsp32;

struct WebsocketServerEsp32 {
  WebsocketObject   obj;        /* must be first — implements WebsocketObject */

  WebsocketCallback callback;
  void*             callback_ctx;

  httpd_handle_t    server;     /* httpd handle — NOT owned, do not stop */
  int               fd;         /* socket fd of the accepted WS connection */
  bool              closed;
  int32_t           close_error;

  SemaphoreHandle_t write_mutex;
};

#define WS_SERVER(obj) ((WebsocketServerEsp32*)(obj))

/* Forward declarations */
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

static void Destruct(WebsocketObject* self) {
  WebsocketServerEsp32* const ws = WS_SERVER(self);
  /* httpd is NOT owned — do not call httpd_stop.
   * Just send a close frame if still open and release resources. */
  if (!ws->closed && ws->fd >= 0) {
    httpd_ws_frame_t frame = {
      .final   = true,
      .type    = HTTPD_WS_TYPE_CLOSE,
      .payload = NULL,
      .len     = 0,
    };
    httpd_ws_send_frame_async(ws->server, ws->fd, &frame);
  }
  if (ws->write_mutex) {
    vSemaphoreDelete(ws->write_mutex);
    ws->write_mutex = NULL;
  }
  EEBUS_FREE(ws);
}

static int32_t Write(WebsocketObject* self, const uint8_t* msg, size_t msg_size) {
  WebsocketServerEsp32* const ws = WS_SERVER(self);

  if (ws->closed || ws->fd < 0 || ws->server == NULL) {
    return -1;
  }

  xSemaphoreTake(ws->write_mutex, portMAX_DELAY);

  httpd_ws_frame_t frame = {
    .final      = true,
    .fragmented = false,
    .type       = HTTPD_WS_TYPE_BINARY,
    .payload    = (uint8_t*)msg,
    .len        = msg_size,
  };

  esp_err_t ret = httpd_ws_send_frame_async(ws->server, ws->fd, &frame);
  xSemaphoreGive(ws->write_mutex);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "httpd_ws_send_frame_async failed: %d (fd=%d)", ret, ws->fd);
    return -1;
  }
  return (int32_t)msg_size;
}

static void Close(WebsocketObject* self, int32_t close_code, const char* reason) {
  WebsocketServerEsp32* const ws = WS_SERVER(self);
  (void)reason;
  ws->close_error = close_code;
  ws->closed      = true;

  if (ws->fd >= 0 && ws->server) {
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
  /* esp_http_server is synchronous per-request; writes go async via
   * httpd_ws_send_frame_async so no explicit scheduling is needed. */
  (void)self;
}

/* -------------------------------------------------------------------------
 * Public helpers called from http_server_esp32.c
 * ---------------------------------------------------------------------- */

void WebsocketServerEsp32DeliverData(WebsocketObject* self,
                                     const uint8_t* data, size_t len) {
  WebsocketServerEsp32* const ws = WS_SERVER(self);
  if (ws->closed || !ws->callback) return;
  ws->callback(kWebsocketCallbackTypeRead, data, len, ws->callback_ctx);
}

void WebsocketServerEsp32NotifyClose(WebsocketObject* self) {
  WebsocketServerEsp32* const ws = WS_SERVER(self);
  if (ws->closed) return;
  ws->closed = true;
  ws->fd     = -1;
  if (ws->callback) {
    ws->callback(kWebsocketCallbackTypeClose, NULL, 0, ws->callback_ctx);
  }
}

/* -------------------------------------------------------------------------
 * WebsocketServerCreatorEsp32 — implements WebsocketCreatorObject
 * ---------------------------------------------------------------------- */

typedef struct WebsocketServerCreatorEsp32 WebsocketServerCreatorEsp32;

struct WebsocketServerCreatorEsp32 {
  WebsocketCreatorObject obj;   /* must be first */

  httpd_handle_t        server;
  int                   sockfd;

  WebsocketObject*      created_ws;  /* set by CreatorCreateWebsocket */
};

#define WS_SERVER_CREATOR(obj) ((WebsocketServerCreatorEsp32*)(obj))

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
    creator->created_ws = NULL;
    return NULL;
  }
  memset(ws, 0, sizeof(*ws));

  WEBSOCKET_INTERFACE(ws)  = &kWebsocketServerMethods;
  ws->callback             = cb;
  ws->callback_ctx         = ctx;
  ws->server               = creator->server;
  ws->fd                   = creator->sockfd;
  ws->closed               = false;
  ws->close_error          = 0;
  ws->write_mutex          = xSemaphoreCreateMutex();

  if (!ws->write_mutex) {
    ESP_LOGE(TAG, "Failed to create write mutex");
    EEBUS_FREE(ws);
    creator->created_ws = NULL;
    return NULL;
  }

  ESP_LOGI(TAG, "WebSocket server session created (fd=%d)", ws->fd);
  creator->created_ws = WEBSOCKET_OBJECT(ws);
  return WEBSOCKET_OBJECT(ws);
}

static const WebsocketCreatorInterface kWebsocketServerCreatorMethods = {
  .destruct          = CreatorDestruct,
  .create_websocket  = CreatorCreateWebsocket,
};

/* -------------------------------------------------------------------------
 * Public constructor
 * ---------------------------------------------------------------------- */

WebsocketCreatorObject* WebsocketServerCreatorEsp32Create(
    httpd_handle_t server,
    int            sockfd)
{
  WebsocketServerCreatorEsp32* const creator =
      (WebsocketServerCreatorEsp32*)EEBUS_MALLOC(sizeof(WebsocketServerCreatorEsp32));
  if (!creator) {
    return NULL;
  }
  memset(creator, 0, sizeof(*creator));

  WEBSOCKET_CREATOR_INTERFACE(creator) = &kWebsocketServerCreatorMethods;
  creator->server     = server;
  creator->sockfd     = sockfd;
  creator->created_ws = NULL;

  return WEBSOCKET_CREATOR_OBJECT(creator);
}

WebsocketObject* WebsocketServerCreatorEsp32GetCreatedWs(
    WebsocketCreatorObject* creator)
{
  return WS_SERVER_CREATOR(creator)->created_ws;
}

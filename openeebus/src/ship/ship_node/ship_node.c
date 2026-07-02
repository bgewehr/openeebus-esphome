/*
 * Copyright 2025 NIBE AB
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

#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ship_node_internal.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_mutex/eebus_mutex.h"
#include "src/common/eebus_queue/eebus_queue.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/service_details.h"
#include "src/common/vector.h"
#include "src/ship/api/http_server_interface.h"
#include "src/ship/api/ship_node_interface.h"
#include "src/ship/api/ship_node_reader_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/mdns/ship_mdns.h"
#include "src/ship/ship_connection/ship_connection.h"
#include "src/ship/websocket/http_server.h"
#include "src/ship/websocket/websocket_client_creator.h"

/** Set SHIP_NODE_DEBUG 1 to enable debug prints */
#ifndef SHIP_NODE_DEBUG
#define SHIP_NODE_DEBUG 0
#endif

/** Ship node debug printf(), enabled whith SHIP_NODE_DEBUG = 1 */
#if SHIP_NODE_DEBUG
#define SHIP_NODE_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define SHIP_NODE_DEBUG_PRINTF(fmt, ...)
#endif  // SHIP_NODE_DEBUG

enum ShipNodeQueueMsgType {
  kShipNodeQueueMsgTypeCancel,
  kShipNodeQueueMsgTypeMdnsEntriesFound,
  kShipNodeQueueMsgTypeShipConnectionClosed,
  kShipNodeQueueMsgTypeShipUnregisterSki,
  kShipNodeQueueMsgTypeShipRegisterSki,
};

typedef enum ShipNodeQueueMsgType ShipNodeQueueMsgType;

typedef struct ShipNodeQueueMessage ShipNodeQueueMessage;

struct ShipNodeQueueMessage {
  ShipNodeQueueMsgType type;
  ShipConnectionObject* ship_connection;
  bool had_error;
  char* ski;
};

static void Destruct(InfoProviderObject* self);
static bool IsRemoteServiceForSkiPaired(InfoProviderObject* self, const char* ski);
static void HandleConnectionClosed(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error);
static void ReportServiceShipId(InfoProviderObject* self, const char* service_id, const char* ship_id);
static bool IsWaitingForTrustAllowed(InfoProviderObject* self, const char* ski);
static void HandleShipStateUpdate(InfoProviderObject* self, const char* ski, SmeState state, const char* err);
static DataReaderObject* SetupRemoteDevice(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer);
static void Start(ShipNodeObject* self);
static void Stop(ShipNodeObject* self);
static void RegisterRemoteSki(ShipNodeObject* self, const char* ski, bool is_trusted);
static void UnregisterRemoteSki(ShipNodeObject* self, const char* ski);
static void CancelPairingWithSki(ShipNodeObject* self, const char* ski);
static void ShipNodeUnregisterSki(ShipNodeObject* self, const char* ski);
static void ShipNodeRegisterSki(ShipNodeObject* self, const char* ski, bool is_trusted);

static const ShipNodeInterface ship_node_methods = {
    .info_provider_interface = {
        .destruct                         = Destruct,
        .is_remote_service_for_ski_paired = IsRemoteServiceForSkiPaired,
        .handle_connection_closed         = HandleConnectionClosed,
        .report_service_ship_id           = ReportServiceShipId,
        .is_waiting_for_trust_allowed     = IsWaitingForTrustAllowed,
        .handle_ship_state_update         = HandleShipStateUpdate,
        .setup_remote_device              = SetupRemoteDevice,
    },

    .start                   = Start,
    .stop                    = Stop,
    .register_remote_ski     = RegisterRemoteSki,
    .unregister_remote_ski   = UnregisterRemoteSki,
    .cancel_pairing_with_ski = CancelPairingWithSki,
};

static void ShipNodeConstruct(
    ShipNode* self,
    const char* ski,
    const char* role,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    const TlsCertificateObject* tsl_certificate,
    ShipNodeReaderObject* ship_node_reader,
    ServiceDetails* local_service_details
);

static void ShipNodeOnMdnsEntriesFoundCallback(Vector* found_entries, void* ctx);
static bool SkiMatches(const char* ski_a, const char* ski_b);
static void CloseShipConnection(ShipNode* self, ShipConnectionObject* sc, bool had_error);
static bool ShipNodeFindService(ShipNode* self, MdnsEntry* found_entry);
static void ShipNodeConnectToService(ShipNode* self, const MdnsEntry* found_entry);
static void ShipNodeConnectToRemoteSki(ShipNode* self);
static void* ShipNodeConnectionLoop(void* ctx);
static int
ShipNodeOnWebsocketServerConnectionCallback(const char* ski, WebsocketCreatorObject* websocket_creator, void* ctx);
static bool ShipNodeIsClientSupported(ShipNode* self);
static bool ShipNodeIsServerSupported(ShipNode* self);

static void ShipNodeQueueMsgDeallocator(void* msg) {
  if (msg == NULL) {
    return;
  }

  ShipNodeQueueMessage* queue_msg = (ShipNodeQueueMessage*)msg;
  StringDelete(queue_msg->ski);
  queue_msg->ski = NULL;
}

void ShipNodeConstruct(
    ShipNode* self,
    const char* ski,
    const char* role,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    const TlsCertificateObject* tsl_certificate,
    ShipNodeReaderObject* ship_node_reader,
    ServiceDetails* local_service_details
) {
  // Override "virtual function table"
  SHIP_NODE_INTERFACE(self) = &ship_node_methods;

  self->mdns = ShipMdnsCreate(ski, device_info, service_name, port, ShipNodeOnMdnsEntriesFoundCallback, self);

  static const size_t kQueueMaxMsg = 10;

  self->msg_queue = EebusQueueCreate(kQueueMaxMsg, sizeof(ShipNodeQueueMessage), ShipNodeQueueMsgDeallocator);

  self->mdns_entries          = VectorCreateWithDeallocator(MdnsEntryDeallocator);
  self->mutex                 = EebusMutexCreate();
  self->search_for_remote_ski = false;
  self->cancel                = false;
  self->connection_thread     = NULL;

  self->remote_ski = NULL;

  self->connections_table     = NULL;
  self->ship_node_reader      = ship_node_reader;
  self->tsl_certificate       = tsl_certificate;
  self->local_service_details = local_service_details;

  self->http_server = HttpServerCreate(port, tsl_certificate, ShipNodeOnWebsocketServerConnectionCallback, self);

  self->websocket_creator          = NULL;
  self->connection_attempt_running = false;

  if (strcmp(role, "server") == 0) {
    self->role = kShipRoleServer;
  } else if (strcmp(role, "client") == 0) {
    self->role = kShipRoleClient;
  } else {
    self->role = kShipRoleAuto;
  }

  self->ship_connection        = NULL;
  self->cancel_ship_connection = NULL;
}

ShipNodeObject* ShipNodeCreate(
    const char* ski,
    const char* role,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    const TlsCertificateObject* tls_certificate,
    ShipNodeReaderObject* ship_node_reader,
    ServiceDetails* local_service_details
) {
  ShipNode* const sn = (ShipNode*)EEBUS_MALLOC(sizeof(ShipNode));

  ShipNodeConstruct(
      sn,
      ski,
      role,
      device_info,
      service_name,
      port,
      tls_certificate,
      ship_node_reader,
      local_service_details
  );

  return SHIP_NODE_OBJECT(sn);
}

void Destruct(InfoProviderObject* self) {
  ShipNode* const sn = SHIP_NODE(self);

  StringDelete(sn->remote_ski);
  sn->remote_ski = NULL;

  if (sn->mdns != NULL) {
    SHIP_MDNS_DESTRUCT(sn->mdns);
    EEBUS_FREE(sn->mdns);
    sn->mdns = NULL;
  }

  if (sn->mdns_entries != NULL) {
    VectorFreeElements(sn->mdns_entries);
    VectorDestruct(sn->mdns_entries);
    EEBUS_FREE(sn->mdns_entries);
    sn->mdns_entries = NULL;
  }

  EebusMutexDelete(sn->mutex);
  sn->mutex = NULL;

  if (sn->http_server != NULL) {
    HttpServerDelete(sn->http_server);
    sn->http_server = NULL;
  }

  if (sn->cancel_ship_connection != NULL) {
    SHIP_CONNECTION_STOP(sn->cancel_ship_connection);
    ShipConnectionDelete(sn->cancel_ship_connection);
    sn->cancel_ship_connection = NULL;
  }

  if (sn->ship_connection != NULL) {
    SHIP_CONNECTION_STOP(sn->ship_connection);
    SHIP_CONNECTION_DESTRUCT(sn->ship_connection);
    EEBUS_FREE(sn->ship_connection);
    sn->ship_connection = NULL;
  }

  EebusQueueDelete(sn->msg_queue);
  sn->msg_queue = NULL;

  sn->connection_attempt_running = false;
}

void ShipNodeOnMdnsEntriesFoundCallback(Vector* found_entries, void* ctx) {
  ShipNode* const sn = (ShipNode*)ctx;

  if (sn->cancel) {
    return;
  }

  if (found_entries == NULL) {
    return;
  }

  EEBUS_MUTEX_LOCK(sn->mutex);
  VectorFreeElements(sn->mdns_entries);
  VectorMove(sn->mdns_entries, found_entries);
  EEBUS_FREE(found_entries);
  EEBUS_MUTEX_UNLOCK(sn->mutex);

  sn->search_for_remote_ski = true;
  if (sn->ship_node_reader != NULL) {
    SHIP_NODE_READER_ON_REMOTE_SERVICES_UPDATE(sn->ship_node_reader, sn->mdns_entries);
  }

  if (ShipNodeIsClientSupported(sn)) {
    ShipNodeQueueMessage queue_msg = {
        .type            = kShipNodeQueueMsgTypeMdnsEntriesFound,
        .ship_connection = NULL,
        .had_error       = false,
        .ski             = NULL,
    };

    EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
  }
}

bool IsRemoteServiceForSkiPaired(InfoProviderObject* self, const char* ski) {
  UNUSED(self);
  UNUSED(ski);
  // TODO: Implement method
  return false;
}

void CloseShipConnection(ShipNode* self, ShipConnectionObject* sc, bool had_error) {
  UNUSED(had_error);

  /* A previously in-progress outbound was stopped to accept a trusted inbound.
   * Its close callback fires here — just free the object, no disconnect processing. */
  if (sc != NULL && sc == self->cancel_ship_connection) {
    ShipConnectionDelete(sc);
    self->cancel_ship_connection = NULL;
    return;
  }

  if ((sc == NULL) || (sc != self->ship_connection)) {
    SHIP_NODE_DEBUG_PRINTF("%s(), invalid Ship Connection instance\n", __func__);
    return;
  }

  SHIP_CONNECTION_STOP(sc);
  SHIP_NODE_DEBUG_PRINTF("%s(), connection closed\n", __func__);
  SHIP_NODE_READER_ON_REMOTE_SKI_DISCONNECTED(self->ship_node_reader, SHIP_CONNECTION_GET_REMOTE_SKI(sc));
  ShipConnectionDelete(sc);
  self->ship_connection = NULL;

  self->connection_attempt_running = false;
  // After any connection close, suppress mDNS-triggered outbound attempts for
  // ~25 min so the remote device (K40RF) has time to initiate inbound.
  // Each mDNS cycle is 10-20 s; 100 cycles ≈ 25-33 min.
  self->outbound_skip_remaining = 100;
}

void HandleConnectionClosed(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error) {
  ShipNode* const sn = SHIP_NODE(self);

  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeShipConnectionClosed,
      .ship_connection = sc,
      .had_error       = had_error,
      .ski             = NULL,
  };

  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}

void ReportServiceShipId(InfoProviderObject* self, const char* service_id, const char* ship_id) {
  const ShipNode* const sn = SHIP_NODE(self);
  if (sn->ship_node_reader != NULL) {
    SHIP_NODE_READER_ON_SHIP_ID_UPDATE(sn->ship_node_reader, service_id, ship_id);
  }
}

bool IsWaitingForTrustAllowed(InfoProviderObject* self, const char* ski) {
  UNUSED(self);
  UNUSED(ski);
  // TODO: Implement method
  return false;
}

void HandleShipStateUpdate(InfoProviderObject* self, const char* ski, SmeState state, const char* err) {
  UNUSED(err);
  const ShipNode* const sn = SHIP_NODE(self);

  SHIP_NODE_READER_ON_SHIP_STATE_UPDATE(sn->ship_node_reader, ski, state);

  if (state == kDataExchange) {
    SHIP_NODE_READER_ON_REMOTE_SKI_CONNECTED(sn->ship_node_reader, ski);
  }
}

DataReaderObject* SetupRemoteDevice(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer) {
  const ShipNode* const sn = SHIP_NODE(self);

  return SHIP_NODE_READER_SETUP_REMOTE_DEVICE(sn->ship_node_reader, ski, data_writer);
}

bool SkiMatches(const char* ski_a, const char* ski_b) {
  if (StringIsEmpty(ski_a) || StringIsEmpty(ski_b)) {
    return false;
  }

  return strcmp(ski_a, ski_b) == 0;
}

static bool ShipNodeFindService(ShipNode* self, MdnsEntry* found_entry) {
  if (self->cancel) {
    return false;
  }

  size_t size = VectorGetSize(self->mdns_entries);
  if (size == 0) {
    return false;
  }

  bool entry_found = false;
  MdnsEntry* entry = NULL;

  // Search for the service with the remote ski
  for (size_t i = 0; i < size; i++) {
    entry = (MdnsEntry*)VectorGetElement(self->mdns_entries, i);
    if (SkiMatches(entry->ski, self->remote_ski)) {
      *found_entry = *entry;
      entry_found  = true;
      break;
    }
  }

  return entry_found;
}

static void ShipNodeConnectToService(ShipNode* self, const MdnsEntry* found_entry) {
  if (self->connection_attempt_running) {
    return;
  }

  // Outbound cooldown: after any connection close we skip the first N mDNS
  // cycles to avoid hammering the remote device (which resets its own inbound
  // reconnect timer on each incoming attempt).
  if (self->outbound_skip_remaining > 0) {
    --self->outbound_skip_remaining;
    return;
  }

  size_t len = strlen(found_entry->host);
  if (len <= 1) {
    return;
  }

  if (found_entry->host[len - 1] == '.') {
    --len;
  }

  const char* const uri
      = StringFmtSprintf("wss://%.*s:%d%s", len, found_entry->host, found_entry->port, found_entry->path);
  if (uri == NULL) {
    return;
  }

  self->websocket_creator = WebsocketClientCreatorCreate(uri, self->tsl_certificate, self->remote_ski);
  StringDelete((char*)uri);
  if (self->websocket_creator == NULL) {
    return;
  }

  self->ship_connection = ShipConnectionCreate(
      INFO_PROVIDER_OBJECT(self),
      kShipRoleClient,
      self->local_service_details->ship_id,
      found_entry->ski,
      ""
  );

  if (self->ship_connection != NULL) {
    /* Save outbound pointer locally — self->ship_connection may be overwritten
     * by ShipNodeOnWebsocketServerConnectionCallback during the blocking
     * SHIP_CONNECTION_START call (TCP+TLS+WS connect can take 50-200 ms). */
    ShipConnectionObject* const outbound_sc = self->ship_connection;
    const EebusError err = SHIP_CONNECTION_START(outbound_sc, self->websocket_creator);

    if (err != kEebusErrorOk) {
      /* Outbound TCP/TLS/WS failed.  Only clear ship_connection if we still own it
       * (the HTTP callback may have already replaced it with an inbound). */
      if (self->ship_connection == outbound_sc) {
        self->ship_connection = NULL;
      }
      ShipConnectionDelete(outbound_sc);
    } else if (self->ship_connection != outbound_sc) {
      /* HTTP callback accepted an inbound while our outbound TCP was completing.
       * Apply SKI tie-breaking: keep the connection from the device with the higher SKI. */
      const char* local_ski = ServiceDetailsGetSki(self->local_service_details);
      bool local_ski_higher = (local_ski != NULL && self->remote_ski != NULL &&
                               strcmp(local_ski, self->remote_ski) > 0);
      if (local_ski_higher) {
        /* We have higher SKI: keep our outbound, discard the inbound. */
        SHIP_NODE_DEBUG_PRINTF("%s(), Tie-break outbound: localSKI > remoteSKI — keeping outbound\n", __func__);
        if (self->cancel_ship_connection == NULL) {
          self->cancel_ship_connection = self->ship_connection;
        } else {
          SHIP_CONNECTION_STOP(self->ship_connection);
          ShipConnectionDelete(self->ship_connection);
        }
        self->ship_connection = outbound_sc;
        /* connection_attempt_running stays true (set by the HTTP callback). */
      } else {
        /* Remote has higher SKI: keep the inbound, orphan our outbound. */
        SHIP_NODE_DEBUG_PRINTF("%s(), Tie-break outbound: remoteSKI > localSKI — keeping inbound\n", __func__);
        if (self->cancel_ship_connection == NULL) {
          self->cancel_ship_connection = outbound_sc;
        } else {
          SHIP_CONNECTION_STOP(outbound_sc);
          ShipConnectionDelete(outbound_sc);
        }
        /* connection_attempt_running already set true by the HTTP callback. */
      }
    } else {
      /* Normal case: outbound connected and is still the current connection. */
      self->connection_attempt_running = true;
    }
  }

  WebsocketCreatorDelete(self->websocket_creator);
  self->websocket_creator = NULL;
}

static void ShipNodeConnectToRemoteSki(ShipNode* self) {
  MdnsEntry found_entry;
  EEBUS_MUTEX_LOCK(self->mutex);
  if (ShipNodeFindService(self, &found_entry)) {
    ShipNodeConnectToService(self, &found_entry);
  }

  self->search_for_remote_ski = false;
  EEBUS_MUTEX_UNLOCK(self->mutex);
}

void* ShipNodeConnectionLoop(void* ctx) {
  ShipNode* const sn             = (ShipNode*)ctx;
  ShipNodeQueueMessage queue_msg = {0};
  EebusError err                 = kEebusErrorOk;

  while (!sn->cancel) {
    err = EEBUS_QUEUE_RECEIVE(sn->msg_queue, &queue_msg, kTimeoutInfinite);
    if (err != kEebusErrorOk) {
      continue;
    }

    if (queue_msg.type == kShipNodeQueueMsgTypeMdnsEntriesFound) {
      ShipNodeConnectToRemoteSki(sn);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipConnectionClosed) {
      CloseShipConnection(sn, queue_msg.ship_connection, queue_msg.had_error);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipUnregisterSki) {
      ShipNodeUnregisterSki(SHIP_NODE_OBJECT(sn), queue_msg.ski);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipRegisterSki) {
      ShipNodeRegisterSki(SHIP_NODE_OBJECT(sn), queue_msg.ski, true);
      /* Do NOT call ShipNodeConnectToRemoteSki here: the remote device (K40RF)
       * is the preferred connection initiator in EEBus "auto" role.  Immediately
       * starting an outbound attempt at registration time races with the inbound
       * from the remote device and causes kSmeStateError (state=39) on both sides.
       * The mDNS-triggered path (kShipNodeQueueMsgTypeMdnsEntriesFound) will still
       * start an outbound if the remote has not connected inbound first. */
    }
    ShipNodeQueueMsgDeallocator(&queue_msg);
  }

  return NULL;
}

int ShipNodeOnWebsocketServerConnectionCallback(const char* ski, WebsocketCreatorObject* websocket_creator, void* ctx) {
  ShipNode* const sn = (ShipNode*)ctx;

  if (sn->cancel) {
    return -1;
  }

  /* Check the SKI BEFORE connection_attempt_running so we can handle the
   * startup race: both sides discover each other simultaneously via mDNS and
   * both try to connect.  If our trusted partner arrives inbound while our
   * outbound attempt is in progress, we prefer the inbound (the remote device
   * is typically the EEBus client/initiator) and cancel the outbound. */
  EEBUS_MUTEX_LOCK(sn->mutex);
  bool is_ski_trusted = SkiMatches(ski, sn->remote_ski);
  if (!is_ski_trusted) {
    /* Pairing mode: ask the service whether trust is permitted for this SKI.
     * Called regardless of whether a remote SKI is already registered, so that
     * runtime pairing-mode activation (EEBUS_SERVICE_SET_PAIRING_POSSIBLE) can
     * re-pair a new device without a full reboot. */
    if (SHIP_NODE_READER_IS_WAITING_FOR_TRUST_ALLOWED(sn->ship_node_reader, ski)) {
      StringDelete(sn->remote_ski);
      sn->remote_ski = StringCopy(ski);
      is_ski_trusted = (sn->remote_ski != NULL);
      SHIP_NODE_DEBUG_PRINTF("%s(), Pairing: auto-trusting incoming SKI %s\n", __func__, ski);
    }
  }
  EEBUS_MUTEX_UNLOCK(sn->mutex);

  if (!is_ski_trusted) {
    SHIP_NODE_DEBUG_PRINTF("%s(), Remote SKI is not trusted\n", __func__);
    return -1;
  }

  /* Trusted inbound while an outbound attempt is running: cancel the outbound
   * and accept the inbound.  The cancelled connection will be freed by
   * CloseShipConnection when its teardown callback fires via cancel_ship_connection. */
  if (sn->connection_attempt_running && sn->ship_connection != NULL) {
    SHIP_NODE_DEBUG_PRINTF(
        "%s(), Trusted inbound from %s during outbound — cancelling outbound\n", __func__, ski);
    /* Do NOT call SHIP_CONNECTION_STOP here — it calls EEBUS_THREAD_JOIN which
     * blocks the HTTP server thread and prevents K40RF's inbound from proceeding.
     * The outbound closes naturally when K40RF rejects it; CloseShipConnection
     * frees it when its teardown callback fires via cancel_ship_connection. */
    sn->cancel_ship_connection   = sn->ship_connection;
    sn->ship_connection          = NULL;
    sn->connection_attempt_running = false;
  }

  if (sn->connection_attempt_running) {
    return -1;
  }

  sn->ship_connection
      = ShipConnectionCreate(INFO_PROVIDER_OBJECT(sn), kShipRoleServer, sn->local_service_details->ship_id, ski, "");
  if (sn->ship_connection == NULL) {
    SHIP_NODE_DEBUG_PRINTF("%s(), creating ship connection failed\n", __func__);
    return -1;
  }

  sn->connection_attempt_running = true;
  SHIP_CONNECTION_START(sn->ship_connection, websocket_creator);
  return 0;
}

bool ShipNodeIsClientSupported(ShipNode* self) {
  return (self->role == kShipRoleClient) || (self->role == kShipRoleAuto);
}

bool ShipNodeIsServerSupported(ShipNode* self) {
  return (self->role == kShipRoleServer) || (self->role == kShipRoleAuto);
}

void Start(ShipNodeObject* self) {
  ShipNode* const sn = SHIP_NODE(self);

  if (ShipNodeIsServerSupported(sn)) {
    HTTP_SERVER_START(sn->http_server);
  }

  SHIP_MDNS_START(sn->mdns);

  sn->connection_thread = EebusThreadCreate(ShipNodeConnectionLoop, sn, 8 * 1024);
  if (sn->connection_thread == NULL) {
    SHIP_NODE_DEBUG_PRINTF("%s(), client connection thread creation failed\n", __func__);
  }
}

void Stop(ShipNodeObject* self) {
  ShipNode* const sn = SHIP_NODE(self);

  sn->cancel = true;

  if (sn->connection_thread != NULL) {
    ShipNodeQueueMessage queue_msg = {.type = kShipNodeQueueMsgTypeCancel, .ski = NULL};
    EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
    EEBUS_THREAD_JOIN(sn->connection_thread);
    EebusThreadDelete(sn->connection_thread);
    sn->connection_thread = NULL;
  }

  SHIP_MDNS_STOP(sn->mdns);

  if (ShipNodeIsServerSupported(sn)) {
    HTTP_SERVER_STOP(sn->http_server);
  }
}

void ShipNodeRegisterSki(ShipNodeObject* self, const char* ski, bool is_trusted) {
  UNUSED(is_trusted);
  ShipNode* const sn = SHIP_NODE(self);

  EEBUS_MUTEX_LOCK(sn->mutex);
  StringDelete(sn->remote_ski);
  sn->remote_ski = StringCopy(ski);
  EEBUS_MUTEX_UNLOCK(sn->mutex);
}

void RegisterRemoteSki(ShipNodeObject* self, const char* ski, bool is_trusted) {
  UNUSED(is_trusted);
  ShipNode* const sn = SHIP_NODE(self);

  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeShipRegisterSki,
      .ship_connection = sn->ship_connection,
      .had_error       = false,
      .ski             = StringCopy(ski),
  };

  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}

void ShipNodeUnregisterSki(ShipNodeObject* self, const char* ski) {
  UNUSED(ski);
  ShipNode* const sn = SHIP_NODE(self);

  EEBUS_MUTEX_LOCK(sn->mutex);
  StringDelete(sn->remote_ski);
  sn->remote_ski = NULL;
  EEBUS_MUTEX_UNLOCK(sn->mutex);

  // TODO: Fix possible situation that ShipConnection Start() is called
  // from another thread at the same time
  if (sn->ship_connection != NULL) {
    CloseShipConnection(sn, sn->ship_connection, false);
  }
}

void UnregisterRemoteSki(ShipNodeObject* self, const char* ski) {
  ShipNode* const sn = SHIP_NODE(self);

  if (!SkiMatches(ski, sn->remote_ski)) {
    SHIP_NODE_DEBUG_PRINTF("%s(), SKI does not match\n", __func__);
    return;
  }

  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeShipUnregisterSki,
      .ship_connection = sn->ship_connection,
      .had_error       = false,
      .ski             = StringCopy(ski),
  };

  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}

void CancelPairingWithSki(ShipNodeObject* self, const char* ski) {
  UNUSED(self);
  UNUSED(ski);
  // TODO: Implement method
}

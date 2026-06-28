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
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

#include <string_view>

#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/string_util.h"
#include "src/ship/ship_connection/ship_connection_internal.h"
#include "tests/src/json.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/mocks/common/eebus_timer/eebus_timer_mock.h"

#define TEST_SHIP_URI "wss://DESKTOP-IAKQS71.local:4769"
#define LOCAL_SHIP_ID "LocalShipID"
#define REMOTE_SHIP_ID "RemoteShipID"
#define DEFAULT_CLOSE_CODE 4001

using testing::_;

EebusError ShipConnectionTestSuite::MessageBufferInitHelper(MessageBuffer* msg_buf, const std::string_view& msg) {
  std::unique_ptr<char[], decltype(&JsonFree)> msg_unformatted{JsonUnformat(msg), JsonFree};
  if (msg_unformatted == nullptr) {
    return kEebusErrorInit;
  }

  const size_t msg_unformatted_size = strlen(msg_unformatted.get()) + 1;

  const size_t data_size = sizeof(uint8_t) + msg_unformatted_size;
  uint8_t* const data    = (uint8_t*)malloc(data_size);
  if (data == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  data[0] = kMsgTypeControl;
  memcpy(data + 1, msg_unformatted.get(), msg_unformatted_size);
  msg_buf->data        = data;
  msg_buf->data_size   = data_size;
  msg_buf->deallocator = free;
  return kEebusErrorOk;
}

void ShipConnectionTestSuite::ExpectCloseWithError(const char* error_msg, bool had_error) {
  EXPECT_CALL(*websocket_mock->gmock, Close(sc.websocket, DEFAULT_CLOSE_CODE, testing::StrCaseEq(error_msg)));
  EXPECT_CALL(*ifp_mock->gmock, HandleConnectionClosed(sc.info_provider, SHIP_CONNECTION_OBJECT(&sc), had_error));
}

void ShipConnectionTestSuite::SetUp() {
  tls_cert_mock          = TlsCertificateMockCreate();
  ifp_mock               = InfoProviderMockCreate();
  websocket_creator_mock = WebsocketCreatorMockCreate();

  ShipConnectionConstruct(
      &sc,
      INFO_PROVIDER_OBJECT(ifp_mock),
      kShipRoleClient,
      LOCAL_SHIP_ID,
      TEST_REMOTE_SKI,
      REMOTE_SHIP_ID
  );

  SHIP_CONNECTION_START(SHIP_CONNECTION_OBJECT(&sc), WEBSOCKET_CREATOR_OBJECT(websocket_creator_mock));
  wfr_timer_mock = EEBUS_TIMER_MOCK(sc.wait_for_ready_timer);
  prr_timer_mock = EEBUS_TIMER_MOCK(sc.prolongation_request_reply_timer);
  spr_timer_mock = EEBUS_TIMER_MOCK(sc.send_prolongation_request_timer);
  websocket_mock = WEBSOCKET_MOCK(sc.websocket);
}

void ShipConnectionTestSuite::TearDown() {
  SHIP_CONNECTION_STOP(SHIP_CONNECTION_OBJECT(&sc));

  EXPECT_CALL(*websocket_mock->gmock, Destruct(sc.websocket));
  EXPECT_CALL(*wfr_timer_mock->gmock, Destruct(sc.wait_for_ready_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Destruct(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Destruct(sc.prolongation_request_reply_timer));
  SHIP_CONNECTION_DESTRUCT(&sc);

  EXPECT_CALL(*ifp_mock->gmock, Destruct(INFO_PROVIDER_OBJECT(ifp_mock)));
  INFO_PROVIDER_DESTRUCT(ifp_mock);
  EEBUS_FREE(ifp_mock);

  EXPECT_CALL(*tls_cert_mock->gmock, Destruct(TLS_CERTIFICATE_OBJECT(tls_cert_mock)));
  TLS_CERTIFICATE_DESTRUCT(TLS_CERTIFICATE_OBJECT(tls_cert_mock));
  EEBUS_FREE(tls_cert_mock);

  EXPECT_CALL(*websocket_creator_mock->gmock, Destruct(WEBSOCKET_CREATOR_OBJECT(websocket_creator_mock)));
  WEBSOCKET_CREATOR_DESTRUCT(WEBSOCKET_CREATOR_OBJECT(websocket_creator_mock));
  EEBUS_FREE(websocket_creator_mock);

  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}

void ShipConnectionTestSuite::SetShipConnectionState(SmeState state) {
  EXPECT_CALL(*ifp_mock->gmock, HandleShipStateUpdate(INFO_PROVIDER_OBJECT(ifp_mock), sc.remote_ski, state, _));
  ShipConnectionSetSmeState(&sc, state);
}

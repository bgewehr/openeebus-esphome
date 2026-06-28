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
#include <gtest/gtest.h>

#include <string_view>

#include "src/ship/ship_connection/server.h"
#include "tests/src/json.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;
using testing::_;
using testing::Return;

struct ShipProtHandshakeServerListenChoiceTestInput {
  std::string_view description              = ""sv;
  const char* close_error_msg               = "Error sending ship message";
  ShipConnectionQueueMsgType queue_msg_type = kShipConnectionQueueMsgTypeDataReceived;
  std::string_view msg                      = ""sv;
  SmeState expected_sme_state               = kSmeStateError;
  std::string_view abort_err_msg            = R"({"messageProtocolHandshakeError":[{"error":2}]})";
};

class ShipConnectionProtocolHandshakeServerListenConfirmMessageReceiveTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipProtHandshakeServerListenChoiceTestInput> {};

class ShipConnectionProtocolHandshakeServerListenConfirmMessageContentTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipProtHandshakeServerListenChoiceTestInput> {};

std::ostream& operator<<(std::ostream& os, const ShipProtHandshakeServerListenChoiceTestInput& input) {
  return os << input.description;
}

TEST_P(
    ShipConnectionProtocolHandshakeServerListenConfirmMessageReceiveTests,
    ProtHandshakeServerListenConfirmWrongMessageReceivedTest
) {
  // Arrange:
  // Init message buffer
  ShipConnectionQueueMessage queue_msg;
  const EebusError error = MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg);
  ASSERT_EQ(error, kEebusErrorOk) << "Wrong test input!";

  // Add message to queue
  queue_msg.type = GetParam().queue_msg_type;
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(3);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  const size_t abort_msg_size = GetParam().abort_err_msg.size() + 1;
  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, abort_msg_size)).WillOnce(Return(0));
  EXPECT_CALL(
      *ifp_mock->gmock,
      HandleShipStateUpdate(
          sc.info_provider,
          testing::StrCaseEq(TEST_REMOTE_SKI),
          GetParam().expected_sme_state,
          testing::StrCaseEq("")
      )
  );
  ExpectCloseWithError(GetParam().close_error_msg, false);

  // Act: Check if wrong message format is handled
  SmeProtHandshakeStateServerListenConfirm(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionProtocolHandshakeServerListenConfirmMessageReceiveTests,
    ShipConnectionProtocolHandshakeServerListenConfirmMessageReceiveTests,
    ::testing::Values(
        ShipProtHandshakeServerListenChoiceTestInput{
            .description    = "Timeout Message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeTimeout,
            .msg            = R"({"messageProtocolHandshake": [
                              {"handshakeType": "select"},
                              {"version": [{"major": 1}, {"minor": 0}]},
                              {"formats": [{"format": ["JSON-UTF8"]}]}
                            ]})"sv,
        },
        ShipProtHandshakeServerListenChoiceTestInput{
            .description    = "Cancel message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeCancel,
            .msg            = R"({"messageProtocolHandshake": [
                              {"handshakeType": "select"},
                              {"version": [{"major": 1}, {"minor": 0}]},
                              {"formats": [{"format": ["JSON-UTF8"]}]}
                            ]})"sv,
        },
        ShipProtHandshakeServerListenChoiceTestInput{
            .description = "Wrong message value type received"sv,
            .msg         = R"({"announceMax": [
                              {"version": [{"major": 1}, {"minor": 0}]},
                              {"formats": [{"format": ["JSON-UTF8"]}]}
                           ]})"sv,
        },
        ShipProtHandshakeServerListenChoiceTestInput{
            .description = "No message value received"sv,
            .msg         = R"({"messageProtocolHandshake": null})"sv,
        }
    )
);

TEST_P(
    ShipConnectionProtocolHandshakeServerListenConfirmMessageContentTests,
    ProtHandshakeServerListenConfirmWrongMessageContentTest
) {
  // Arrange:
  // Init message buffer
  ShipConnectionQueueMessage queue_msg;
  const EebusError error = MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg);
  ASSERT_EQ(error, kEebusErrorOk) << "Wrong test input!";

  // Add message to queue
  queue_msg.type = GetParam().queue_msg_type;
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(3);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  if (GetParam().expected_sme_state == kSmeStateError) {
    const size_t abort_msg_size = GetParam().abort_err_msg.size() + 1;
    EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, abort_msg_size)).WillOnce(Return(0));
  }

  EXPECT_CALL(
      *ifp_mock->gmock,
      HandleShipStateUpdate(
          sc.info_provider,
          testing::StrCaseEq(TEST_REMOTE_SKI),
          GetParam().expected_sme_state,
          testing::StrCaseEq("")
      )
  );
  ExpectCloseWithError(GetParam().close_error_msg, false);

  // Act: Check if the message content is handled apropriately
  SmeProtHandshakeStateServerListenConfirm(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionProtocolHandshakeServerListenConfirmMessageContentTests,
    ShipConnectionProtocolHandshakeServerListenConfirmMessageContentTests,
    ::testing::Values(
        ShipProtHandshakeServerListenChoiceTestInput{
            .description = "announceMax handshake type message received"sv,
            .msg         = R"({"messageProtocolHandshake": [
                              {"handshakeType": "announceMax"},
                              {"version": [{"major": 1}, {"minor": 0}]},
                              {"formats": [{"format": ["JSON-UTF8"]}]}
                           ]})"sv,
        },
        ShipProtHandshakeServerListenChoiceTestInput{
            .description        = "Select handshake type message received"sv,
            .close_error_msg    = "",
            .msg                = R"({"messageProtocolHandshake": [
                                  {"handshakeType": "select"},
                                  {"version": [{"major": 1}, {"minor": 0}]},
                                  {"formats": [{"format": ["JSON-UTF8"]}]}
                                ]})"sv,
            .expected_sme_state = kSmeProtHStateServerOk,
        }
    )
);

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

#include "tests/src/json.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;
using testing::_;
using testing::Return;

struct ShipConnectionAccessMethodsInitTestInput {
  std::string_view description              = ""sv;
  const char* close_error_msg               = "";
  ShipConnectionQueueMsgType queue_msg_type = kShipConnectionQueueMsgTypeDataReceived;
  std::string_view msg                      = R"({"accessMethodsRequest": []})"sv;
  bool msg_send_successful                  = false;
  SmeState expected_sme_state               = kSmeStateError;
};

class ShipConnectionAccessMethodsInitTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipConnectionAccessMethodsInitTestInput> {};

std::ostream& operator<<(std::ostream& os, const ShipConnectionAccessMethodsInitTestInput& input) {
  return os << input.description;
}

TEST_P(ShipConnectionAccessMethodsInitTests, ShipConnectionAccessMethodsInitMessageSendTest) {
  // Arrange:
  // Init message bufer
  ShipConnectionQueueMessage queue_msg;
  const EebusError error = MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg);
  ASSERT_EQ(error, kEebusErrorOk) << "Wrong test input!";

  // Add message to queue
  queue_msg.type = GetParam().queue_msg_type;
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  const size_t msg_size      = queue_msg.msg_buf.data_size - 1;
  const size_t ret_num_bytes = GetParam().msg_send_successful ? msg_size : 0;
  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, msg_size))
      .WillOnce(Return(static_cast<int32_t>(ret_num_bytes)));

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  if (GetParam().expected_sme_state != kSmeStateError) {
    EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  } else {
    // State change expected only if error occurs, otherwise still in DataExchange
    EXPECT_CALL(
        *ifp_mock->gmock,
        HandleShipStateUpdate(
            sc.info_provider,
            testing::StrCaseEq(TEST_REMOTE_SKI),
            GetParam().expected_sme_state,
            testing::StrCaseEq("")
        )
    );
  }

  ExpectCloseWithError(GetParam().close_error_msg, false);

  // Act: Request access methods
  SmeHandshakeAccessMethodsInit(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionAccessMethodsInitTests,
    ShipConnectionAccessMethodsInitTests,
    ::testing::Values(
        ShipConnectionAccessMethodsInitTestInput{
            .description     = "Message not sent"sv,
            .close_error_msg = "SmeAccessMethodsRequest: Error sending kSmeConnectionAccessMethodsRequest message",
        },
        ShipConnectionAccessMethodsInitTestInput{
            .description         = "Message successfully sent"sv,
            .msg_send_successful = true,
            .expected_sme_state  = kCmiStateClientSend,
        }
    )
);

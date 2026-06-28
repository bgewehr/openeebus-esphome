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

struct ShipSmeHelloReadyListenTestInput {
  std::string_view description              = ""sv;
  const char* close_error_msg               = "";
  ShipConnectionQueueMsgType queue_msg_type = kShipConnectionQueueMsgTypeDataReceived;
  std::string_view msg                      = R"({"connectionHello": [{"phase": "ready"}, {"waiting": 60000}]})"sv;
  bool msg_send_successful                  = false;
  bool common_prolongation_req_send         = false;
  SmeState expected_sme_state               = kSmeHelloStateAbort;
};

class ShipConnectionHelloStateReadyListenAbortTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipSmeHelloReadyListenTestInput> {};

class ShipConnectionHelloStateReadyListenEvaluateReadyAndAbortPhaseMessageTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipSmeHelloReadyListenTestInput> {};

class ShipConnectionHelloStateReadyListenEvaluatePendingPhaseMessageTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipSmeHelloReadyListenTestInput> {};

std::ostream& operator<<(std::ostream& os, const ShipSmeHelloReadyListenTestInput& input) {
  return os << input.description;
}

TEST_P(ShipConnectionHelloStateReadyListenAbortTests, SmeHelloReadyListenAbortTest) {
  // Arrange:
  // Init message buffer
  ShipConnectionQueueMessage queue_msg;
  const EebusError error = MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg);
  ASSERT_EQ(error, kEebusErrorOk) << "Wrong test input!";

  // Add message to queue
  queue_msg.type = GetParam().queue_msg_type;
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
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

  // Act: Listen and process the received message
  SmeHelloStateReadyListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionHelloStateReadyListenAbortTests,
    ShipConnectionHelloStateReadyListenAbortTests,
    ::testing::Values(
        ShipSmeHelloReadyListenTestInput{
            .description        = "Timeout message received"sv,
            .queue_msg_type     = kShipConnectionQueueMsgTypeTimeout,
            .expected_sme_state = kSmeHelloStateReadyTimeout,
        },
        ShipSmeHelloReadyListenTestInput{
            .description    = "Cancel message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeCancel,
        }
    )
);

TEST_P(
    ShipConnectionHelloStateReadyListenEvaluateReadyAndAbortPhaseMessageTests,
    SmeHelloReadyListenEvaluateReadyAndAbortMessageTest
) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf  = {0};
  const EebusError error = MessageBufferInitHelper(&msg_buf, GetParam().msg);
  ASSERT_EQ(error, kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  // Calculate message size
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

  // Act: Receive and process the hello message
  SmeHelloStateReadyListen(&sc);

  // Assert: Verify that the state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionHelloStateReadyListenEvaluateReadyAndAbortPhaseMessageTests,
    ShipConnectionHelloStateReadyListenEvaluateReadyAndAbortPhaseMessageTests,
    ::testing::Values(
        ShipSmeHelloReadyListenTestInput{
            .description        = "Ready phase message received"sv,
            .expected_sme_state = kSmeHelloStateOk,
        },
        ShipSmeHelloReadyListenTestInput{
            .description = "Abort phase message received"sv,
            .msg         = R"({"connectionHello": [{"phase": "aborted"}]})"sv,
        }
    )
);

TEST_P(
    ShipConnectionHelloStateReadyListenEvaluatePendingPhaseMessageTests,
    SmeHelloReadyListenEvaluatePendingMessageTest
) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf  = {0};
  const EebusError error = MessageBufferInitHelper(&msg_buf, GetParam().msg);
  ASSERT_EQ(error, kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Set initial SME state
  if (GetParam().expected_sme_state == kSmeHelloStateAbort) {
    EXPECT_CALL(*ifp_mock->gmock, HandleShipStateUpdate(sc.info_provider, _, kSmeHelloStateAbort, _));
  }

  SetShipConnectionState(kSmeHelloStateReadyListen);

  // Expect function calls
  if (GetParam().common_prolongation_req_send) {
    EXPECT_CALL(*wfr_timer_mock->gmock, GetTimerState(sc.wait_for_ready_timer));
    EXPECT_CALL(*wfr_timer_mock->gmock, GetRemainingTime(sc.wait_for_ready_timer));

    std::string_view remaining_time_msg = R"({"connectionHello":[{"phase":"pending"}]})"sv;

    const size_t msg_size      = remaining_time_msg.size() + 1;
    const size_t ret_num_bytes = GetParam().msg_send_successful ? msg_size : 0;
    EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, msg_size))
        .WillOnce(Return(static_cast<int32_t>(ret_num_bytes)));
    EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false)).Times(2);
  } else {
    EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  }

  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  ExpectCloseWithError(GetParam().close_error_msg, false);

  // Act: Receive and process the hello message
  SmeHelloStateReadyListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionHelloStateReadyListenEvaluatePendingPhaseMessageTests,
    ShipConnectionHelloStateReadyListenEvaluatePendingPhaseMessageTests,
    ::testing::Values(
        ShipSmeHelloReadyListenTestInput{
            .description         = "Pending phase without prolongation request"sv,
            .msg                 = R"({"connectionHello": [{"phase": "pending"}]})"sv,
            .msg_send_successful = true,
            .expected_sme_state  = kSmeHelloStateReadyListen,
        },
        ShipSmeHelloReadyListenTestInput{
            .description         = "Pending phase no prolongation request"sv,
            .msg                 = R"({"connectionHello": [{"phase": "pending"}, {"prolongationRequest": false}]})"sv,
            .msg_send_successful = true,
            .expected_sme_state  = kSmeHelloStateReadyListen,
        },
        ShipSmeHelloReadyListenTestInput{
            .description         = "Pending phase with prolongation request, remaining time msg sent"sv,
            .msg                 = R"({"connectionHello": [{"phase": "pending"}, {"prolongationRequest": true}]})"sv,
            .msg_send_successful = true,
            .common_prolongation_req_send = true,
            .expected_sme_state           = kSmeHelloStateReadyListen,
        },
        ShipSmeHelloReadyListenTestInput{
            .description         = "Pending phase with prolongation request, remaining time msg not sent"sv,
            .msg                 = R"({"connectionHello": [{"phase": "pending"}, {"prolongationRequest": true}]})"sv,
            .msg_send_successful = false,
            .common_prolongation_req_send = true,
            .expected_sme_state           = kSmeHelloStateAbort,
        }
    )
);

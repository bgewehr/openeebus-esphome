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

struct ShipSmeHelloPendingListenTestInput {
  std::string_view description              = ""sv;
  const char* close_error_msg               = "";
  ShipConnectionQueueMsgType queue_msg_type = kShipConnectionQueueMsgTypeDataReceived;
  std::string_view msg                      = R"({"connectionHello": [{"phase": "ready"}, {"waiting": 60000}]})"sv;
  bool msg_send_successful                  = false;
  bool no_waiting                           = false;
  bool no_prolong_req                       = false;
  bool waiting_gt_prolong_inc               = false;
  bool waiting_lt_prolong_inc               = false;
  SmeState expected_sme_state               = kSmeHelloStateAbort;
};

class ShipConnectionHelloStatePendingListenAbortTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipSmeHelloPendingListenTestInput> {};

class ShipConnectionHelloStatePendingListenEvaluateMessageReadyPhaseTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipSmeHelloPendingListenTestInput> {};

class ShipConnectionHelloStatePendingListenEvaluateMessagePendingPhaseTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipSmeHelloPendingListenTestInput> {};

std::ostream& operator<<(std::ostream& os, const ShipSmeHelloPendingListenTestInput& input) {
  return os << input.description;
}

TEST_P(ShipConnectionHelloStatePendingListenAbortTests, SmeHelloPendingListenAbortTest) {
  // Arrange:
  // Init message buffer
  ShipConnectionQueueMessage queue_msg;
  const EebusError error = MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg);
  ASSERT_EQ(error, kEebusErrorOk) << "Wrong test input!";

  // Add message to queue
  queue_msg.type = GetParam().queue_msg_type;
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
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
  SmeHelloStatePendingListen(&sc);

  // Assert: Verify that the state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionHelloStatePendingListenAbortTests,
    ShipConnectionHelloStatePendingListenAbortTests,
    ::testing::Values(
        ShipSmeHelloPendingListenTestInput{
            .description        = "Timeout message received"sv,
            .queue_msg_type     = kShipConnectionQueueMsgTypeTimeout,
            .expected_sme_state = kSmeHelloStatePendingTimeout,
        },
        ShipSmeHelloPendingListenTestInput{
            .description    = "Cancel message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeCancel,
        }
    )
);

TEST_P(
    ShipConnectionHelloStatePendingListenEvaluateMessageReadyPhaseTests,
    SmeHelloPendingListenEvalueateMessageReadyPhaseTest
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

  SetShipConnectionState(kSmeHelloStatePendingListen);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));

  if (GetParam().no_waiting) {
    EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
    EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
    EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  }

  if (GetParam().waiting_gt_prolong_inc) {
    EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(3);
    EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
    EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
    EXPECT_CALL(*spr_timer_mock->gmock, Start(sc.send_prolongation_request_timer, 45000, false));
  }

  if (GetParam().waiting_lt_prolong_inc) {
    EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(3);
    EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
    EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer)).Times(2);
  }

  ExpectCloseWithError(GetParam().close_error_msg, false);

  // Act: Receive and process the hello message
  SmeHelloStatePendingListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionHelloStatePendingListenEvaluateMessageReadyPhaseTests,
    ShipConnectionHelloStatePendingListenEvaluateMessageReadyPhaseTests,
    ::testing::Values(
        ShipSmeHelloPendingListenTestInput{
            .description        = "Ready phase message received, no waiting period"sv,
            .msg                = R"({"connectionHello": [{"phase": "ready"}]})"sv,
            .no_waiting         = true,
            .expected_sme_state = kSmeHelloStateAbort,
        },
        ShipSmeHelloPendingListenTestInput{
            .description            = "phase = ready, waiting > tHelloProlongThrInc"sv,
            .msg                    = R"({"connectionHello": [{"phase": "ready"}, {"waiting": 60000}]})"sv,
            .waiting_gt_prolong_inc = true,
            .expected_sme_state     = kSmeHelloStatePendingListen,
        },
        ShipSmeHelloPendingListenTestInput{
            .description            = "phase = ready, waiting < tHelloProlongThrInc"sv,
            .msg                    = R"({"connectionHello": [{"phase": "ready"}, {"waiting": 1000}]})"sv,
            .waiting_lt_prolong_inc = true,
            .expected_sme_state     = kSmeHelloStatePendingListen,
        }
    )
);

TEST_P(
    ShipConnectionHelloStatePendingListenEvaluateMessagePendingPhaseTests,
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

  SetShipConnectionState(kSmeHelloStatePendingListen);

  // Expect function calls
  if (GetParam().no_waiting) {
    EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
    EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
    EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));

    if (GetParam().no_prolong_req) {
      EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
    } else {
      EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false)).Times(2);
      EXPECT_CALL(*wfr_timer_mock->gmock, GetTimerState(sc.wait_for_ready_timer));
      EXPECT_CALL(*wfr_timer_mock->gmock, GetRemainingTime(sc.wait_for_ready_timer));

      // Unformat response message
      std::string_view sent_msg = R"({"connectionHello": [{"phase": "pending"}]})"sv;
      std::unique_ptr<char[], decltype(&JsonFree)> sa(JsonUnformat(sent_msg), JsonFree);
      ASSERT_NE(sa, nullptr) << "Wrong test input!";

      const size_t msg_size      = strlen(sa.get()) + 1;
      const size_t ret_num_bytes = GetParam().msg_send_successful ? msg_size : 0;
      EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, msg_size))
          .WillOnce(Return(static_cast<int32_t>(ret_num_bytes)));
    }
  } else {
    EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  }

  if (GetParam().waiting_gt_prolong_inc) {
    EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
    EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
    EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
    EXPECT_CALL(*spr_timer_mock->gmock, Start(sc.send_prolongation_request_timer, 45000, false));
  }

  if (GetParam().waiting_lt_prolong_inc) {
    EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
    EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
    EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer)).Times(2);
  }

  ExpectCloseWithError(GetParam().close_error_msg, false);

  // Act: Receive and process the hello message
  SmeHelloStatePendingListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionHelloStatePendingListenEvaluateMessagePendingPhaseTests,
    ShipConnectionHelloStatePendingListenEvaluateMessagePendingPhaseTests,
    ::testing::Values(
        ShipSmeHelloPendingListenTestInput{
            .description        = "Pending phase, waiting == NULL, prolongationRequest == NULL, msg sent"sv,
            .msg                = R"({"connectionHello": [{"phase": "pending"}]})"sv,
            .no_waiting         = true,
            .no_prolong_req     = true,
            .expected_sme_state = kSmeHelloStatePendingListen,
        },
        ShipSmeHelloPendingListenTestInput{
            .description         = "Pending phase, prolongationRequest != 0, msg not sent"sv,
            .msg                 = R"({"connectionHello": [
                                      {"phase": "pending"}, {"prolongationRequest": true}]})"sv,
            .msg_send_successful = false,
            .no_waiting          = true,
            .no_prolong_req      = false,
            .expected_sme_state  = kSmeHelloStateAbort,
        },
        ShipSmeHelloPendingListenTestInput{
            .description         = "Pending phase, prolongationRequest != 0, msg sent"sv,
            .msg                 = R"({"connectionHello": [
                                      {"phase": "pending"}, {"prolongationRequest": true}]})"sv,
            .msg_send_successful = true,
            .no_waiting          = true,
            .no_prolong_req      = false,
            .expected_sme_state  = kSmeHelloStatePendingListen,
        },
        ShipSmeHelloPendingListenTestInput{
            .description            = "Pending phase, waiting > tHelloProlongThrInc"sv,
            .msg                    = R"({"connectionHello": [{"phase": "pending"}, {"waiting": 60000}]})"sv,
            .waiting_gt_prolong_inc = true,
            .expected_sme_state     = kSmeHelloStatePendingListen,
        },
        ShipSmeHelloPendingListenTestInput{
            .description            = "Pending phase, waiting < tHelloProlongThrInc"sv,
            .msg                    = R"({"connectionHello": [{"phase": "pending"}, {"waiting": 1000}]})"sv,
            .waiting_lt_prolong_inc = true,
            .expected_sme_state     = kSmeHelloStatePendingListen,
        }
    )
);

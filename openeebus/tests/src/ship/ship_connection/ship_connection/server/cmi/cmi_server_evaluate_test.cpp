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

#include "src/common/array_util.h"
#include "src/ship/ship_connection/server.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;
using testing::_;
using testing::Return;

struct ShipCmiServerEvaluateInitMessageTestInput {
  std::string_view description     = ""sv;
  const char* close_error_msg      = "Invalid init message received";
  bool use_correct_msg_size        = true;
  uint8_t init_msg_values[2]       = {kMsgTypeInit, 0x00};
  SmeState expected_sme_state      = kSmeStateError;
};

class ShipConnectionServerCmiStateEvaluateInitMessageTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipCmiServerEvaluateInitMessageTestInput> {};

class ShipConnectionServerCmiStateEvaluateNoInitMessageProvidedTests : public ShipConnectionTestSuite {};

std::ostream& operator<<(std::ostream& os, const ShipCmiServerEvaluateInitMessageTestInput& input) {
  return os << input.description;
}

TEST_F(ShipConnectionServerCmiStateEvaluateNoInitMessageProvidedTests, CmiStateServerEvaluateNoMessageProvidedTest) {
  // Arrange:
  // Create message buffer
  const size_t msg_size = ARRAY_SIZE(kShipInitMessage);
  MessageBufferInit(&sc.msg, NULL, msg_size);

  // Expect message abort function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(
      *ifp_mock->gmock,
      HandleShipStateUpdate(
          sc.info_provider,
          testing::StrCaseEq(TEST_REMOTE_SKI),
          kSmeStateError,
          testing::StrCaseEq("")
      )
  );
  ExpectCloseWithError("Invalid init message received", false);

  // Act: Evaluate message
  CmiStateServerEvaluate(&sc);

  // Assert: SME state expected to be error and message buffer is released
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeStateError);
  EXPECT_EQ(sc.msg.data, nullptr);
  EXPECT_EQ(sc.msg.data_size, 0);
}

TEST_P(ShipConnectionServerCmiStateEvaluateInitMessageTests, CmiStateServerEvaluateInitMessage) {
  // Arrange: Init message buffer and expect function calls
  const size_t msg_size     = ARRAY_SIZE(kShipInitMessage);
  const size_t msg_buf_size = GetParam().use_correct_msg_size ? msg_size : 0;

  uint8_t* const msg = reinterpret_cast<uint8_t*>(malloc(msg_buf_size));
  memcpy(msg, GetParam().init_msg_values, msg_buf_size);
  MessageBufferInitWithDeallocator(&sc.msg, msg, msg_buf_size, free);

  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));

  if (GetParam().expected_sme_state != kSmeStateError) {
    // Message is sent, expect call
    EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, msg_size))
        .WillOnce(Return(static_cast<int32_t>(msg_size)));
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

  // Act: Evaluate message
  CmiStateServerEvaluate(&sc);

  // Assert: SME state changed accordingly and message buffer is released
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
  EXPECT_EQ(sc.msg.data, nullptr);
  EXPECT_EQ(sc.msg.data_size, 0);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionServerCmiStateEvaluateInitMessageTests,
    ShipConnectionServerCmiStateEvaluateInitMessageTests,
    ::testing::Values(
        ShipCmiServerEvaluateInitMessageTestInput{
            .description        = "Init message correctly evaluated"sv,
            .close_error_msg    = "",
            .expected_sme_state = kSmeHelloState,
},
        ShipCmiServerEvaluateInitMessageTestInput{
            .description          = "Wrong init message size"sv,
            .use_correct_msg_size = false,
        },
        ShipCmiServerEvaluateInitMessageTestInput{
            .description     = "Wrong init message type"sv,
            .init_msg_values = {kMsgTypeControl, 0x00},
        },
        ShipCmiServerEvaluateInitMessageTestInput{
            .description     = "Wrong init message data"sv,
            .init_msg_values = {kMsgTypeInit, 0x01},
        }
    )
);

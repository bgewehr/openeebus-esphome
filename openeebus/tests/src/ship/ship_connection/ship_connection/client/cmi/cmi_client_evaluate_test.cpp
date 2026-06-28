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
#include "src/common/eebus_malloc.h"
#include "src/ship/ship_connection/client.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;

struct ShipCmiClientEvaluateInitMessageTestInput {
  std::string_view description = ""sv;
  const char* close_error_msg  = "Invalid init message received";
  size_t message_size          = ARRAY_SIZE(kShipInitMessage);
  uint8_t init_msg_values[2]   = {kMsgTypeInit, 0x00};
  SmeState expected_sme_state  = kSmeStateError;
};

class ShipConnectionCmiStateEvaluateInitMessageTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipCmiClientEvaluateInitMessageTestInput> {};

class ShipConnectionCmiStateEvaluateInitMessageSpecialTests : public ShipConnectionTestSuite {};

std::ostream& operator<<(std::ostream& os, const ShipCmiClientEvaluateInitMessageTestInput& input) {
  return os << input.description;
}

TEST_F(ShipConnectionCmiStateEvaluateInitMessageSpecialTests, CmiStateEvaluateNoMessageProvidedTest) {
  // Arrange: Init message buffer
  MessageBufferInit(&sc.msg, NULL, ARRAY_SIZE(kShipInitMessage));

  // Expect connection close function calls
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
  CmiStateClientEvaluate(&sc);

  // Assert: SME state changed accordingly and message buffer is released
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeStateError);
  EXPECT_EQ(sc.msg.data, nullptr);
  EXPECT_EQ(sc.msg.data_size, 0);
}

TEST_P(ShipConnectionCmiStateEvaluateInitMessageTests, CmiStateEvaluateInitMessageTest) {
  // Arrange:
  // Create message and init message buffer
  const size_t msg_size = GetParam().message_size;
  uint8_t* const msg    = reinterpret_cast<uint8_t*>(EEBUS_MALLOC(msg_size));
  memcpy(msg, GetParam().init_msg_values, msg_size);
  MessageBufferInit(&sc.msg, msg, msg_size);

  // Expect timer stop function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
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

  // Act: Evaluate message
  CmiStateClientEvaluate(&sc);

  // Assert: SME state changed accordingly and message buffer is released
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
  EXPECT_EQ(sc.msg.data, nullptr);
  EXPECT_EQ(sc.msg.data_size, 0);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionCmiStateEvaluateInitMessageTests,
    ShipConnectionCmiStateEvaluateInitMessageTests,
    ::testing::Values(
        ShipCmiClientEvaluateInitMessageTestInput{
            .description        = "Init message correctly evaluated"sv,
            .close_error_msg    = "",
            .expected_sme_state = kSmeHelloState,
},
        ShipCmiClientEvaluateInitMessageTestInput{
            .description  = "Wrong init message size"sv,
            .message_size = 0,
        },
        ShipCmiClientEvaluateInitMessageTestInput{
            .description     = "Wrong init message type"sv,
            .init_msg_values = {kMsgTypeControl, 0x00},
        },
        ShipCmiClientEvaluateInitMessageTestInput{
            .description     = "Wrong init message data"sv,
            .init_msg_values = {kMsgTypeInit, 0x01},
        }
    )
);

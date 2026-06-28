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
#include "src/ship/ship_connection/client.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;
using testing::_;
using testing::Return;

struct ShipCmiSendInitMessageTestInput {
  std::string_view description = ""sv;
  const char* close_error_msg  = "";
  SmeState expected_sme_state  = kSmeStateError;
};

class ShipConnectionCmiStateSendInitMessageTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ShipCmiSendInitMessageTestInput> {};

std::ostream& operator<<(std::ostream& os, const ShipCmiSendInitMessageTestInput& input) {
  return os << input.description;
}

TEST_P(ShipConnectionCmiStateSendInitMessageTests, CmiClientInitMessageSendTest) {
  // Arrange: Calculate message size and expect function calls
  const size_t message_size = ARRAY_SIZE(kShipInitMessage);
  size_t ret_num_bytes      = message_size;

  // Expect timer stop function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));

  if (GetParam().expected_sme_state == kSmeStateError) {
    ret_num_bytes = 0;
  }

  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, message_size))
      .WillOnce(Return(static_cast<int32_t>(ret_num_bytes)));
  EXPECT_CALL(*ifp_mock->gmock, HandleShipStateUpdate(sc.info_provider, testing::StrCaseEq(TEST_REMOTE_SKI),
                                    GetParam().expected_sme_state, testing::StrCaseEq("")));
  ExpectCloseWithError(GetParam().close_error_msg, false);

  // Act: Send init message
  CmiStateClientSend(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionCmiStateSendInitMessageTests,
    ShipConnectionCmiStateSendInitMessageTests,
    ::testing::Values(
        ShipCmiSendInitMessageTestInput{
            .description        = "Init message successfully sent"sv,
            .expected_sme_state = kCmiStateClientWait,
        },
        ShipCmiSendInitMessageTestInput{
            .description     = "Init message failed to send"sv,
            .close_error_msg = "CMI client send failed",
        }
    )
);

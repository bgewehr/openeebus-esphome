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

struct ShipSmeHelloAbortTestInput {
  std::string_view description     = ""sv;
  const char* close_error_msg      = "SME Hello state connection aborted";
  std::string_view msg             = R"({"connectionHello": [{"phase": "aborted"}]})"sv;
  bool msg_send_successful         = false;
};

class ShipConnectionHelloStateAbortTests : public ShipConnectionTestSuite,
                                           public ::testing::WithParamInterface<ShipSmeHelloAbortTestInput> {};

std::ostream& operator<<(std::ostream& os, const ShipSmeHelloAbortTestInput& input) {
  return os << input.description;
}

TEST_P(ShipConnectionHelloStateAbortTests, SmeHelloAbortTest) {
  // Arrange:
  // Unformat JSON message
  std::unique_ptr<char[], decltype(&JsonFree)> s(JsonUnformat(GetParam().msg), JsonFree);
  ASSERT_NE(s, nullptr) << "Wrong test input!";

  // Set initial state and calculate message size
  SetShipConnectionState(kSmeHelloStateAbort);
  const size_t msg_size      = strlen(s.get()) + 1;
  const size_t ret_num_bytes = GetParam().msg_send_successful ? msg_size : 0;
  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, msg_size))
      .WillOnce(Return(static_cast<int32_t>(ret_num_bytes)));

  // Expect connection closing function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(
      *ifp_mock->gmock,
      HandleShipStateUpdate(
          sc.info_provider,
          testing::StrCaseEq(TEST_REMOTE_SKI),
          kSmeStateError,
          testing::StrCaseEq("")
      )
  );
  ExpectCloseWithError(GetParam().close_error_msg, false);

  // Act: Abort connection
  SmeHelloStateAbort(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeStateError);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionHelloStateAbortTests,
    ShipConnectionHelloStateAbortTests,
    ::testing::Values(
        ShipSmeHelloAbortTestInput{
            .description         = "Abort message sent successfully"sv,
            .msg_send_successful = true,
        },
        ShipSmeHelloAbortTestInput{
            .description = "Abort message failed to send"sv,
        }
    )
);

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

#include "src/common/string_util.h"
#include "tests/src/json.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;
using testing::_;
using testing::Return;

struct ShipSmePinCheckInitTestInput {
  std::string_view description     = ""sv;
  const char* close_error_msg      = "";
  std::string_view msg             = R"({"connectionPinState": [{"pinState": "none"}]})"sv;
  bool msg_send_successful         = false;
  SmeState expected_sme_state      = kSmeStateError;
};

class ShipConnectionPinCheckInitTests : public ShipConnectionTestSuite,
                                        public ::testing::WithParamInterface<ShipSmePinCheckInitTestInput> {};

std::ostream& operator<<(std::ostream& os, const ShipSmePinCheckInitTestInput& input) {
  return os << input.description;
}

TEST_P(ShipConnectionPinCheckInitTests, SmePinCheckInit) {
  // Arrange:
  // Set initial SME state
  SetShipConnectionState(kSmePinStateCheckInit);
  // Init message buffer
  MessageBuffer msg_buf  = {0};
  const EebusError error = MessageBufferInitHelper(&msg_buf, GetParam().msg);
  ASSERT_EQ(error, kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Calculate message length and expect message to be sent or not
  const size_t msg_size      = msg_buf.data_size - 1;
  const size_t ret_num_bytes = GetParam().msg_send_successful ? msg_size : 0;
  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, msg_size))
      .WillOnce(Return(static_cast<int32_t>(ret_num_bytes)));

  // Expect timer function calls
  if (GetParam().msg_send_successful) {
    EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
    EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
    EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
    EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  } else {
    EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
    EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
    EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
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

  // Act: Receive and send PIN requirement
  SmePinStateCheckInit(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionPinCheckInitTests,
    ShipConnectionPinCheckInitTests,
    ::testing::Values(
        ShipSmePinCheckInitTestInput{
            .description     = "Error sending PIN message"sv,
            .close_error_msg = "Error sending PIN requirement message",
        },
        ShipSmePinCheckInitTestInput{
            .description         = "PIN message successfully sent"sv,
            .msg_send_successful = true,
            .expected_sme_state  = kSmePinStateCheckOk,
        }
    )
);

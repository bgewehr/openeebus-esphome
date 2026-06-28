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
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

TEST_F(ShipConnectionTestSuite, ProtHandshakeServerInitTest) {
  // Arrange: Expect timer stop and connection close function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  EXPECT_CALL(
      *ifp_mock->gmock,
      HandleShipStateUpdate(
          sc.info_provider,
          testing::StrCaseEq(TEST_REMOTE_SKI),
          kSmeProtHStateServerListenProposal,
          testing::StrCaseEq("")
      )
  );
  ExpectCloseWithError("", false);

  // Act: Stop timer and move to next state
  SmeProtHandshakeStateServerInit(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeProtHStateServerListenProposal);
}

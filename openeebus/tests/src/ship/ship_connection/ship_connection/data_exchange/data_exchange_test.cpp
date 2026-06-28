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

#include "mocks/ship/api/data_reader_mock.h"
#include "src/common/json.h"
#include "src/common/string_util.h"
#include "tests/src/json.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;
using testing::_;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

static constexpr std::string_view websocket_received_msg =
    R"({"data":[
      {"header":[
        {"protocolId":"ee1.0"}
      ]},
      {"payload":
        {"datagram":[
          {"header":[
            {"specificationVersion":"1.3.0"},
            {"addressSource":[
              {"device":"d:_i:Demo_EVSE-234567890"},
              {"entity":[0]},
              {"feature":0}
            ]},
            {"addressDestination":[
              {"entity":[0]},
              {"feature":0}
            ]},
            {"msgCounter":1},
            {"cmdClassifier":"read"}
          ]},
          {"payload":[
            {"cmd":[
              [
                {"nodeManagementDetailedDiscoveryData":[]}
              ]
            ]}
          ]}
        ]}
      }
    ]})"sv;

static constexpr std::string_view spine_data_received =
    R"({"datagram":[
      {"header":[
        {"specificationVersion":"1.3.0"},
        {"addressSource":[
          {"device":"d:_i:Demo_EVSE-234567890"},
          {"entity":[0]},
          {"feature":0}
        ]},
        {"addressDestination":[
          {"entity":[0]},
          {"feature":0}
        ]},
        {"msgCounter":1},
        {"cmdClassifier":"read"}
      ]},
      {"payload":[
        {"cmd":[
          [
            {"nodeManagementDetailedDiscoveryData":[]}
          ]
        ]}
      ]}
    ]})"sv;

TEST_F(ShipConnectionTestSuite, ShipConnectionDataExchangeReceiveSpineDataTest) {
  // Arrange:
  // Setup data reader
  std::unique_ptr<DataReaderMock, decltype(&DataReaderMockDelete)> data_reader_mock(
      DataReaderMockCreate(),
      DataReaderMockDelete
  );
  sc.data_reader = DATA_READER_OBJECT(data_reader_mock.get());

  // Check only data exchange handling
  sc.is_access_methods_req_sent = true;

  // Set initial SME state
  SetShipConnectionState(kDataExchange);

  // Unformat JSON message
  std::unique_ptr<char[], decltype(&JsonFree)> received_json(JsonUnformat(websocket_received_msg), JsonFree);
  ASSERT_NE(received_json, nullptr) << "Wrong test input!";

  std::unique_ptr<char[], decltype(&StringDelete)> received_msg(
      const_cast<char*>(StringFmtSprintf("\002%s", received_json.get())),
      StringDelete
  );

  // Init message bufer
  ShipConnectionQueueMessage queue_msg;
  MessageBufferInitWithDeallocator(
      &queue_msg.msg_buf,
      reinterpret_cast<uint8_t*>(received_msg.get()),
      strlen(received_msg.get()) + 1,
      nullptr
  );

  // Add message to queue
  queue_msg.type = kShipConnectionQueueMsgTypeDataReceived;
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  EXPECT_CALL(*data_reader_mock->gmock, HandleMessage(sc.data_reader, _))
      .WillOnce(Invoke([](DataReaderObject*, MessageBuffer* msg_buf) {
        ASSERT_NE(msg_buf, nullptr);
        const uint8_t* msg = msg_buf->data;
        size_t msg_size    = msg_buf->data_size;

        std::unique_ptr<char[], decltype(&JsonFree)> expected(JsonUnformat(spine_data_received), JsonFree);
        ASSERT_NE(expected, nullptr) << "Wrong test input!";
        ASSERT_NE(msg, nullptr);
        ASSERT_GT(msg_size, 0);

        EXPECT_STREQ(reinterpret_cast<const char*>(msg), expected.get());
        EXPECT_EQ(msg_size, strlen(expected.get()) + 1);
      }));

  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));

  // Act: Handle Data Exchange
  DataExchange(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kDataExchange);
  ExpectCloseWithError("", true);
}

static constexpr std::string_view spine_data_to_send =
    R"({"datagram":[
      {"header":[
        {"specificationVersion":"1.3.0"},
        {"addressSource":[
          {"device":"HeatGenerationSystem"},
          {"entity":[0]},
          {"feature":0}
        ]},
        {"addressDestination":[
          {"device":"HeatGenerationSystem"},
          {"entity":[0]},
          {"feature":0}
        ]},
        {"msgCounter":1},
        {"cmdClassifier":"read"}
      ]},
      {"payload":[
        {"cmd":[
          [
            {"nodeManagementDetailedDiscoveryData":[]}
          ]
        ]}
      ]}
    ]})"sv;

static constexpr std::string_view websocket_write_msg =
    R"({"data":[
      {"header":[
        {"protocolId":"ee1.0"}
      ]},
      {"payload":
        {"datagram":[
          {"header":[
            {"specificationVersion":"1.3.0"},
            {"addressSource":[
              {"device":"HeatGenerationSystem"},
              {"entity":[0]},
              {"feature":0}
            ]},
            {"addressDestination":[
              {"device":"HeatGenerationSystem"},
              {"entity":[0]},
              {"feature":0}
            ]},
            {"msgCounter":1},
            {"cmdClassifier":"read"}
          ]},
          {"payload":[
            {"cmd":[
              [
                {"nodeManagementDetailedDiscoveryData":[]}
              ]
            ]}
          ]}
        ]}
      }
    ]})"sv;

TEST_F(ShipConnectionTestSuite, ShipConnectionDataExchangeSendSpineDataTest) {
  // Arrange:

  // Check only data exchange handling
  sc.is_access_methods_req_sent = true;

  // Set initial SME state
  SetShipConnectionState(kDataExchange);

  // Unformat JSON message
  std::unique_ptr<char[], decltype(&JsonFree)> datagram(JsonUnformat(spine_data_to_send), JsonFree);
  ASSERT_NE(datagram, nullptr) << "Wrong test input!";
  if (datagram == nullptr) {
    return;
  }

  // Init message bufer
  ShipConnectionQueueMessage queue_msg;
  MessageBufferInitWithDeallocator(
      &queue_msg.msg_buf,
      reinterpret_cast<uint8_t*>(datagram.get()),
      strlen(datagram.get()) + 1,
      nullptr
  );

  // Add message to queue
  queue_msg.type = kShipConnectionQueueMsgTypeSpineDataToSend;
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, _))
      .WillOnce(WithArgs<1, 2>(Invoke([](const uint8_t* msg, size_t msg_size) -> int32_t {
        EXPECT_NE(msg, nullptr);
        EXPECT_GT(msg_size, 1);
        if ((msg == nullptr) || (msg_size <= 1)) {
          return 0;
        }

        std::unique_ptr<char[], decltype(&JsonFree)> expected(JsonUnformat(websocket_write_msg), JsonFree);
        EXPECT_NE(expected, nullptr) << "Wrong test input!";
        if (expected == nullptr) {
          return 0;
        }

        EXPECT_EQ(msg[0], kMsgTypeData);
        std::string_view obtained(reinterpret_cast<const char*>(&msg[1]), msg_size - 1);
        EXPECT_EQ(obtained, expected.get()) << expected.get();
        EXPECT_EQ(msg_size - 1, strlen(expected.get()));
        return static_cast<int32_t>(msg_size);
      })));

  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));

  // Act: Handle Data Exchange
  DataExchange(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kDataExchange);
  ExpectCloseWithError("", true);
}

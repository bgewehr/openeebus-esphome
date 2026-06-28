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
/**
 * @file
 * @brief Mdns Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_SHIP_MDNS_MDNS_MOCK_H_
#define TESTS_SRC_MOCKS_SHIP_MDNS_MDNS_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/mdns_interface.h"

class MdnsGMockInterface {
 public:
  virtual ~MdnsGMockInterface() {};
  virtual void Destruct(ShipMdnsObject* self)                       = 0;
  virtual EebusError Start(ShipMdnsObject* self)                    = 0;
  virtual void Stop(ShipMdnsObject* self)                           = 0;
  virtual EebusError RegisterService(ShipMdnsObject* self)          = 0;
  virtual void DeregisterService(ShipMdnsObject* self)              = 0;
  virtual void SetAutoaccept(ShipMdnsObject* self, bool autoaccept) = 0;
};

class MdnsGMock : public MdnsGMockInterface {
 public:
  virtual ~MdnsGMock() {};
  MOCK_METHOD1(Destruct, void(ShipMdnsObject*));
  MOCK_METHOD1(Start, EebusError(ShipMdnsObject*));
  MOCK_METHOD1(Stop, void(ShipMdnsObject*));
  MOCK_METHOD1(RegisterService, EebusError(ShipMdnsObject*));
  MOCK_METHOD1(DeregisterService, void(ShipMdnsObject*));
  MOCK_METHOD2(SetAutoaccept, void(ShipMdnsObject*, bool));
};

typedef struct MdnsMock {
  /** Implements the Mdns Interface */
  ShipMdnsObject obj;
  MdnsGMock* gmock;
} MdnsMock;

#define MDNS_MOCK(obj) ((MdnsMock*)(obj))

MdnsMock* MdnsMockCreate(void);

static inline void MdnsMockDelete(MdnsMock* self) {
  if (self != nullptr) {
    MDNS_DESTRUCT(MDNS_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_SHIP_MDNS_MDNS_MOCK_H_

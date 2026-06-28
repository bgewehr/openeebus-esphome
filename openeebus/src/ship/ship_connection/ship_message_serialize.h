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
 * @brief Ship Message Serialize implementation declarations
 */

#ifndef SRC_SHIP_SHIP_CONNECTION_SHIP_MESSAGE_SERIALIZE_H_
#define SRC_SHIP_SHIP_CONNECTION_SHIP_MESSAGE_SERIALIZE_H_

#include "src/ship/api/ship_message_serialize_interface.h"
#include "src/ship/model/model.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

ShipMessageSerializeObject* ShipMessageSerializeCreate(const void* value, MsgValueType value_type);

void ShipMessageSerializeDelete(ShipMessageSerializeObject* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_CONNECTION_SHIP_MESSAGE_SERIALIZE_H_

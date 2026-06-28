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
 * @brief Datagram "private" declarations accessible for unit tests only
 */

#ifndef SRC_EEBUS_SRC_SPINE_MODEL_DATAGRAM_INTERNAL_H_
#define SRC_EEBUS_SRC_SPINE_MODEL_DATAGRAM_INTERNAL_H_

#include "src/spine/model/datagram.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

bool DatagramHeaderIsValid(const HeaderType* header);
bool DatagramPayloadIsValid(const PayloadType* payload);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_EEBUS_SRC_SPINE_MODEL_DATAGRAM_INTERNAL_H_

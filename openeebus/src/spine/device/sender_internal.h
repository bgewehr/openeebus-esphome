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
 * @brief Sender functions for unit test purpose only
 */

#ifndef SRC_SPINE_DEVICE_SENDER_INTERNAL_H_
#define SRC_SPINE_DEVICE_SENDER_INTERNAL_H_

#include "src/spine/api/sender_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#ifdef GTEST
void SenderSetMsgCounter(SenderObject* self, uint64_t msg_num);
#endif

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_DEVICE_SENDER_INTERNAL_H_

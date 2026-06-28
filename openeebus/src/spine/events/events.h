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

#ifndef SRC_EEBUS_SRC_SPINE_EVENTS_EVENTS_H_
#define SRC_EEBUS_SRC_SPINE_EVENTS_EVENTS_H_

#include "src/common/eebus_errors.h"
#include "src/spine/api/events.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EebusError EventSubscribe(EventHandlerLevel level, EventHandler handler, void* ctx);
EebusError EventUnsubscribe(EventHandlerLevel level, EventHandler handler, void* ctx);
void EventPublish(const EventPayload* payload);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // SRC_EEBUS_SRC_SPINE_EVENTS_EVENTS_H_

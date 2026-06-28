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

#include "src/spine/api/events.h"

#include "src/common/eebus_malloc.h"
#include "src/common/vector.h"

typedef struct EventHandlerInfo EventHandlerInfo;

struct EventHandlerInfo {
  EventHandlerLevel level;
  EventHandler handler;
  void* ctx;
};

static Vector handlers = {0};

EventHandlerInfo* EventHandlerInfoCreate(EventHandlerLevel level, EventHandler handler, void* ctx) {
  EventHandlerInfo* info = EEBUS_MALLOC(sizeof(*info));
  if (info == NULL) {
    return NULL;
  }

  info->level   = level;
  info->handler = handler;
  info->ctx     = ctx;
  return info;
}

void EventHandlerInfoDelete(EventHandlerInfo* info) { EEBUS_FREE(info); }

const EventHandlerInfo* EventHandlerFind(EventHandlerLevel level, EventHandler handler, void* ctx) {
  for (size_t i = 0; i < VectorGetSize(&handlers); ++i) {
    EventHandlerInfo* info = VectorGetElement(&handlers, i);
    if ((info->level == level) && (info->handler == handler) && (info->ctx == ctx)) {
      return info;
    }
  }

  return NULL;
}

EebusError EventSubscribe(EventHandlerLevel level, EventHandler handler, void* ctx) {
  if (EventHandlerFind(level, handler, ctx) != NULL) {
    return kEebusErrorOk;
  }

  EventHandlerInfo* const new_handler_info = EventHandlerInfoCreate(level, handler, ctx);
  if (new_handler_info == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  VectorPushBack(&handlers, new_handler_info);
  return kEebusErrorOk;
}

EebusError EventUnsubscribe(EventHandlerLevel level, EventHandler handler, void* ctx) {
  const EventHandlerInfo* const info = EventHandlerFind(level, handler, ctx);
  if (info == NULL) {
    return kEebusErrorNoChange;
  }

  VectorRemove(&handlers, (void*)info);
  EventHandlerInfoDelete((void*)info);
  if (VectorGetSize(&handlers) == 0) {
    // Required to pass the memory check test as vector can keep the allocated buffer
    VectorClear(&handlers);
  }

  return kEebusErrorOk;
}

void EventPublish(const EventPayload* payload) {
  // TODO: Check if level is required and should be analysed
  for (size_t i = 0; i < VectorGetSize(&handlers); ++i) {
    EventHandlerInfo* info = VectorGetElement(&handlers, i);
    info->handler(payload, info->ctx);
  }
}

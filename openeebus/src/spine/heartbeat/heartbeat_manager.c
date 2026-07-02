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
 * @brief Heartbeat Manager implementation
 */

#include <stdint.h>
#include <time.h>

#include "src/common/eebus_malloc.h"

/* ESPHome log bridge — defined in eebus_wp.cpp */
extern void eebus_log_d(const char* tag, int line, const char* fmt, ...);
#include "src/spine/api/entity_local_interface.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/heartbeat_manager_interface.h"
#include "src/spine/model/absolute_or_relative_time.h"
#include "src/spine/model/device_diagnosis_types.h"
#include "src/spine/model/model.h"

typedef struct HeartbeatManager HeartbeatManager;

struct HeartbeatManager {
  /** Implements the Heartbeat Manager Interface */
  HeartbeatManagerObject obj;

  EntityLocalObject* local_entity;
  FeatureLocalObject* local_feature;
  uint64_t heartbeat_num;
  uint32_t tick_cnt;
  uint32_t heartbeat_timeout;

  bool running;
};

#define HEARTBEAT_MANAGER(obj) ((HeartbeatManager*)(obj))

static void Destruct(HeartbeatManagerObject* self);
static bool IsHeartbeatRunning(const HeartbeatManagerObject* self);
static void SetLocalFeature(HeartbeatManagerObject* self, EntityLocalObject* entity, FeatureLocalObject* feature);
static void Tick(HeartbeatManagerObject* self);
static EebusError Start(HeartbeatManagerObject* self);
static void Stop(HeartbeatManagerObject* self);

static const HeartbeatManagerInterface heartbeat_manager_methods = {
    .destruct             = Destruct,
    .is_heartbeat_running = IsHeartbeatRunning,
    .set_local_feature    = SetLocalFeature,
    .tick                 = Tick,
    .start                = Start,
    .stop                 = Stop,
};

static void HeartbeatManagerConstruct(HeartbeatManager* self, EntityLocalObject* local_entity, uint32_t timeout);
static void UpdateHeartbeatData(HeartbeatManager* self);

void HeartbeatManagerConstruct(HeartbeatManager* self, EntityLocalObject* local_entity, uint32_t timeout) {
  // Override "virtual functions table"
  HEARTBEAT_MANAGER_INTERFACE(self) = &heartbeat_manager_methods;

  self->local_entity      = local_entity;
  self->local_feature     = NULL;
  self->heartbeat_num     = 0;
  self->tick_cnt          = timeout;
  self->heartbeat_timeout = timeout;
  self->running           = false;
}

HeartbeatManagerObject* HeartbeatManagerCreate(EntityLocalObject* local_entity, uint32_t timeout) {
  HeartbeatManager* const heartbeat_manager = (HeartbeatManager*)EEBUS_MALLOC(sizeof(HeartbeatManager));

  HeartbeatManagerConstruct(heartbeat_manager, local_entity, timeout);

  return HEARTBEAT_MANAGER_OBJECT(heartbeat_manager);
}

void Destruct(HeartbeatManagerObject* self) {
  Stop(self);
}

bool IsHeartbeatRunning(const HeartbeatManagerObject* self) {
  HeartbeatManager* const hm = HEARTBEAT_MANAGER(self);
  return hm->running;
}

void SetLocalFeature(HeartbeatManagerObject* self, EntityLocalObject* entity, FeatureLocalObject* feature) {
  HeartbeatManager* const hm = HEARTBEAT_MANAGER(self);

  if (entity == NULL || feature == NULL) {
    return;
  }

  FeatureObject* const fr = FEATURE_OBJECT(feature);
  if ((FEATURE_GET_TYPE(fr) != kFeatureTypeTypeDeviceDiagnosis) || (FEATURE_GET_ROLE(fr) != kRoleTypeServer)) {
    return;
  }

  if (FEATURE_GET_FUNCTION_OPERATIONS(fr, kFunctionTypeDeviceDiagnosisHeartbeatData) == NULL) {
    return;
  }

  hm->local_entity  = entity;
  hm->local_feature = feature;

  UpdateHeartbeatData(hm);
  Start(self);
}

void Tick(HeartbeatManagerObject* self) {
  HeartbeatManager* const hm = HEARTBEAT_MANAGER(self);

  if (hm->heartbeat_timeout == 0) {
    return;
  }

  if ((hm->running) && (hm->tick_cnt > 0)) {
    hm->tick_cnt--;
  }

  if (hm->tick_cnt == 0) {
    hm->heartbeat_num++;
    UpdateHeartbeatData(hm);
    /* Use 3/4 of the declared timeout (45 s for a 60 s interval) so the next
     * heartbeat arrives ~15 s before the remote's strict 1× deadline.
     * Without this fix, beats land at T=0/30/90 — the T=90 beat races
     * against the K40rf's T=30+60=90 disconnect timer and loses. */
    hm->tick_cnt = (hm->heartbeat_timeout * 3u) / 4u;
    if (hm->tick_cnt == 0u) {
      hm->tick_cnt = 1u;
    }
  }
}

void UpdateHeartbeatData(HeartbeatManager* self) {
  eebus_log_d("eebus_wp", __LINE__, "HEMS\xe2\x86\x92WP heartbeat (outbound): counter=%llu timeout=%us",
              (unsigned long long)self->heartbeat_num, (unsigned)self->heartbeat_timeout);
  DeviceDiagnosisHeartbeatDataType heartbeat_data = {
      .timestamp         = &ABSOLUTE_OR_RELATIVE_TIME_NOW,
      .heartbeat_counter = &self->heartbeat_num,
      .heartbeat_timeout = &(DurationType){.seconds = self->heartbeat_timeout},
  };

  FEATURE_LOCAL_SET_DATA(self->local_feature, kFunctionTypeDeviceDiagnosisHeartbeatData, &heartbeat_data);
}

EebusError Start(HeartbeatManagerObject* self) {
  HeartbeatManager* const hm = HEARTBEAT_MANAGER(self);

  hm->running = true;

  /* Immediately publish a fresh heartbeat so remote entities that subscribe
   * right after Start() see a current timestamp rather than the stale one
   * from before the last Stop(). Without this, a remote LP that subscribes
   * after a reconnect would see an old timestamp and conclude the EG
   * heartbeat is overdue, causing it to immediately disconnect. */
  if (hm->local_feature != NULL) {
    hm->heartbeat_num++;
    UpdateHeartbeatData(hm);
    /* Use half the timeout so the next periodic heartbeat fires at timeout/2
     * seconds — well before the remote LP's 1×-timeout deadline.
     * Prevents the race where K40rf's timer and our tick both expire at T=timeout. */
    hm->tick_cnt = (hm->heartbeat_timeout > 1u) ? (hm->heartbeat_timeout / 2u) : hm->heartbeat_timeout;
  }

  return kEebusErrorOk;
}

void Stop(HeartbeatManagerObject* self) {
  HeartbeatManager* const hm = HEARTBEAT_MANAGER(self);
  if (IsHeartbeatRunning(self)) {
    hm->running = false;
  }
}

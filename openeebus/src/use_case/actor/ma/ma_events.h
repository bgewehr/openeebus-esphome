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
 * @brief Shared MA event handlers for MPC and MGCP monitoring use cases
 */

#ifndef SRC_USE_CASE_ACTOR_MA_MA_EVENTS_H_
#define SRC_USE_CASE_ACTOR_MA_MA_EVENTS_H_

#include "src/spine/api/entity_remote_interface.h"
#include "src/spine/events/events.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void MaOnEntityAddedHandleElectricalConnection(const UseCase* use_case, EntityRemoteObject* entity);
void MaOnEntityAddedHandleMeasurement(const UseCase* use_case, EntityRemoteObject* entity);
void MaOnMeasurementDescriptionDataUpdate(const UseCase* use_case, const EventPayload* payload);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_MA_MA_EVENTS_H_

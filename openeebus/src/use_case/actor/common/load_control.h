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
 * @brief Load Control helper functions
 */

#ifndef SRC_USE_CASE_ACTOR_COMMON_LOAD_CONTROL_H_
#define SRC_USE_CASE_ACTOR_COMMON_LOAD_CONTROL_H_

#include "src/common/eebus_errors.h"
#include "src/spine/api/entity_local_interface.h"
#include "src/spine/api/entity_remote_interface.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/use_case/model/load_limit_types.h"

/**
 * @brief Generic helper to be used in LPC and LPP
 * to send new LoadControlLimit to the remote entity
 *
 * @param local_entity Pointer to the local entity object
 * @param remote_entity Pointer to the remote entity object
 * @param filter Filter to select the LoadControlLimitDescriptionDataType
 * @param limit Pointer to the LoadLimit structure containing the limit data to be sent
 * @return EebusError indicating the success or failure of the operation
 */
EebusError LoadControlWriteLimit(
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity,
    const LoadControlLimitDescriptionDataType* filter,
    const LoadLimit* limit
);

#endif  // SRC_USE_CASE_ACTOR_COMMON_LOAD_CONTROL_H_

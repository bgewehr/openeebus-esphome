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
 * @brief Energy Guard Limitation of Power use case base class to
 * be used by CS LPC and CS LPP concrete use cases
 */

#ifndef SRC_USE_CASE_ACTOR_EG_EG_LP_H_
#define SRC_USE_CASE_ACTOR_EG_EG_LP_H_

#include "src/spine/entity/entity_local.h"
#include "src/spine/model/common_data_types.h"
#include "src/use_case/api/eg_lp_listener_interface.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EgLpUseCaseObject EgLpUseCaseObject;
struct EgLpUseCaseObject {
  /** Inherits the Entity */
  UseCaseObject obj;
};

#define EG_LP_USE_CASE_OBJECT(obj) ((EgLpUseCaseObject*)(obj))

EgLpUseCaseObject* EgLpUseCaseCreate(
    EnergyDirectionType energy_direction,
    const UseCaseInfo* use_case_info,
    EntityLocalObject* local_entity,
    EgLpListenerObject* eg_lp_listener
);

static inline void EgLpUseCaseDelete(EgLpUseCaseObject* eg_lp_use_case) {
  if (eg_lp_use_case != NULL) {
    USE_CASE_DESTRUCT(USE_CASE_OBJECT(eg_lp_use_case));
    EEBUS_FREE(eg_lp_use_case);
  }
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 1
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the active power consumption limit
 *
 * @param self LP EG Use Case instance to get the active power consumption limit with
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param limit The active power consumption limit output buffer, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError
EgLpGetActivePowerLimit(const EgLpUseCaseObject* self, const EntityAddressType* remote_entity_addr, LoadLimit* limit);

/**
 * @brief Send the new active power consumption limit
 *
 * @param self LP EG Use Case instance to send the active power consumption limit with
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param limit The active power consumption limit to be sent
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError
EgLpSetActivePowerLimit(EgLpUseCaseObject* self, const EntityAddressType* remote_entity_addr, const LoadLimit* limit);

//-------------------------------------------------------------------------------------------//
//
// Scenario 2
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the Failsafe Limit for the consumed active (real) power of the
 * Controllable System. This limit becomes activated in "init" state or "failsafe state".
 *
 * @param self LP EG Use Case instance to get the Failsafe Limit with
 * @param power_limit Output buffer to store the Failsafe Power Limit value
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError EgLpGetFailsafeActivePowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
);

/**
 * @brief Send new Failsafe  Active Power Limit
 *
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param power_limit The new limit in W
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError EgLpSetFailsafeActivePowerLimit(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const ScaledValue* power_limit
);

/**
 * @brief Get the minimum time the Controllable System remains in "failsafe state" unless conditions
 * specified in this Use Case permit leaving the "failsafe state"
 *
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param duration The duration output buffer, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError EgLpGetFailsafeDurationMinimum(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    DurationType* duration
);

/**
 * @brief Send the new Failsafe Duration Minimum
 *
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param duration The duration, must be in range between 2h and 24h
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError EgLpSetFailsafeDurationMinimum(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const EebusDuration* duration
);

//-------------------------------------------------------------------------------------------//
//
// Scenario 3
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Start sending heartbeat from the local entity supporting this usecase
 *
 * The heartbeat is started by default when a non 0 timeout is set in the service configuration
 *
 * @param self EG LP Use Case instance to start the heartbeat with
 */
void EgLpStartHeartbeat(EgLpUseCaseObject* self);

/**
 * @brief Stop sending heartbeat from the local entity
 *
 * @param self EG LP Use Case instance to stop the heartbeat with
 */
void EgLpStopHeartbeat(EgLpUseCaseObject* self);

/**
 * @brief Check whether there was a heartbeat received within the last 2 minutes
 *
 * @param self EG LP Use Case instance to check the heartbeat data with
 * @return true if check is passed, false otherwise
 */
bool EgLpIsHeartbeatWithinDuration(EgLpUseCaseObject* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_EG_EG_LP_H_

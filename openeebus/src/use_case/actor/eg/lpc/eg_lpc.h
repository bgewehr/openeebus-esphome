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
 * @brief Energy Guard LPC use case
 */

#ifndef SRC_USE_CASE_ACTOR_EG_LPC_EG_LPC_H_
#define SRC_USE_CASE_ACTOR_EG_LPC_EG_LPC_H_

#include "src/spine/entity/entity_local.h"
#include "src/use_case/actor/eg/eg_lp.h"
#include "src/use_case/api/eg_lp_listener_interface.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EgLpUseCaseObject* EgLpcUseCaseCreate(EntityLocalObject* local_entity, EgLpListenerObject* eg_lpc_listener);

static inline void EgLpcUseCaseDelete(EgLpUseCaseObject* eg_lpc_use_case) {
  EgLpUseCaseDelete(eg_lpc_use_case);
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 1
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the active power consumption limit
 *
 * @param self LPC EG Use Case instance to get the active power consumption limit with
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param limit The active power consumption limit output buffer, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLpcGetActiveConsumptionPowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    LoadLimit* limit
) {
  return EgLpGetActivePowerLimit(self, remote_entity_addr, limit);
}

/**
 * @brief Send the new active power consumption limit
 *
 * @param self LPC EG Use Case instance to send the active power consumption limit with
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param limit The active power consumption limit to be sent
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLpcSetActiveConsumptionPowerLimit(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const LoadLimit* limit
) {
  return EgLpSetActivePowerLimit(self, remote_entity_addr, limit);
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 2
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the Failsafe Limit for the consumed active (real) power of the
 * Controllable System. This limit becomes activated in "init" state or "failsafe state".
 *
 * @param self LPC EG Use Case instance to get the Failsafe Limit with
 * @param power_limit Output buffer to store the Failsafe Power Limit value
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLpcGetFailsafeConsumptionActivePowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
) {
  return EgLpGetFailsafeActivePowerLimit(self, remote_entity_addr, power_limit);
}

/**
 * @brief Send new Failsafe Consumption Active Power Limit
 *
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param power_limit The new limit in W
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLpcSetFailsafeConsumptionActivePowerLimit(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const ScaledValue* power_limit
) {
  return EgLpSetFailsafeActivePowerLimit(self, remote_entity_addr, power_limit);
}

/**
 * @brief Get the minimum time the Controllable System remains in "failsafe state" unless conditions
 * specified in this Use Case permit leaving the "failsafe state"
 *
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param duration The duration output buffer, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLpcGetFailsafeDurationMinimum(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    DurationType* duration
) {
  return EgLpGetFailsafeDurationMinimum(self, remote_entity_addr, duration);
}

/**
 * @brief Send the new Failsafe Duration Minimum
 *
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param duration The duration, must be in range between 2h and 24h
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLpcSetFailsafeDurationMinimum(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const EebusDuration* duration
) {
  return EgLpSetFailsafeDurationMinimum(self, remote_entity_addr, duration);
}

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
 * @param self EG LPC Use Case instance to start the heartbeat with
 */
static inline void EgLpcStartHeartbeat(EgLpUseCaseObject* self) {
  EgLpStartHeartbeat(self);
}

/**
 * @brief Stop sending heartbeat from the local entity
 *
 * @param self EG LPC Use Case instance to stop the heartbeat with
 */
static inline void EgLpcStopHeartbeat(EgLpUseCaseObject* self) {
  EgLpStopHeartbeat(self);
}

/**
 * @brief Check whether there was a heartbeat received within the last 2 minutes
 *
 * @param self EG LPC Use Case instance to check the heartbeat data with
 * @return true if check is passed, false otherwise
 */
static inline bool EgLpcIsHeartbeatWithinDuration(EgLpUseCaseObject* self) {
  return EgLpIsHeartbeatWithinDuration(self);
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_EG_LPC_EG_LPC_H_

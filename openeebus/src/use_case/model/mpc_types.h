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
 * @brief MPC (MA and MU) type declarations, constants and utilities
 */

#ifndef SRC_USE_CASE_MODEL_MPC_TYPES_H_
#define SRC_USE_CASE_MODEL_MPC_TYPES_H_

#include "src/use_case/model/eebus_measurement_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef EebusMeasurementMonitorNameId MuMpcMonitorNameId;
typedef EebusMeasurementNameId MuMpcMeasurementNameId;

#define kMpcMonitorPower kEebusMeasurementMonitorPower
#define kMpcMonitorEnergy kEebusMeasurementMonitorEnergy
#define kMpcMonitorCurrent kEebusMeasurementMonitorCurrent
#define kMpcMonitorVoltage kEebusMeasurementMonitorVoltage
#define kMpcMonitorFrequency kEebusMeasurementMonitorFrequency
#define kMpcMonitorNameIdMask kEebusMeasurementMonitorNameIdMask

#define kMpcPowerTotal kMuPowerTotal
#define kMpcPowerPhaseA kMuPowerPhaseA
#define kMpcPowerPhaseB kMuPowerPhaseB
#define kMpcPowerPhaseC kMuPowerPhaseC
#define kMpcEnergyConsumed kMuEnergyConsumed
#define kMpcEnergyProduced kMuEnergyProduced
#define kMpcCurrentPhaseA kMuCurrentPhaseA
#define kMpcCurrentPhaseB kMuCurrentPhaseB
#define kMpcCurrentPhaseC kMuCurrentPhaseC
#define kMpcVoltagePhaseA kMuVoltagePhaseA
#define kMpcVoltagePhaseB kMuVoltagePhaseB
#define kMpcVoltagePhaseC kMuVoltagePhaseC
#define kMpcVoltagePhaseAb kMuVoltagePhaseAb
#define kMpcVoltagePhaseBc kMuVoltagePhaseBc
#define kMpcVoltagePhaseAc kMuVoltagePhaseAc
#define kMpcFrequency kMuFrequency

/**
 * @brief Get the MU MPC measurement name id from its string name
 * @param name String name (e.g. "power_total", "energy_consumed")
 * @return Pointer to the matching id, or NULL if not found
 */
const MuMpcMeasurementNameId* MuMpcMeasurementGetNameId(const char* name);

/**
 * @brief Get the string name of a MU MPC measurement
 * @param state The measurement name identifier
 * @return String name, or NULL if the identifier is unknown
 */
const char* MuMpcMeasurementGetName(MuMpcMeasurementNameId state);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_MODEL_MPC_TYPES_H_

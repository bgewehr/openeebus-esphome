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
 * @brief Ship connection debug logging specific functionality declarations
 */

#ifndef SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_DEBUG_H_
#define SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_DEBUG_H_

#include "src/ship/model/types.h"

/** Set SHIP_CONNECTION_DEBUG 1 to enable debug prints */
#ifndef SHIP_CONNECTION_DEBUG
#define SHIP_CONNECTION_DEBUG 0
#endif

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Convert the SME state name into a string
 * @param state SME state to be converted
 * @return String representation of state name on success
 * or empty string (not NULL) on failure
 */
const char* SmeStateToString(SmeState state);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_DEBUG_H_

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
 * @brief EEBUS Data related utility functions
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_UTIL_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_UTIL_H_

#include <stddef.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_enum.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Get the number of elements within EEBUS Data Configuration container
 * @param cfg_first Pointer onto First EEBUS Data Configuration instance within container
 * @return Number of elements within EEBUS Data Configuration container
 */
size_t EebusDataGetCfgSize(const EebusDataCfg* cfg_first);

/**
 * @brief Get the number of elements within Enum Mapping container
 * @param lenum_mapping_firstt Pointer onto first Enum Mapping instance within container
 * @return Number of elements within Enum Mapping container
 */
size_t EebusDataGetEnumMappingSize(const EnumMapping* enum_mapping_first);

const EnumMapping* EebusDataGetEnumMappingWithValue(const EnumMapping* enum_mapping_first, int32_t value);
const EnumMapping* EebusDataGetEnumMappingWithName(const EnumMapping* enum_mapping_first, const char* name);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_UTIL_H_

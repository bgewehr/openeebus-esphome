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
 * @brief Helper functions
 */

#ifndef SRC_USE_CASE_HELPER_H_
#define SRC_USE_CASE_HELPER_H_

#include <stdbool.h>

#include "src/common/eebus_data/eebus_data_list.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/feature_remote_interface.h"
#include "src/spine/model/function_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Retrieves feature data for a specified function type.
 *
 * This function retrieves data associated with a specific function type from either
 * a local feature object or a remote feature object.
 *
 * @param feature_local A pointer to the local feature object. Can be NULL if only
 *                      the remote feature object is used.
 * @param feature_remote A pointer to the remote feature object. Can be NULL if only
 *                       the local feature object is used.
 * @param function_type The function type for which the data is to be retrieved.
 * @return A pointer to the data associated with the specified function type, or NULL
 *         if no matching data is found.
 */
const void* HelperGetFeatureData(
    const FeatureLocalObject* feature_local,
    const FeatureRemoteObject* feature_remote,
    FunctionType function_type
);

/**
 * @brief Finds the first matching entry in a data list based on the specified filter.
 *
 * This function searches a data list associated with a specific function type and
 * initializes an iterator to point to the first entry that matches the given filter
 * criteria. The iterator can be used to traverse the list for additional matches.
 *
 * @param function_type The function type associated with the data list to be searched.
 * @param data_container A pointer to the container holding the data list to be searched.
 * @param filter A pointer to the filter criteria used to identify matching entries.
 * @param it A pointer to an EebusDataListMatchIterator structure, which will be
 *           initialized to point to the first matching entry.
 */
void HelperListMatchFirst(
    FunctionType function_type,
    const void* data_container,
    const void* filter,
    EebusDataListMatchIterator* it
);

/**
 * @brief Retrieves a unique matching entry from a data list based on the specified filter.
 *
 * This function searches a data list associated with a specific function type and
 * retrieves a unique entry that matches the given filter criteria. If no match is
 * found or if multiple matches are found, the function returns NULL.
 *
 * @param function_type The function type associated with the data list to be searched.
 * @param data_container A pointer to the container holding the data list to be searched.
 * @param filter A pointer to the filter criteria used to identify the matching entry.
 * @return A pointer to the unique matching entry in the data list, or NULL if no
 *         match is found or if multiple matches are found.
 */
const void* HelperGetListUniqueMatch(FunctionType function_type, const void* data_container, const void* filter);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_HELPER_H_

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
 * @brief Pointer to boolean helper
 */

#ifndef SRC_EEBUS_SRC_COMMON_BOOL_PTR_H_
#define SRC_EEBUS_SRC_COMMON_BOOL_PTR_H_

#include <stdbool.h>

#include "src/common/eebus_malloc.h"
#include "src/common/simple_ptr.h"

/**
 * @brief Helper function to dynamically create and intialise the bool data
 * @return Created pointer on success or NULL on fail
 */
static inline SIMPLE_PTR_CREATE_IMPL(BoolCreate, bool);

/**
 * @brief Helper function to delete the bool data
 */
static inline SIMPLE_PTR_DELETE_IMPL(BoolDelete, bool);

#endif  // SRC_EEBUS_SRC_COMMON_BOOL_PTR_H_

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
 * @brief Pointer to number helper
 */

#ifndef SRC_EEBUS_SRC_COMMON_NUM_PTR_H_
#define SRC_EEBUS_SRC_COMMON_NUM_PTR_H_

#include <stdint.h>

#include "src/common/eebus_malloc.h"
#include "src/common/simple_ptr.h"

/**
 * @defgroup CreateNum Helper functions to dynamically create and intialise the numeric data.
 * Take value to initialise the created data with.
 * Return the created pointer on success or NULL on fail.
 * @{
 */
static inline SIMPLE_PTR_CREATE_IMPL(Uint8Create, uint8_t);
static inline SIMPLE_PTR_CREATE_IMPL(Uint16Create, uint16_t);
static inline SIMPLE_PTR_CREATE_IMPL(Uint32Create, uint32_t);
static inline SIMPLE_PTR_CREATE_IMPL(Uint64Create, uint64_t);
static inline SIMPLE_PTR_CREATE_IMPL(Int8Create, int8_t);
static inline SIMPLE_PTR_CREATE_IMPL(Int16Create, int16_t);
static inline SIMPLE_PTR_CREATE_IMPL(Int32Create, int32_t);
static inline SIMPLE_PTR_CREATE_IMPL(Int64Create, int64_t);
/** @} */

/**
 * @defgroup DeleteNum Helper functions to delete the numeric data.
 * Take instance created with one of related @ref CreateNum functions.
 * @{
 */
static inline SIMPLE_PTR_DELETE_IMPL(Uint8Delete, uint8_t);
static inline SIMPLE_PTR_DELETE_IMPL(Uint16Delete, uint16_t);
static inline SIMPLE_PTR_DELETE_IMPL(Uint32Delete, uint32_t);
static inline SIMPLE_PTR_DELETE_IMPL(Uint64Delete, uint64_t);
static inline SIMPLE_PTR_DELETE_IMPL(Int8Delete, int8_t);
static inline SIMPLE_PTR_DELETE_IMPL(Int16Delete, int16_t);
static inline SIMPLE_PTR_DELETE_IMPL(Int32Delete, int32_t) static inline SIMPLE_PTR_DELETE_IMPL(Int64Delete, int64_t);
/** @} */

#endif  // SRC_EEBUS_SRC_COMMON_NUM_PTR_H_

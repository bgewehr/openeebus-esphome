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
 * @file eebus_assert.h
 * @brief Functions for asserting conditions
 */

#ifndef SRC_COMMON_EEBUS_ASSERT_H_
#define SRC_COMMON_EEBUS_ASSERT_H_

#include <assert.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

/**
 * @brief Asserts that the condition is true
 * @param cond Condition to assert
 */
#define EEBUS_ASSERT(cond) assert(cond)

/**
 * @brief Asserts that the condition should never be reached
 */
#define EEBUS_ASSERT_ALWAYS() EEBUS_ASSERT(0)

#if defined(__cplusplus)
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_ASSERT_H_

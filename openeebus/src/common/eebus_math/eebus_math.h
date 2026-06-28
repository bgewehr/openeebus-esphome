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
 * @file eebus_math.h
 * @brief Math functions used within EEBUS stack
 */

#ifndef SRC_COMMON_EEBUS_MATH_EEBUS_MATH_H_
#define SRC_COMMON_EEBUS_MATH_EEBUS_MATH_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Calculate power of ten
 * @param exponent Exponent
 * @return 10 raised to the power of exponent
 */
int64_t PowerOfTen(int8_t exponent);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_MATH_EEBUS_MATH_H_

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
 * @brief Structure utility
 */
#ifndef SRC_COMMON_STRUCT_UTIL_H_
#define SRC_COMMON_STRUCT_UTIL_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define STRUCT_MEMBER(type, field) (((type*)0)->field)

#define STRUCT_MEMBER_OFFSET(type, field) (size_t)(&STRUCT_MEMBER(type, field))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_STRUCT_UTIL_H_

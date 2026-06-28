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
 * @brief EEEBUS Data Tag types declarations
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_TAG_TYPES_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_TAG_TYPES_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef void* TagType;

#define EEBUS_TAG_RESET ((TagType)NULL)
#define EEBUS_TAG_SET ((TagType)(size_t)!EEBUS_TAG_RESET)
#define EEBUS_TAG_FROM_BOOL(v) ((v) ? EEBUS_TAG_SET : EEBUS_TAG_RESET)
#define EEBUS_TAG_TO_BOOL(v) ((v) != EEBUS_TAG_RESET)

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_TAG_TYPES_H_

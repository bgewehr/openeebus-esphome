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
 * @brief EEBUS Data handling based on configuration tree.
 * How to add the EEBUS Data specific type support:
 * 1. Declare a data structure considering that most of values have to be pointers
 * (exception is tag, see EEBUS_DATA_ definitions group for details).
 * 2. Declare a configuration tree using EEBUS_DATA_ definitions (this maps the
 * structure fields to specific EEBUS data)
 *
 * Simple example of Json text and related declarations is provided below
 * (see Json unit test with more usecases covered):
 * @code{.cpp}
 *
 * static const char* json_txt =
 * {"person": [
 *    {"name": "John Doe"},
 *    {"age": 43},
 *    {"address": [
 *      {"street": "10 Downing Street"},
 *      {"city": "London"}
 *    ]}
 * ]}
 *
 * //---------------------------------------------------------------------------//
 * // Data type structures
 * //---------------------------------------------------------------------------//
 * typedef struct Address Address;
 *
 * struct Address {
 *   char* street;
 *   char* city;
 * };
 *
 * typedef struct Person Person;
 *
 * struct Person {
 *   char* name;
 *   uint8_t* age;
 *   Address* address;
 * };
 *
 * //---------------------------------------------------------------------------//
 * // Data Configuration tree declaration
 * //---------------------------------------------------------------------------//
 * static const EebusDataCfg address_json_cfg[] = {
 *     EEBUS_DATA_STRING("street", Address, street),
 *     EEBUS_DATA_STRING("city", Address, city),
 *     EEBUS_DATA_END,
 * };
 *
 * static const EebusDataCfg person_sequence_json_cfg[] = {
 *     EEBUS_DATA_STRING("name", Person, name),
 *     EEBUS_DATA_UINT8("age", Person, age),
 *     EEBUS_DATA_SEQUENCE("address", Person, address, address_json_cfg),
 *     EEBUS_DATA_END,
 * };
 *
 * static const EebusDataCfg person_data_cfg
 *     = EEBUS_DATA_SEQUENCE_ROOT("person", Person, person_sequence_json_cfg);
 *
 * //---------------------------------------------------------------------------//
 *
 * Person* const person = (Person*)EEBUS_DATA_PARSE(&person_data_cfg, json_txt);
 * // ...
 * Person* person_copy = NULL;
 * EEBUS_DATA_COPY(&person_data_cfg, &person, &person_copy);
 * // ...
 * const char* s = EEBUS_DATA_PRINT_UNFORMATTED(&person_data_cfg, &person);
 * // ...
 * JsonFree(s);
 * EEBUS_DATA_DELETE(&person_data_cfg, &person_copy);
 * EEBUS_DATA_DELETE(&person_data_cfg, &person);
 * @endcode
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_bool.h"
#include "src/common/eebus_data/eebus_data_choice.h"
#include "src/common/eebus_data/eebus_data_choice_root.h"
#include "src/common/eebus_data/eebus_data_container.h"
#include "src/common/eebus_data/eebus_data_date_time.h"
#include "src/common/eebus_data/eebus_data_enum.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/common/eebus_data/eebus_data_numeric.h"
#include "src/common/eebus_data/eebus_data_sequence.h"
#include "src/common/eebus_data/eebus_data_string.h"
#include "src/common/eebus_data/eebus_data_tag.h"
#include "src/common/eebus_data/eebus_data_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_H_

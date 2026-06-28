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
 * @brief Wrapping layer over 3-rd party Json library
 */

#ifndef SRC_EEBUS_SRC_COMMON_JSON_H_
#define SRC_EEBUS_SRC_COMMON_JSON_H_

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct JsonObject JsonObject;

JsonObject* JsonCreateObject(void);
const JsonObject* JsonGetItem(const JsonObject* json_obj, const char* name, bool is_root);
JsonObject* JsonAddObjectToArray(JsonObject* json_ar);
JsonObject* JsonAddStringToArray(JsonObject* json_ar, const char* s);
JsonObject* JsonAddNumberToArray(JsonObject* json_obj, double num);
bool JsonAddItem(JsonObject* json_obj, const char* name, JsonObject* json_item, bool is_root);
bool JsonIsNumber(const JsonObject* json_obj);
double JsonGetNumber(const JsonObject* json_obj);
JsonObject* JsonCreateNumber(double num);
bool JsonIsString(const JsonObject* json_object);
const char* JsonGetString(const JsonObject* json_object);
JsonObject* JsonCreateString(const char* s);
bool JsonIsArray(const JsonObject* json_obj);
JsonObject* JsonCreateArray(void);
size_t JsonGetArraySize(const JsonObject* json_obj);
bool JsonAddItemToArray(JsonObject* json_obj, JsonObject* json_el);
JsonObject* JsonGetArrayItem(const JsonObject* json_obj, size_t idx);
bool JsonIsBool(const JsonObject* json_obj);
bool JsonGetBool(const JsonObject* json_obj);
JsonObject* JsonCreateBool(bool b);
JsonObject* JsonParse(const char* s);
JsonObject* JsonParseWithLength(const char* s, size_t len);
char* JsonPrintUnformatted(const JsonObject* json_obj);
void JsonDelete(JsonObject* json_obj);

/**
 * @brief Free the json allocated data
 * @param p pointer to data to be deallocated
 */
void JsonFree(void* p);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_EEBUS_SRC_COMMON_JSON_H_

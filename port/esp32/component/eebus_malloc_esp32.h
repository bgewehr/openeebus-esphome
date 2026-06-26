/*
 * Copyright 2025 bgewehr (bg-hems branch)
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
 * @file eebus_malloc_esp32.h
 * @brief ESP32 memory allocation override for openeebus.
 *
 * On ESP32-S3 with PSRAM, routes openeebus allocations to external RAM
 * (MALLOC_CAP_SPIRAM) to free up precious internal SRAM for the FreeRTOS
 * stack, WiFi, and mbedTLS.
 *
 * On plain ESP32 without PSRAM, falls back to standard heap.
 *
 * To activate: add to your sdkconfig.defaults:
 *   CONFIG_EEBUS_USE_PSRAM=y   (for ESP32-S3 with PSRAM)
 *
 * Or include this header before any openeebus headers in your main.c
 * and define EEBUS_MALLOC/EEBUS_FREE before including openeebus headers.
 */

#ifndef PORT_ESP32_EEBUS_MALLOC_ESP32_H_
#define PORT_ESP32_EEBUS_MALLOC_ESP32_H_

#include <esp_heap_caps.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_EEBUS_USE_PSRAM
  /**
   * Route openeebus allocations to PSRAM (SPIRAM) on ESP32-S3.
   * Falls back to internal heap if PSRAM allocation fails.
   */
  static inline void* eebus_malloc_psram(size_t size) {
    void* p = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!p) {
      p = heap_caps_malloc(size, MALLOC_CAP_8BIT);
    }
    return p;
  }

  #define EEBUS_MALLOC(size)  eebus_malloc_psram(size)
  #define EEBUS_FREE(ptr)     heap_caps_free(ptr)
#else
  /**
   * Standard internal heap allocation.
   */
  #define EEBUS_MALLOC(size)  malloc(size)
  #define EEBUS_FREE(ptr)     free(ptr)
#endif /* CONFIG_EEBUS_USE_PSRAM */

#ifdef __cplusplus
}
#endif

#endif  /* PORT_ESP32_EEBUS_MALLOC_ESP32_H_ */

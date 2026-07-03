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
 * @brief Debug functions implementation
 */

#include "src/common/debug.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

void DebugPrintf(const char* format, ...) {
  time_t now;
  struct tm tm;

  time(&now);
#ifdef _WIN32
  localtime_s(&tm, &now);
#else
  localtime_r(&now, &tm);
#endif  // _WIN32

  char timestamp_buf[16];
  strftime(timestamp_buf, sizeof(timestamp_buf), "%H:%M:%S", &tm);
  printf("[%s] ", timestamp_buf);

  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

void DebugHexdump(void* data, size_t data_size) {
  const uint8_t* const bytes = (const uint8_t*)data;

  for (size_t offset = 0; offset < data_size; offset += 16) {
    const size_t line_size = ((data_size - offset) < 16) ? (data_size - offset) : 16;

    printf("%08zx  ", offset);
    for (size_t i = 0; i < 16; ++i) {
      if (i < line_size) {
        printf("%02x ", bytes[offset + i]);
      } else {
        printf("   ");
      }
      if (i == 7) {
        printf(" ");
      }
    }

    printf(" |");
    for (size_t i = 0; i < line_size; ++i) {
      const uint8_t byte = bytes[offset + i];
      printf("%c", isprint(byte) ? (char)byte : '.');
    }
    printf("|\n");
  }
}

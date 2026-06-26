// ESP32 replacement for openeebus src/common/debug.c
// (original requires libwebsockets)
#include <stdarg.h>
#include <stddef.h>
#include "esp_log.h"
#define EEBUS_PLATFORM_FREERTOS 1
#define EEBUS_PLATFORM_ESP32    1
static const char *EEBUS_DBG_TAG_ = "openeebus";
void DebugPrintf(const char *format, ...) {
  va_list args; va_start(args, format);
  esp_log_writev(ESP_LOG_DEBUG, EEBUS_DBG_TAG_, format, args);
  va_end(args);
}
void DebugHexdump(void *data, size_t data_size) {
  ESP_LOG_BUFFER_HEXDUMP(EEBUS_DBG_TAG_, data, data_size, ESP_LOG_DEBUG);
}

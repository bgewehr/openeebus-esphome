# port/esp32/component/CMakeLists.txt
#
# ESP-IDF component for openeebus — integrates the openeebus SHIP/SPINE/LPC
# stack into an ESP-IDF project, replacing the libwebsockets dependency with
# native ESP-IDF websocket components.
#
# Usage in your ESP-IDF project:
#   set(EXTRA_COMPONENT_DIRS path/to/openeebus/port/esp32/component)
#   idf_component_register(REQUIRES eebus_esp32 ...)

cmake_minimum_required(VERSION 3.16)

# Root of the openeebus submodule (repo_root/openeebus/)
get_filename_component(OPENEEBUS_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../openeebus" ABSOLUTE)

# Collect all openeebus C sources except platform-specific alternatives
# (we provide our own websocket and use the built-in freertos impls)
file(GLOB_RECURSE OPENEEBUS_SOURCES
  "${OPENEEBUS_ROOT}/src/common/*.c"
  "${OPENEEBUS_ROOT}/src/ship/mdns/*.c"
  "${OPENEEBUS_ROOT}/src/ship/ship_connection/*.c"
  "${OPENEEBUS_ROOT}/src/ship/ship_node/*.c"
  "${OPENEEBUS_ROOT}/src/ship/tls_certificate/tls_certificate_mbedtls.c"
  "${OPENEEBUS_ROOT}/src/ship/websocket/websocket.c"
  "${OPENEEBUS_ROOT}/src/ship/websocket/websocket_creator.c"
  "${OPENEEBUS_ROOT}/src/ship/websocket/websocket_debug.c"
  "${OPENEEBUS_ROOT}/src/spine/*.c"
  "${OPENEEBUS_ROOT}/src/spine/binding/*.c"
  "${OPENEEBUS_ROOT}/src/spine/device/*.c"
  "${OPENEEBUS_ROOT}/src/spine/entity/*.c"
  "${OPENEEBUS_ROOT}/src/spine/events/*.c"
  "${OPENEEBUS_ROOT}/src/spine/feature/*.c"
  "${OPENEEBUS_ROOT}/src/spine/feature_link/*.c"
  "${OPENEEBUS_ROOT}/src/spine/function/*.c"
  "${OPENEEBUS_ROOT}/src/spine/heartbeat/*.c"
  "${OPENEEBUS_ROOT}/src/spine/model/*.c"
  "${OPENEEBUS_ROOT}/src/spine/node_management/*.c"
  "${OPENEEBUS_ROOT}/src/spine/subscription/*.c"
  "${OPENEEBUS_ROOT}/src/service/service/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/common/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/cs/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/cs/lpc/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/cs/lpp/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/eg/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/eg/lpc/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/eg/lpp/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/gcp/mgcp/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/ma/mgcp/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/ma/mpc/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/actor/mu/mpc/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/model/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/specialization/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/specialization/device_configuration/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/specialization/device_diagnosis/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/specialization/electrical_connection/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/specialization/load_control/*.c"
  "${OPENEEBUS_ROOT}/src/use_case/specialization/measurement/*.c"
)

# Exclude .tmp files and libwebsockets-specific platform files
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*\\.tmp$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*_bonjour\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*_linux\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*_apple\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*_windows\\.c$")
# Exclude libwebsockets server/client — replaced by our ESP32 port
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*websocket_server\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*websocket_client\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*websocket_server_creator\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*websocket_client_creator\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*http_server\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*openssl_util\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*applink\\.c$")
# Exclude openssl tls_certificate — we use mbedtls variant
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*tls_certificate\\.c$")
# Keep only freertos mutex/thread/queue/timer
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*eebus_mutex\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*eebus_thread\\.c$")
list(FILTER OPENEEBUS_SOURCES EXCLUDE REGEX ".*eebus_queue\\.c$")

# Our ESP32 WebSocket port
set(ESP32_WS_SOURCES
  "${CMAKE_CURRENT_LIST_DIR}/../websocket/websocket_server_esp32.c"
  "${CMAKE_CURRENT_LIST_DIR}/../websocket/websocket_client_esp32.c"
  "${CMAKE_CURRENT_LIST_DIR}/../websocket/websocket_server_creator_esp32.c"
  "${CMAKE_CURRENT_LIST_DIR}/../websocket/websocket_client_creator_esp32.c"
)

idf_component_register(
  SRCS
    ${OPENEEBUS_SOURCES}
    ${ESP32_WS_SOURCES}
  INCLUDE_DIRS
    "${OPENEEBUS_ROOT}"
    "${CMAKE_CURRENT_LIST_DIR}/.."
  REQUIRES
    freertos
    esp_http_server
    esp_https_server
    esp_websocket_client
    mdns
    mbedtls
    json
    esp_netif
    esp_event
    nvs_flash
    log
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC
  EEBUS_PLATFORM_FREERTOS=1
  EEBUS_PLATFORM_ESP32=1
)

# Allow the code to find eebus_malloc.h → maps to heap_caps on ESP32
target_compile_options(${COMPONENT_LIB} PRIVATE
  -Wno-unused-function
  -Wno-unused-variable
)

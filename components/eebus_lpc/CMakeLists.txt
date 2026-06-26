# esphome/components/eebus_lpc/CMakeLists.txt
#
# Tells ESPHome's CMake/ESP-IDF build to include the openeebus ESP32 port
# as an IDF component alongside this ESPHome component.

# Include the openeebus ESP32 IDF component
# ESPHome's build system adds extra component dirs via EXTRA_COMPONENT_DIRS;
# we add ours here relative to the repo root.
get_filename_component(OPENEEBUS_COMPONENT_DIR
  "${CMAKE_CURRENT_LIST_DIR}/../../../port/esp32/component" ABSOLUTE)

list(APPEND EXTRA_COMPONENT_DIRS "${OPENEEBUS_COMPONENT_DIR}")

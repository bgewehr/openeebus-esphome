/**
 * @file
 * @brief SPINE Datagram Actuator Switch payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_ACTUATOR_SWITCH_TYPES_INC_
#define SRC_SPINE_MODEL_ACTUATOR_SWITCH_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/actuator_switch_types.h"
#include "src/spine/model/common_data_types.inc"

static const EnumMapping actuator_switch_fct_lut[] = {
    {"on", kActuatorSwitchFctTypeOn},
    {"off", kActuatorSwitchFctTypeOff},
    {"toggle", kActuatorSwitchFctTypeToggle},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg actuator_switch_data_cfg[] = {
    EEBUS_DATA_ENUM("function", ActuatorSwitchDataType, function, actuator_switch_fct_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg actuator_switch_data_elements_cfg[] = {
    EEBUS_DATA_TAG("function", ActuatorSwitchDataElementsType, function),
    EEBUS_DATA_END,
};

static const EebusDataCfg actuator_switch_description_data_cfg[] = {
    EEBUS_DATA_STRING("label", ActuatorSwitchDescriptionDataType, label),
    EEBUS_DATA_STRING("description", ActuatorSwitchDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg actuator_switch_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("label", ActuatorSwitchDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", ActuatorSwitchDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_ACTUATOR_SWITCH_TYPES_INC_

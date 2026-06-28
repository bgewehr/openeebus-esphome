/**
 * @file
 * @brief SPINE Datagram Actuator Level payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_ACTUATOR_LEVEL_TYPES_INC_
#define SRC_SPINE_MODEL_ACTUATOR_LEVEL_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/actuator_level_types.h"
#include "src/spine/model/common_data_types.inc"

static const EnumMapping actuator_level_fct_lut[] = {
    {"start", kActuatorLevelFctTypeStart},
    {"up", kActuatorLevelFctTypeUp},
    {"down", kActuatorLevelFctTypeDown},
    {"stop", kActuatorLevelFctTypeStop},
    {"percentageAbsolute", kActuatorLevelFctTypePercentageAbsolute},
    {"percentageRelative", kActuatorLevelFctTypePercentageRelative},
    {"absolut", kActuatorLevelFctTypeAbsolut},
    {"relative", kActuatorLevelFctTypeRelative},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg actuator_level_data_cfg[] = {
    EEBUS_DATA_ENUM("function", ActuatorLevelDataType, function, actuator_level_fct_lut),
    EEBUS_DATA_SEQUENCE("value", ActuatorLevelDataType, value, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg actuator_level_data_elements_cfg[] = {
    EEBUS_DATA_TAG("function", ActuatorLevelDataElementsType, function),
    EEBUS_DATA_SEQUENCE("value", ActuatorLevelDataElementsType, value, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg actuator_level_description_data_cfg[] = {
    EEBUS_DATA_STRING("label", ActuatorLevelDescriptionDataType, label),
    EEBUS_DATA_STRING("description", ActuatorLevelDescriptionDataType, description),
    EEBUS_DATA_ENUM("levelDefaultUnit", ActuatorLevelDescriptionDataType, level_default_unit, unit_of_measurement_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg actuator_level_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("label", ActuatorLevelDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", ActuatorLevelDescriptionDataElementsType, description),
    EEBUS_DATA_TAG("levelDefaultUnit", ActuatorLevelDescriptionDataElementsType, level_default_unit),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_ACTUATOR_LEVEL_TYPES_INC_

/**
 * @file
 * @brief SPINE Datagram Sensing payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_SENSING_TYPES_INC_
#define SRC_SPINE_MODEL_SENSING_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/sensing_types.h"

static const EnumMapping sensing_state_lut[] = {
    {"on", kSensingStateTypeOn},
    {"off", kSensingStateTypeOff},
    {"toggle", kSensingStateTypeToggle},
    {"level", kSensingStateTypeLevel},
    {"levelUp", kSensingStateTypeLevelUp},
    {"levelDown", kSensingStateTypeLevelDown},
    {"levelStart", kSensingStateTypeLevelStart},
    {"levelStop", kSensingStateTypeLevelStop},
    {"levelAbsolute", kSensingStateTypeLevelAbsolute},
    {"levelRelative", kSensingStateTypeLevelRelative},
    {"levelPercentageAbsolute", kSensingStateTypeLevelPercentageAbsolute},
    {"levelPercentageRelative", kSensingStateTypeLevelPercentageRelative},
    {"pressed", kSensingStateTypePressed},
    {"longPressed", kSensingStateTypeLongPressed},
    {"released", kSensingStateTypeReleased},
    {"changed", kSensingStateTypeChanged},
    {"started", kSensingStateTypeStarted},
    {"stopped", kSensingStateTypeStopped},
    {"paused", kSensingStateTypePaused},
    {"middle", kSensingStateTypeMiddle},
    {"up", kSensingStateTypeUp},
    {"down", kSensingStateTypeDown},
    {"forward", kSensingStateTypeForward},
    {"backwards", kSensingStateTypeBackwards},
    {"open", kSensingStateTypeOpen},
    {"closed", kSensingStateTypeClosed},
    {"opening", kSensingStateTypeOpening},
    {"closing", kSensingStateTypeClosing},
    {"high", kSensingStateTypeHigh},
    {"low", kSensingStateTypeLow},
    {"day", kSensingStateTypeDay},
    {"night", kSensingStateTypeNight},
    {"detected", kSensingStateTypeDetected},
    {"notDetected", kSensingStateTypeNotDetected},
    {"alarmed", kSensingStateTypeAlarmed},
    {"notAlarmed", kSensingStateTypeNotAlarmed},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping sensing_type_lut[] = {
    {"switch", kSensingTypeTypeSwitch},
    {"button", kSensingTypeTypeButton},
    {"level", kSensingTypeTypeLevel},
    {"levelSwitch", kSensingTypeTypeLevelSwitch},
    {"windowHandle", kSensingTypeTypeWindowHandle},
    {"contactSensor", kSensingTypeTypeContactSensor},
    {"occupancySensor", kSensingTypeTypeOccupancySensor},
    {"motionDetector", kSensingTypeTypeMotionDetector},
    {"fireDetector", kSensingTypeTypeFireDetector},
    {"smokeDetector", kSensingTypeTypeSmokeDetector},
    {"heatDetector", kSensingTypeTypeHeatDetector},
    {"waterDetector", kSensingTypeTypeWaterDetector},
    {"gasDetector", kSensingTypeTypeGasDetector},
    {"alarmSensor", kSensingTypeTypeAlarmSensor},
    {"powerAlarmSensor", kSensingTypeTypePowerAlarmSensor},
    {"dayNightIndicator", kSensingTypeTypeDayNightIndicator},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg sensing_data_cfg[] = {
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", SensingDataType, timestamp),
    EEBUS_DATA_ENUM("state", SensingDataType, state, sensing_state_lut),
    EEBUS_DATA_SEQUENCE("value", SensingDataType, value, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg sensing_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timestamp", SensingDataElementsType, timestamp),
    EEBUS_DATA_TAG("state", SensingDataElementsType, state),
    EEBUS_DATA_SEQUENCE("value", SensingDataElementsType, value, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg sensing_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(SensingDataType), sensing_data_cfg);

static const EebusDataCfg sensing_list_data_cfg[] = {
    EEBUS_DATA_LIST("sensingData", SensingListDataType, sensing_data, &sensing_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg sensing_list_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("timestampInterval", SensingListDataSelectorsType, timestamp_interval, timestamp_interval_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg sensing_description_data_cfg[] = {
    EEBUS_DATA_ENUM("sensingType", SensingDescriptionDataType, sensing_type, sensing_type_lut),
    EEBUS_DATA_ENUM("unit", SensingDescriptionDataType, unit, unit_of_measurement_lut),
    EEBUS_DATA_ENUM("scopeType", SensingDescriptionDataType, scope_type, scope_type_lut),
    EEBUS_DATA_STRING("label", SensingDescriptionDataType, label),
    EEBUS_DATA_STRING("description", SensingDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg sensing_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sensingType", SensingDescriptionDataElementsType, sensing_type),
    EEBUS_DATA_TAG("unit", SensingDescriptionDataElementsType, unit),
    EEBUS_DATA_TAG("scopeType", SensingDescriptionDataElementsType, scope_type),
    EEBUS_DATA_TAG("label", SensingDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", SensingDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_SENSING_TYPES_INC_

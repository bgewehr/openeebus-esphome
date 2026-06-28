/**
 * @file
 * @brief SPINE Datagram Setpoint payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_SETPOINT_TYPES_INC_
#define SRC_SPINE_MODEL_SETPOINT_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/setpoint_types.h"

static const EnumMapping setpoint_type_lut[] = {
    {"valueAbsolute", kSetpointTypeTypeValueAbsolute},
    {"valueRelative", kSetpointTypeTypeValueRelative},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg setpoint_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("setpointId", SetpointDataType, setpoint_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("value", SetpointDataType, value, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("valueMin", SetpointDataType, value_min, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("valueMax", SetpointDataType, value_max, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("valueToleranceAbsolute", SetpointDataType, value_tolerance_absolute, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("valueTolerancePercentage", SetpointDataType, value_tolerance_percentage, scaled_number_cfg),
    EEBUS_DATA_BOOL("isSetpointChangeable", SetpointDataType, is_setpoint_changeable),
    EEBUS_DATA_BOOL("isSetpointActive", SetpointDataType, is_setpoint_active),
    EEBUS_DATA_SEQUENCE("timePeriod", SetpointDataType, time_period, time_period_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_data_elements_cfg[] = {
    EEBUS_DATA_TAG("setpointId", SetpointDataElementsType, setpoint_id),
    EEBUS_DATA_SEQUENCE("value", SetpointDataElementsType, value, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE("valueMin", SetpointDataElementsType, value_min, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE("valueMax", SetpointDataElementsType, value_max, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "valueToleranceAbsolute", SetpointDataElementsType, value_tolerance_absolute, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "valueTolerancePercentage", SetpointDataElementsType, value_tolerance_percentage, scaled_number_elements_cfg),
    EEBUS_DATA_TAG("isSetpointChangeable", SetpointDataElementsType, is_setpoint_changeable),
    EEBUS_DATA_TAG("isSetpointActive", SetpointDataElementsType, is_setpoint_active),
    EEBUS_DATA_SEQUENCE("timePeriod", SetpointDataElementsType, time_period, time_period_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(SetpointDataType), setpoint_data_cfg);

static const EebusDataCfg setpoint_list_data_cfg[] = {
    EEBUS_DATA_LIST("setpointData", SetpointListDataType, setpoint_data, &setpoint_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("setpointId", SetpointListDataSelectorsType, setpoint_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("setpointId", SetpointConstraintsDataType, setpoint_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("setpointRangeMin", SetpointConstraintsDataType, setpoint_range_min, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("setpointRangeMax", SetpointConstraintsDataType, setpoint_range_max, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("setpointStepSize", SetpointConstraintsDataType, setpoint_step_size, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("setpointId", SetpointConstraintsDataElementsType, setpoint_id),
    EEBUS_DATA_SEQUENCE(
        "setpointRangeMin", SetpointConstraintsDataElementsType, setpoint_range_min, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "setpointRangeMax", SetpointConstraintsDataElementsType, setpoint_range_max, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "setpointStepSize", SetpointConstraintsDataElementsType, setpoint_step_size, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_constraints_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(SetpointConstraintsDataType), setpoint_constraints_data_cfg);

static const EebusDataCfg setpoint_constraints_list_data_cfg[] = {
    EEBUS_DATA_LIST("setpointConstraintsData", SetpointConstraintsListDataType, setpoint_constraints_data,
        &setpoint_constraints_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_constraints_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("setpointId", SetpointConstraintsListDataSelectorsType, setpoint_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("setpointId", SetpointDescriptionDataType, setpoint_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "measurementId", SetpointDescriptionDataType, measurement_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32_WITH_FLAGS("timeTableId", SetpointDescriptionDataType, time_table_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("setpointType", SetpointDescriptionDataType, setpoint_type, setpoint_type_lut),
    EEBUS_DATA_SEQUENCE("unit", SetpointDescriptionDataType, unit, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("scopeType", SetpointDescriptionDataType, scope_type, scaled_number_cfg),
    EEBUS_DATA_STRING("label", SetpointDescriptionDataType, label),
    EEBUS_DATA_STRING("description", SetpointDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("setpointId", SetpointDescriptionDataElementsType, setpoint_id),
    EEBUS_DATA_TAG("measurementId", SetpointDescriptionDataElementsType, measurement_id),
    EEBUS_DATA_TAG("timeTableId", SetpointDescriptionDataElementsType, time_table_id),
    EEBUS_DATA_TAG("setpointType", SetpointDescriptionDataElementsType, setpoint_type),
    EEBUS_DATA_TAG("unit", SetpointDescriptionDataElementsType, unit),
    EEBUS_DATA_TAG("scopeType", SetpointDescriptionDataElementsType, scope_type),
    EEBUS_DATA_TAG("label", SetpointDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", SetpointDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(SetpointDescriptionDataType), setpoint_description_data_cfg);

static const EebusDataCfg setpoint_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("setpointDescriptionData", SetpointDescriptionListDataType, setpoint_description_data,
        &setpoint_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg setpoint_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("setpointId", SetpointDescriptionListDataSelectorsType, setpoint_id),
    EEBUS_DATA_UINT32("measurementId", SetpointDescriptionListDataSelectorsType, measurement_id),
    EEBUS_DATA_UINT32("timeTableId", SetpointDescriptionListDataSelectorsType, time_table_id),
    EEBUS_DATA_UINT32("setpointType", SetpointDescriptionListDataSelectorsType, setpoint_type),
    EEBUS_DATA_SEQUENCE("scopeType", SetpointDescriptionListDataSelectorsType, scope_type, scaled_number_cfg),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_SETPOINT_TYPES_INC_

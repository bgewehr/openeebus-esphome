/**
 * @file
 * @brief SPINE Datagram Loadcontrol payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_LOADCONTROL_TYPES_INC_
#define SRC_SPINE_MODEL_LOADCONTROL_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/loadcontrol_types.h"
#include "src/spine/model/measurement_types.inc"

static const EnumMapping load_control_event_action_lut[] = {
    {"pause", kLoadControlEventActionTypePause},
    {"resume", kLoadControlEventActionTypeResume},
    {"reduce", kLoadControlEventActionTypeReduce},
    {"increase", kLoadControlEventActionTypeIncrease},
    {"emergency", kLoadControlEventActionTypeEmergency},
    {"normal", kLoadControlEventActionTypeNormal},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping load_control_event_state_lut[] = {
    {"eventAccepted", kLoadControlEventStateTypeEventAccepted},
    {"eventStarted", kLoadControlEventStateTypeEventStarted},
    {"eventStopped", kLoadControlEventStateTypeEventStopped},
    {"eventRejected", kLoadControlEventStateTypeEventRejected},
    {"eventCancelled", kLoadControlEventStateTypeEventCancelled},
    {"eventError", kLoadControlEventStateTypeEventError},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping load_control_limit_type_lut[] = {
    {"minValueLimit", kLoadControlLimitTypeTypeMinValueLimit},
    {"maxValueLimit", kLoadControlLimitTypeTypeMaxValueLimit},
    {"signDependentAbsValueLimit", kLoadControlLimitTypeTypeSignDependentAbsValueLimit},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping load_control_category_lut[] = {
    {"obligation", kLoadControlCategoryTypeObligation},
    {"recommendation", kLoadControlCategoryTypeRecommendation},
    {"optimization", kLoadControlCategoryTypeOptimization},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg load_control_node_data_cfg[] = {
    EEBUS_DATA_BOOL("isNodeRemoteControllable", LoadControlNodeDataType, is_node_remote_controllable),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_node_data_elements_cfg[] = {
    EEBUS_DATA_TAG("isNodeRemoteControllable", LoadControlNodeDataElementsType, is_node_remote_controllable),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_event_data_cfg[] = {
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", LoadControlEventDataType, timestamp),
    EEBUS_DATA_UINT32_WITH_FLAGS("eventId", LoadControlEventDataType, event_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM(
        "eventActionConsume", LoadControlEventDataType, event_action_consume, load_control_event_action_lut),
    EEBUS_DATA_ENUM(
        "eventActionProduce", LoadControlEventDataType, event_action_produce, load_control_event_action_lut),
    EEBUS_DATA_SEQUENCE("timePeriod", LoadControlEventDataType, time_period, time_period_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_event_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timestamp", LoadControlEventDataElementsType, timestamp),
    EEBUS_DATA_TAG("eventId", LoadControlEventDataElementsType, event_id),
    EEBUS_DATA_TAG("eventActionConsume", LoadControlEventDataElementsType, event_action_consume),
    EEBUS_DATA_TAG("eventActionProduce", LoadControlEventDataElementsType, event_action_produce),
    EEBUS_DATA_SEQUENCE("timePeriod", LoadControlEventDataElementsType, time_period, time_period_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_event_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(LoadControlEventDataType), load_control_event_data_cfg);

static const EebusDataCfg load_control_event_list_data_cfg[] = {
    EEBUS_DATA_LIST("loadControlEventData", LoadControlEventListDataType, load_control_event_data,
        &load_control_event_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_event_list_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "timestampInterval", LoadControlEventListDataSelectorsType, timestamp_interval, timestamp_interval_cfg),
    EEBUS_DATA_UINT32("eventId", LoadControlEventListDataSelectorsType, event_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_state_data_cfg[] = {
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", LoadControlStateDataType, timestamp),
    EEBUS_DATA_UINT32_WITH_FLAGS("eventId", LoadControlStateDataType, event_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("eventStateConsume", LoadControlStateDataType, event_state_consume, load_control_event_state_lut),
    EEBUS_DATA_ENUM("appliedEventActionConsume", LoadControlStateDataType, applied_event_action_consume,
        load_control_event_action_lut),
    EEBUS_DATA_ENUM("eventStateProduce", LoadControlStateDataType, event_state_produce, load_control_event_state_lut),
    EEBUS_DATA_ENUM("appliedEventActionProduce", LoadControlStateDataType, applied_event_action_produce,
        load_control_event_action_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_state_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timestamp", LoadControlStateDataElementsType, timestamp),
    EEBUS_DATA_TAG("eventId", LoadControlStateDataElementsType, event_id),
    EEBUS_DATA_TAG("eventStateConsume", LoadControlStateDataElementsType, event_state_consume),
    EEBUS_DATA_TAG("appliedEventActionConsume", LoadControlStateDataElementsType, applied_event_action_consume),
    EEBUS_DATA_TAG("eventStateProduce", LoadControlStateDataElementsType, event_state_produce),
    EEBUS_DATA_TAG("appliedEventActionProduce", LoadControlStateDataElementsType, applied_event_action_produce),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_state_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(LoadControlStateDataType), load_control_state_data_cfg);

static const EebusDataCfg load_control_state_list_data_cfg[] = {
    EEBUS_DATA_LIST("loadControlStateData", LoadControlStateListDataType, load_control_state_data,
        &load_control_state_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_state_list_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "timestampInterval", LoadControlStateListDataSelectorsType, timestamp_interval, timestamp_interval_cfg),
    EEBUS_DATA_UINT32("eventId", LoadControlStateListDataSelectorsType, event_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("limitId", LoadControlLimitDataType, limit_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_BOOL("isLimitChangeable", LoadControlLimitDataType, is_limit_changeable),
    EEBUS_DATA_BOOL("isLimitActive", LoadControlLimitDataType, is_limit_active),
    EEBUS_DATA_SEQUENCE("timePeriod", LoadControlLimitDataType, time_period, time_period_cfg),
    EEBUS_DATA_SEQUENCE("value", LoadControlLimitDataType, value, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_data_elements_cfg[] = {
    EEBUS_DATA_TAG("limitId", LoadControlLimitDataElementsType, limit_id),
    EEBUS_DATA_TAG("isLimitChangeable", LoadControlLimitDataElementsType, is_limit_changeable),
    EEBUS_DATA_TAG("isLimitActive", LoadControlLimitDataElementsType, is_limit_active),
    EEBUS_DATA_SEQUENCE("timePeriod", LoadControlLimitDataElementsType, time_period, time_period_elements_cfg),
    EEBUS_DATA_SEQUENCE("value", LoadControlLimitDataElementsType, value, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(LoadControlLimitDataType), load_control_limit_data_cfg);

static const EebusDataCfg load_control_limit_list_data_cfg[] = {
    EEBUS_DATA_LIST("loadControlLimitData", LoadControlLimitListDataType, load_control_limit_data,
        &load_control_limit_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("limitId", LoadControlLimitListDataSelectorsType, limit_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("limitId", LoadControlLimitConstraintsDataType, limit_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("valueRangeMin", LoadControlLimitConstraintsDataType, value_range_min, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("valueRangeMax", LoadControlLimitConstraintsDataType, value_range_max, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("valueStepSize", LoadControlLimitConstraintsDataType, value_step_size, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("limitId", LoadControlLimitConstraintsDataElementsType, limit_id),
    EEBUS_DATA_SEQUENCE(
        "valueRangeMin", LoadControlLimitConstraintsDataElementsType, value_range_min, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "valueRangeMax", LoadControlLimitConstraintsDataElementsType, value_range_max, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "valueStepSize", LoadControlLimitConstraintsDataElementsType, value_step_size, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_constraints_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(LoadControlLimitConstraintsDataType), load_control_limit_constraints_data_cfg);

static const EebusDataCfg load_control_limit_constraints_list_data_cfg[] = {
    EEBUS_DATA_LIST("loadControlLimitConstraintsData", LoadControlLimitConstraintsListDataType,
        load_control_limit_constraints_data, &load_control_limit_constraints_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_constraints_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("limitId", LoadControlLimitConstraintsListDataSelectorsType, limit_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("limitId", LoadControlLimitDescriptionDataType, limit_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("limitType", LoadControlLimitDescriptionDataType, limit_type, load_control_limit_type_lut),
    EEBUS_DATA_ENUM("limitCategory", LoadControlLimitDescriptionDataType, limit_category, load_control_category_lut),
    EEBUS_DATA_ENUM("limitDirection", LoadControlLimitDescriptionDataType, limit_direction, energy_direction_lut),
    EEBUS_DATA_UINT32("measurementId", LoadControlLimitDescriptionDataType, measurement_id),
    EEBUS_DATA_ENUM("unit", LoadControlLimitDescriptionDataType, unit, unit_of_measurement_lut),
    EEBUS_DATA_ENUM("scopeType", LoadControlLimitDescriptionDataType, scope_type, scope_type_lut),
    EEBUS_DATA_STRING("label", LoadControlLimitDescriptionDataType, label),
    EEBUS_DATA_STRING("description", LoadControlLimitDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("limitId", LoadControlLimitDescriptionDataElementsType, limit_id),
    EEBUS_DATA_TAG("limitType", LoadControlLimitDescriptionDataElementsType, limit_type),
    EEBUS_DATA_TAG("limitCategory", LoadControlLimitDescriptionDataElementsType, limit_category),
    EEBUS_DATA_TAG("limitDirection", LoadControlLimitDescriptionDataElementsType, limit_direction),
    EEBUS_DATA_TAG("measurementId", LoadControlLimitDescriptionDataElementsType, measurement_id),
    EEBUS_DATA_TAG("unit", LoadControlLimitDescriptionDataElementsType, unit),
    EEBUS_DATA_TAG("scopeType", LoadControlLimitDescriptionDataElementsType, scope_type),
    EEBUS_DATA_TAG("label", LoadControlLimitDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", LoadControlLimitDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_description_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(LoadControlLimitDescriptionDataType), load_control_limit_description_data_cfg);

static const EebusDataCfg load_control_limit_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("loadControlLimitDescriptionData", LoadControlLimitDescriptionListDataType,
        load_control_limit_description_data, &load_control_limit_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg load_control_limit_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("limitId", LoadControlLimitDescriptionListDataSelectorsType, limit_id),
    EEBUS_DATA_ENUM(
        "limitType", LoadControlLimitDescriptionListDataSelectorsType, limit_type, load_control_limit_type_lut),
    EEBUS_DATA_ENUM(
        "limitDirection", LoadControlLimitDescriptionListDataSelectorsType, limit_direction, energy_direction_lut),
    EEBUS_DATA_UINT32("measurementId", LoadControlLimitDescriptionListDataSelectorsType, measurement_id),
    EEBUS_DATA_ENUM("scopeType", LoadControlLimitDescriptionListDataSelectorsType, scope_type, scope_type_lut),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_LOADCONTROL_TYPES_INC_

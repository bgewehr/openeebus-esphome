/**
 * @file
 * @brief SPINE Datagram Alarm payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_ALARM_TYPES_INC_
#define SRC_SPINE_MODEL_ALARM_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/alarm_types.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/threshold_types.inc"

static const EnumMapping alarm_type_lut[] = {
    {"alarmCancelled", kAlarmTypeTypeAlarmCancelled},
    {"underThreshold", kAlarmTypeTypeUnderThreshold},
    {"overThreshold", kAlarmTypeTypeOverThreshold},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg alarm_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("alarmId", AlarmDataType, alarm_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("thresholdId", AlarmDataType, threshold_id),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", AlarmDataType, timestamp),
    EEBUS_DATA_ENUM("alarmType", AlarmDataType, alarm_type, alarm_type_lut),
    EEBUS_DATA_SEQUENCE("measuredValue", AlarmDataType, measured_value, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("evaluationPeriod", AlarmDataType, evaluation_period, time_period_cfg),
    EEBUS_DATA_ENUM("scopeType", AlarmDataType, scope_type, scope_type_lut),
    EEBUS_DATA_STRING("label", AlarmDataType, label),
    EEBUS_DATA_STRING("description", AlarmDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg alarm_data_elements_cfg[] = {
    EEBUS_DATA_TAG("alarmId", AlarmDataElementsType, alarm_id),
    EEBUS_DATA_TAG("thresholdId", AlarmDataElementsType, threshold_id),
    EEBUS_DATA_TAG("timestamp", AlarmDataElementsType, timestamp),
    EEBUS_DATA_TAG("alarmType", AlarmDataElementsType, alarm_type),
    EEBUS_DATA_SEQUENCE("measuredValue", AlarmDataElementsType, measured_value, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE("evaluationPeriod", AlarmDataElementsType, evaluation_period, time_period_elements_cfg),
    EEBUS_DATA_TAG("scopeType", AlarmDataElementsType, scope_type),
    EEBUS_DATA_TAG("label", AlarmDataElementsType, label),
    EEBUS_DATA_TAG("description", AlarmDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg alarm_list_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(AlarmDataType), alarm_data_cfg);

static const EebusDataCfg alarm_list_data_cfg[] = {
    EEBUS_DATA_LIST("alarmListData", AlarmListDataType, alarm_list_data, &alarm_list_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg alarm_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("alarmId", AlarmListDataSelectorsType, alarm_id),
    EEBUS_DATA_ENUM("scopeType", AlarmListDataSelectorsType, scope_type, scope_type_lut),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_ALARM_TYPES_INC_

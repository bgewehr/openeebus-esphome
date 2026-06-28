/**
 * @file
 * @brief SPINE Datagram Hvac payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_HVAC_TYPES_INC_
#define SRC_SPINE_MODEL_HVAC_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/hvac_types.h"
#include "src/spine/model/power_sequences_types.inc"
#include "src/spine/model/setpoint_types.inc"
#include "src/spine/model/timetable_types.inc"

static const EnumMapping hvac_system_function_type_lut[] = {
    {"heating", kHvacSystemFunctionTypeTypeHeating},
    {"cooling", kHvacSystemFunctionTypeTypeCooling},
    {"ventilation", kHvacSystemFunctionTypeTypeVentilation},
    {"dhw", kHvacSystemFunctionTypeTypeDhw},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping hvac_operation_mode_type_lut[] = {
    {"auto", kHvacOperationModeTypeTypeAuto},
    {"on", kHvacOperationModeTypeTypeOn},
    {"off", kHvacOperationModeTypeTypeOff},
    {"eco", kHvacOperationModeTypeTypeEco},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping hvac_overrun_type_lut[] = {
    {"oneTimeDhw", kHvacOverrunTypeTypeOneTimeDhw},
    {"party", kHvacOverrunTypeTypeParty},
    {"sgReadyCondition1", kHvacOverrunTypeTypeSgReadyCondition1},
    {"sgReadyCondition3", kHvacOverrunTypeTypeSgReadyCondition3},
    {"sgReadyCondition4", kHvacOverrunTypeTypeSgReadyCondition4},
    {"oneDayAway", kHvacOverrunTypeTypeOneDayAway},
    {"oneDayAtHome", kHvacOverrunTypeTypeOneDayAtHome},
    {"oneTimeVentilation", kHvacOverrunTypeTypeOneTimeVentilation},
    {"hvacSystemOff", kHvacOverrunTypeTypeHvacSystemOff},
    {"valveKick", kHvacOverrunTypeTypeValveKick},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping hvac_overrun_status_lut[] = {
    {"active", kHvacOverrunStatusTypeActive},
    {"running", kHvacOverrunStatusTypeRunning},
    {"finished", kHvacOverrunStatusTypeFinished},
    {"inactive", kHvacOverrunStatusTypeInactive},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg hvac_system_function_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "systemFunctionId", HvacSystemFunctionDataType, system_function_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("currentOperationModeId", HvacSystemFunctionDataType, current_operation_mode_id),
    EEBUS_DATA_BOOL("isOperationModeIdChangeable", HvacSystemFunctionDataType, is_operation_mode_id_changeable),
    EEBUS_DATA_UINT32("currentSetpointId", HvacSystemFunctionDataType, current_setpoint_id),
    EEBUS_DATA_BOOL("isSetpointIdChangeable", HvacSystemFunctionDataType, is_setpoint_id_changeable),
    EEBUS_DATA_BOOL("isOverrunActive", HvacSystemFunctionDataType, is_overrun_active),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_data_elements_cfg[] = {
    EEBUS_DATA_TAG("systemFunctionId", HvacSystemFunctionDataElementsType, system_function_id),
    EEBUS_DATA_TAG("currentOperationModeId", HvacSystemFunctionDataElementsType, current_operation_mode_id),
    EEBUS_DATA_TAG("isOperationModeIdChangeable", HvacSystemFunctionDataElementsType, is_operation_mode_id_changeable),
    EEBUS_DATA_TAG("currentSetpointId", HvacSystemFunctionDataElementsType, current_setpoint_id),
    EEBUS_DATA_TAG("isSetpointIdChangeable", HvacSystemFunctionDataElementsType, is_setpoint_id_changeable),
    EEBUS_DATA_TAG("isOverrunActive", HvacSystemFunctionDataElementsType, is_overrun_active),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(HvacSystemFunctionDataType), hvac_system_function_data_cfg);

static const EebusDataCfg hvac_system_function_list_data_cfg[] = {
    EEBUS_DATA_LIST("hvacSystemFunctionData", HvacSystemFunctionListDataType, hvac_system_function_data,
        &hvac_system_function_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg system_function_id_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg hvac_system_function_list_data_selectors_cfg[] = {
    EEBUS_DATA_LIST("systemFunctionId", HvacSystemFunctionListDataSelectorsType, system_function_id,
        &system_function_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_operation_mode_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("systemFunctionId", HvacSystemFunctionOperationModeRelationDataType,
        system_function_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("operationModeId", HvacSystemFunctionOperationModeRelationDataType, operation_mode_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_operation_mode_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("systemFunctionId", HvacSystemFunctionOperationModeRelationDataElementsType, system_function_id),
    EEBUS_DATA_TAG("operationModeId", HvacSystemFunctionOperationModeRelationDataElementsType, operation_mode_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_operation_mode_relation_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(HvacSystemFunctionOperationModeRelationDataType),
        hvac_system_function_operation_mode_relation_data_cfg);

static const EebusDataCfg hvac_system_function_operation_mode_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("hvacSystemFunctionOperationModeRelationData", HvacSystemFunctionOperationModeRelationListDataType,
        hvac_system_function_operation_mode_relation_data,
        &hvac_system_function_operation_mode_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_operation_mode_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_LIST("systemFunctionId", HvacSystemFunctionOperationModeRelationListDataSelectorsType,
        system_function_id, &system_function_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_setpoint_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "systemFunctionId", HvacSystemFunctionSetpointRelationDataType, system_function_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("operationModeId", HvacSystemFunctionSetpointRelationDataType, operation_mode_id),
    EEBUS_DATA_UINT32("setpointId", HvacSystemFunctionSetpointRelationDataType, setpoint_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_setpoint_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("systemFunctionId", HvacSystemFunctionSetpointRelationDataElementsType, system_function_id),
    EEBUS_DATA_TAG("operationModeId", HvacSystemFunctionSetpointRelationDataElementsType, operation_mode_id),
    EEBUS_DATA_TAG("setpointId", HvacSystemFunctionSetpointRelationDataElementsType, setpoint_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_setpoint_relation_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(HvacSystemFunctionSetpointRelationDataType), hvac_system_function_setpoint_relation_data_cfg);

static const EebusDataCfg hvac_system_function_setpoint_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("hvacSystemFunctionSetpointRelationData", HvacSystemFunctionSetpointRelationListDataType,
        hvac_system_function_setpoint_relation_data, &hvac_system_function_setpoint_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_setpoint_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("systemFunctionId", HvacSystemFunctionSetpointRelationListDataSelectorsType, system_function_id),
    EEBUS_DATA_UINT32("operationModeId", HvacSystemFunctionSetpointRelationListDataSelectorsType, operation_mode_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_power_sequence_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("systemFunctionId", HvacSystemFunctionPowerSequenceRelationDataType,
        system_function_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_LIST(
        "sequenceId", HvacSystemFunctionPowerSequenceRelationDataType, sequence_id, &sequence_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_power_sequence_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("systemFunctionId", HvacSystemFunctionPowerSequenceRelationDataElementsType, system_function_id),
    EEBUS_DATA_TAG("sequenceId", HvacSystemFunctionPowerSequenceRelationDataElementsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_power_sequence_relation_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(HvacSystemFunctionPowerSequenceRelationDataType),
        hvac_system_function_power_sequence_relation_data_cfg);

static const EebusDataCfg hvac_system_function_power_sequence_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("hvacSystemFunctionPowerSequenceRelationData", HvacSystemFunctionPowerSequenceRelationListDataType,
        hvac_system_function_power_sequence_relation_data,
        &hvac_system_function_power_sequence_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_power_sequence_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32(
        "systemFunctionId", HvacSystemFunctionPowerSequenceRelationListDataSelectorsType, system_function_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "systemFunctionId", HvacSystemFunctionDescriptionDataType, system_function_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("systemFunctionType", HvacSystemFunctionDescriptionDataType, system_function_type,
        hvac_system_function_type_lut),
    EEBUS_DATA_STRING("label", HvacSystemFunctionDescriptionDataType, label),
    EEBUS_DATA_STRING("description", HvacSystemFunctionDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("systemFunctionId", HvacSystemFunctionDescriptionDataElementsType, system_function_id),
    EEBUS_DATA_TAG("systemFunctionType", HvacSystemFunctionDescriptionDataElementsType, system_function_type),
    EEBUS_DATA_TAG("label", HvacSystemFunctionDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", HvacSystemFunctionDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_description_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(HvacSystemFunctionDescriptionDataType), hvac_system_function_description_data_cfg);

static const EebusDataCfg hvac_system_function_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("hvacSystemFunctionDescriptionData", HvacSystemFunctionDescriptionListDataType,
        hvac_system_function_description_data, &hvac_system_function_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_system_function_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("systemFunctionId", HvacSystemFunctionDescriptionListDataSelectorsType, system_function_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_operation_mode_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "operationModeId", HvacOperationModeDescriptionDataType, operation_mode_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM(
        "operationModeType", HvacOperationModeDescriptionDataType, operation_mode_type, hvac_operation_mode_type_lut),
    EEBUS_DATA_STRING("label", HvacOperationModeDescriptionDataType, label),
    EEBUS_DATA_STRING("description", HvacOperationModeDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_operation_mode_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("operationModeId", HvacOperationModeDescriptionDataElementsType, operation_mode_id),
    EEBUS_DATA_TAG("operationModeType", HvacOperationModeDescriptionDataElementsType, operation_mode_type),
    EEBUS_DATA_TAG("label", HvacOperationModeDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", HvacOperationModeDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_operation_mode_description_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(HvacOperationModeDescriptionDataType), hvac_operation_mode_description_data_cfg);

static const EebusDataCfg hvac_operation_mode_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("hvacOperationModeDescriptionData", HvacOperationModeDescriptionListDataType,
        hvac_operation_mode_description_data, &hvac_operation_mode_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_operation_mode_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("operationModeId", HvacOperationModeDescriptionListDataSelectorsType, operation_mode_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_overrun_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("overrunId", HvacOverrunDataType, overrun_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("overrunStatus", HvacOverrunDataType, overrun_status, hvac_overrun_status_lut),
    EEBUS_DATA_UINT32("timeTableId", HvacOverrunDataType, time_table_id),
    EEBUS_DATA_BOOL("isOverrunStatusChangeable", HvacOverrunDataType, is_overrun_status_changeable),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_overrun_data_elements_cfg[] = {
    EEBUS_DATA_TAG("overrunId", HvacOverrunDataElementsType, overrun_id),
    EEBUS_DATA_TAG("overrunStatus", HvacOverrunDataElementsType, overrun_status),
    EEBUS_DATA_TAG("timeTableId", HvacOverrunDataElementsType, time_table_id),
    EEBUS_DATA_TAG("isOverrunStatusChangeable", HvacOverrunDataElementsType, is_overrun_status_changeable),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_overrun_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(HvacOverrunDataType), hvac_overrun_data_cfg);

static const EebusDataCfg hvac_overrun_list_data_cfg[] = {
    EEBUS_DATA_LIST("hvacOverrunData", HvacOverrunListDataType, hvac_overrun_data, &hvac_overrun_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_overrun_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("overrunId", HvacOverrunListDataSelectorsType, overrun_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg affected_system_function_id_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg hvac_overrun_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("overrunId", HvacOverrunDescriptionDataType, overrun_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("overrunType", HvacOverrunDescriptionDataType, overrun_type, hvac_overrun_type_lut),
    EEBUS_DATA_LIST("affectedSystemFunctionId", HvacOverrunDescriptionDataType, affected_system_function_id,
        &affected_system_function_id_element_data_cfg),
    EEBUS_DATA_STRING("label", HvacOverrunDescriptionDataType, label),
    EEBUS_DATA_STRING("description", HvacOverrunDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_overrun_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("overrunId", HvacOverrunDescriptionDataElementsType, overrun_id),
    EEBUS_DATA_TAG("overrunType", HvacOverrunDescriptionDataElementsType, overrun_type),
    EEBUS_DATA_TAG("affectedSystemFunctionId", HvacOverrunDescriptionDataElementsType, affected_system_function_id),
    EEBUS_DATA_TAG("label", HvacOverrunDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", HvacOverrunDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_overrun_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(HvacOverrunDescriptionDataType), hvac_overrun_description_data_cfg);

static const EebusDataCfg hvac_overrun_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("hvacOverrunDescriptionData", HvacOverrunDescriptionListDataType, hvac_overrun_description_data,
        &hvac_overrun_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg hvac_overrun_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("overrunId", HvacOverrunDescriptionListDataSelectorsType, overrun_id),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_HVAC_TYPES_INC_

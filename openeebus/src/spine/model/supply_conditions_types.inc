/**
 * @file
 * @brief SPINE Datagram Supply Conditions payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_SUPPLY_CONDITIONS_TYPES_INC_
#define SRC_SPINE_MODEL_SUPPLY_CONDITIONS_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/measurement_types.inc"
#include "src/spine/model/supply_conditions_types.h"
#include "src/spine/model/threshold_types.inc"

static const EnumMapping supply_condition_event_type_lut[] = {
    {"thesholdExceeded", kSupplyConditionEventTypeTypeThesholdExceeded},
    {"fallenBelowThreshold", kSupplyConditionEventTypeTypeFallenBelowThreshold},
    {"supplyInterrupt", kSupplyConditionEventTypeTypeSupplyInterrupt},
    {"releaseOfLimitations", kSupplyConditionEventTypeTypeReleaseOfLimitations},
    {"otherProblem", kSupplyConditionEventTypeTypeOtherProblem},
    {"gridConditionUpdate", kSupplyConditionEventTypeTypeGridConditionUpdate},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping supply_condition_originator_lut[] = {
    {"externDSO", kSupplyConditionOriginatorTypeExternDSO},
    {"externSupplier", kSupplyConditionOriginatorTypeExternSupplier},
    {"internalLimit", kSupplyConditionOriginatorTypeInternalLimit},
    {"internalService", kSupplyConditionOriginatorTypeInternalService},
    {"internalUser", kSupplyConditionOriginatorTypeInternalUser},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping grid_condition_lut[] = {
    {"consumptionRed", kGridConditionTypeConsumptionRed},
    {"consumptionYellow", kGridConditionTypeConsumptionYellow},
    {"good", kGridConditionTypeGood},
    {"productionRed", kGridConditionTypeProductionRed},
    {"productionYellow", kGridConditionTypeProductionYellow},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg supply_condition_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("conditionId", SupplyConditionDataType, condition_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", SupplyConditionDataType, timestamp),
    EEBUS_DATA_ENUM("eventType", SupplyConditionDataType, event_type, supply_condition_event_type_lut),
    EEBUS_DATA_ENUM("originator", SupplyConditionDataType, originator, supply_condition_originator_lut),
    EEBUS_DATA_UINT32("thresholdId", SupplyConditionDataType, threshold_id),
    EEBUS_DATA_SEQUENCE("thresholdPercentage", SupplyConditionDataType, threshold_percentage, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("relevantPeriod", SupplyConditionDataType, relevant_period, time_period_cfg),
    EEBUS_DATA_STRING("description", SupplyConditionDataType, description),
    EEBUS_DATA_ENUM("gridCondition", SupplyConditionDataType, grid_condition, grid_condition_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_data_elements_cfg[] = {
    EEBUS_DATA_TAG("conditionId", SupplyConditionDataElementsType, condition_id),
    EEBUS_DATA_TAG("timestamp", SupplyConditionDataElementsType, timestamp),
    EEBUS_DATA_TAG("eventType", SupplyConditionDataElementsType, event_type),
    EEBUS_DATA_TAG("originator", SupplyConditionDataElementsType, originator),
    EEBUS_DATA_TAG("thresholdId", SupplyConditionDataElementsType, threshold_id),
    EEBUS_DATA_SEQUENCE(
        "thresholdPercentage", SupplyConditionDataElementsType, threshold_percentage, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE("relevantPeriod", SupplyConditionDataElementsType, relevant_period, time_period_elements_cfg),
    EEBUS_DATA_TAG("description", SupplyConditionDataElementsType, description),
    EEBUS_DATA_TAG("gridCondition", SupplyConditionDataElementsType, grid_condition),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(SupplyConditionDataType), supply_condition_data_cfg);

static const EebusDataCfg supply_condition_list_data_cfg[] = {
    EEBUS_DATA_LIST("supplyConditionData", SupplyConditionListDataType, supply_condition_data,
        &supply_condition_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("conditionId", SupplyConditionListDataSelectorsType, condition_id),
    EEBUS_DATA_SEQUENCE(
        "timestampInterval", SupplyConditionListDataSelectorsType, timestamp_interval, timestamp_interval_cfg),
    EEBUS_DATA_ENUM("eventType", SupplyConditionListDataSelectorsType, event_type, supply_condition_event_type_lut),
    EEBUS_DATA_ENUM("originator", SupplyConditionListDataSelectorsType, originator, supply_condition_originator_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "conditionId", SupplyConditionDescriptionDataType, condition_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("commodityType", SupplyConditionDescriptionDataType, commodity_type, commodity_type_lut),
    EEBUS_DATA_ENUM(
        "positiveEnergyDirection", SupplyConditionDescriptionDataType, positive_energy_direction, energy_direction_lut),
    EEBUS_DATA_STRING("label", SupplyConditionDescriptionDataType, label),
    EEBUS_DATA_STRING("description", SupplyConditionDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("conditionId", SupplyConditionDescriptionDataElementsType, condition_id),
    EEBUS_DATA_TAG("commodityType", SupplyConditionDescriptionDataElementsType, commodity_type),
    EEBUS_DATA_TAG("positiveEnergyDirection", SupplyConditionDescriptionDataElementsType, positive_energy_direction),
    EEBUS_DATA_TAG("label", SupplyConditionDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", SupplyConditionDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_description_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(SupplyConditionDescriptionDataType), supply_condition_description_data_cfg);

static const EebusDataCfg supply_condition_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("supplyConditionDescriptionData", SupplyConditionDescriptionListDataType,
        supply_condition_description_data, &supply_condition_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("conditionId", SupplyConditionDescriptionListDataSelectorsType, condition_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_threshold_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "conditionId", SupplyConditionThresholdRelationDataType, condition_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_LIST(
        "thresholdId", SupplyConditionThresholdRelationDataType, threshold_id, &threshold_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_threshold_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("conditionId", SupplyConditionThresholdRelationDataElementsType, condition_id),
    EEBUS_DATA_TAG("thresholdId", SupplyConditionThresholdRelationDataElementsType, threshold_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_threshold_relation_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(SupplyConditionThresholdRelationDataType), supply_condition_threshold_relation_data_cfg);

static const EebusDataCfg supply_condition_threshold_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("SupplyConditionThresholdRelationDataType", SupplyConditionThresholdRelationListDataType,
        supply_condition_threshold_relation_data, &supply_condition_threshold_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg supply_condition_threshold_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("conditionId", SupplyConditionThresholdRelationListDataSelectorsType, condition_id),
    EEBUS_DATA_UINT32("thresholdId", SupplyConditionThresholdRelationListDataSelectorsType, threshold_id),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_SUPPLY_CONDITIONS_TYPES_INC_

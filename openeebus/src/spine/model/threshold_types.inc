/**
 * @file
 * @brief SPINE Datagram Threshold payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_THRESHOLD_TYPES_INC_
#define SRC_SPINE_MODEL_THRESHOLD_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/threshold_types.h"

static const EnumMapping threshold_type_lut[] = {
    {"goodAbove", kThresholdTypeTypeGoodAbove},
    {"badAbove", kThresholdTypeTypeBadAbove},
    {"goodBelow", kThresholdTypeTypeGoodBelow},
    {"badBelow", kThresholdTypeTypeBadBelow},
    {"minValueThreshold", kThresholdTypeTypeMinValueThreshold},
    {"maxValueThreshold", kThresholdTypeTypeMaxValueThreshold},
    {"minValueThresholdExtreme", kThresholdTypeTypeMinValueThresholdExtreme},
    {"maxValueThresholdExtreme", kThresholdTypeTypeMaxValueThresholdExtreme},
    {"sagThreshold", kThresholdTypeTypeSagThreshold},
    {"swellThreshold", kThresholdTypeTypeSwellThreshold},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg threshold_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("thresholdId", ThresholdDataType, threshold_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("thresholdValue", ThresholdDataType, threshold_value, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_data_elements_cfg[] = {
    EEBUS_DATA_TAG("thresholdId", ThresholdDataElementsType, threshold_id),
    EEBUS_DATA_SEQUENCE("thresholdValue", ThresholdDataElementsType, threshold_value, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(ThresholdDataType), threshold_data_cfg);

static const EebusDataCfg threshold_list_data_cfg[] = {
    EEBUS_DATA_LIST("thresholdData", ThresholdListDataType, threshold_data, &threshold_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("thresholdId", ThresholdListDataSelectorsType, threshold_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("thresholdId", ThresholdConstraintsDataType, threshold_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("thresholdRangeMin", ThresholdConstraintsDataType, threshold_range_min, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("thresholdRangeMax", ThresholdConstraintsDataType, threshold_range_max, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("thresholdStepSize", ThresholdConstraintsDataType, threshold_step_size, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("thresholdId", ThresholdConstraintsDataElementsType, threshold_id),
    EEBUS_DATA_SEQUENCE(
        "thresholdRangeMin", ThresholdConstraintsDataElementsType, threshold_range_min, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "thresholdRangeMax", ThresholdConstraintsDataElementsType, threshold_range_max, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "thresholdStepSize", ThresholdConstraintsDataElementsType, threshold_step_size, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_constraints_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(ThresholdConstraintsDataType), threshold_constraints_data_cfg);

static const EebusDataCfg threshold_constraints_list_data_cfg[] = {
    EEBUS_DATA_LIST("thresholdConstraintsData", ThresholdConstraintsListDataType, threshold_constraints_data,
        &threshold_constraints_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_constraints_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("thresholdId", ThresholdConstraintsListDataSelectorsType, threshold_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("thresholdId", ThresholdDescriptionDataType, threshold_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("thresholdType", ThresholdDescriptionDataType, threshold_type, threshold_type_lut),
    EEBUS_DATA_ENUM("unit", ThresholdDescriptionDataType, unit, unit_of_measurement_lut),
    EEBUS_DATA_ENUM("scopeType", ThresholdDescriptionDataType, scope_type, scope_type_lut),
    EEBUS_DATA_STRING("label", ThresholdDescriptionDataType, label),
    EEBUS_DATA_STRING("description", ThresholdDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("thresholdId", ThresholdDescriptionDataElementsType, threshold_id),
    EEBUS_DATA_TAG("thresholdType", ThresholdDescriptionDataElementsType, threshold_type),
    EEBUS_DATA_TAG("unit", ThresholdDescriptionDataElementsType, unit),
    EEBUS_DATA_TAG("scopeType", ThresholdDescriptionDataElementsType, scope_type),
    EEBUS_DATA_TAG("label", ThresholdDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", ThresholdDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(ThresholdDescriptionDataType), threshold_description_data_cfg);

static const EebusDataCfg threshold_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("thresholdDescriptionData", ThresholdDescriptionListDataType, threshold_description_data,
        &threshold_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("thresholdId", ThresholdDescriptionListDataSelectorsType, threshold_id),
    EEBUS_DATA_ENUM("scopeType", ThresholdDescriptionListDataSelectorsType, scope_type, scope_type_lut),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_THRESHOLD_TYPES_INC_

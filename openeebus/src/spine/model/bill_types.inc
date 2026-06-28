/**
 * @file
 * @brief SPINE Datagram Bill payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_BILL_TYPES_INC_
#define SRC_SPINE_MODEL_BILL_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/bill_types.h"
#include "src/spine/model/common_data_types.inc"

static const EnumMapping bill_type_lut[] = {
    {"chargingSummary", kBillTypeTypeChargingSummary},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping bill_position_type_lut[] = {
    {"gridElectricEnergy", kBillPositionTypeTypeGridElectricEnergy},
    {"selfProducedElectricEnergy", kBillPositionTypeTypeSelfProducedElectricEnergy},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping bill_cost_type_lut[] = {
    {"absolutePrice", kBillCostTypeTypeAbsolutePrice},
    {"relativePrice", kBillCostTypeTypeRelativePrice},
    {"co2Emission", kBillCostTypeTypeCo2Emission},
    {"renewableEnergy", kBillCostTypeTypeRenewableEnergy},
    {"radioactiveWaste", kBillCostTypeTypeRadioactiveWaste},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg bill_value_cfg[] = {
    EEBUS_DATA_UINT32("valueId", BillValueType, value_id),
    EEBUS_DATA_ENUM("unit", BillValueType, unit, unit_of_measurement_lut),
    EEBUS_DATA_SEQUENCE("value", BillValueType, value, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("valuePercentage", BillValueType, value_percentage, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_value_elements_cfg[] = {
    EEBUS_DATA_TAG("valueId", BillValueElementsType, value_id),
    EEBUS_DATA_TAG("unit", BillValueElementsType, unit),
    EEBUS_DATA_TAG("value", BillValueElementsType, value),
    EEBUS_DATA_TAG("valuePercentage", BillValueElementsType, value_percentage),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_cost_cfg[] = {
    EEBUS_DATA_UINT32("costId", BillCostType, cost_id),
    EEBUS_DATA_ENUM("costType", BillCostType, cost_type, bill_cost_type_lut),
    EEBUS_DATA_UINT32("valueId", BillCostType, value_id),
    EEBUS_DATA_ENUM("unit", BillCostType, unit, unit_of_measurement_lut),
    EEBUS_DATA_ENUM("currency", BillCostType, currency, currency_lut),
    EEBUS_DATA_SEQUENCE("cost", BillCostType, cost, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("costPercentage", BillCostType, cost_percentage, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_cost_elements_cfg[] = {
    EEBUS_DATA_TAG("costId", BillCostElementsType, cost_id),
    EEBUS_DATA_TAG("costType", BillCostElementsType, cost_type),
    EEBUS_DATA_TAG("valueId", BillCostElementsType, value_id),
    EEBUS_DATA_TAG("unit", BillCostElementsType, unit),
    EEBUS_DATA_TAG("currency", BillCostElementsType, currency),
    EEBUS_DATA_SEQUENCE("cost", BillCostElementsType, cost, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE("costPercentage", BillCostElementsType, cost_percentage, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_position_cfg[] = {
    EEBUS_DATA_UINT32("positionId", BillPositionType, position_id),
    EEBUS_DATA_ENUM("positionType", BillPositionType, position_type, bill_position_type_lut),
    EEBUS_DATA_SEQUENCE("timePeriod", BillPositionType, time_period, time_period_cfg),
    EEBUS_DATA_SEQUENCE("value", BillPositionType, value, bill_value_cfg),
    EEBUS_DATA_SEQUENCE("cost", BillPositionType, cost, bill_cost_cfg),
    EEBUS_DATA_STRING("label", BillPositionType, label),
    EEBUS_DATA_STRING("description", BillPositionType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_position_elements_cfg[] = {
    EEBUS_DATA_TAG("positionId", BillPositionElementsType, position_id),
    EEBUS_DATA_TAG("positionType", BillPositionElementsType, position_type),
    EEBUS_DATA_SEQUENCE("timePeriod", BillPositionElementsType, time_period, time_period_elements_cfg),
    EEBUS_DATA_SEQUENCE("value", BillPositionElementsType, value, bill_value_elements_cfg),
    EEBUS_DATA_SEQUENCE("cost", BillPositionElementsType, cost, bill_cost_elements_cfg),
    EEBUS_DATA_TAG("label", BillPositionElementsType, label),
    EEBUS_DATA_TAG("description", BillPositionElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg position_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(BillPositionType), bill_position_cfg);

static const EebusDataCfg bill_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("billId", BillDataType, bill_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("billType", BillDataType, bill_type, bill_type_lut),
    EEBUS_DATA_ENUM("scopeType", BillDataType, scope_type, scope_type_lut),
    EEBUS_DATA_SEQUENCE("total", BillDataType, total, bill_position_cfg),
    EEBUS_DATA_LIST("position", BillDataType, position, &position_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_data_elements_cfg[] = {
    EEBUS_DATA_TAG("billId", BillDataElementsType, bill_id),
    EEBUS_DATA_TAG("billType", BillDataElementsType, bill_type),
    EEBUS_DATA_TAG("scopeType", BillDataElementsType, scope_type),
    EEBUS_DATA_SEQUENCE("total", BillDataElementsType, total, bill_position_elements_cfg),
    EEBUS_DATA_SEQUENCE("position", BillDataElementsType, position, bill_position_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(BillDataType), bill_data_cfg);

static const EebusDataCfg bill_list_data_cfg[] = {
    EEBUS_DATA_LIST("billData", BillListDataType, bill_data, &bill_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("billId", BillListDataSelectorsType, bill_id),
    EEBUS_DATA_ENUM("scopeType", BillListDataSelectorsType, scope_type, scope_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("billId", BillConstraintsDataType, bill_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("positionCountMin", BillConstraintsDataType, position_count_min),
    EEBUS_DATA_UINT32("positionCountMax", BillConstraintsDataType, position_count_max),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("billId", BillConstraintsDataElementsType, bill_id),
    EEBUS_DATA_TAG("positionCountMin", BillConstraintsDataElementsType, position_count_min),
    EEBUS_DATA_TAG("positionCountMax", BillConstraintsDataElementsType, position_count_max),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_constraints_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(BillConstraintsDataType), bill_constraints_data_cfg);

static const EebusDataCfg bill_constraints_list_data_cfg[] = {
    EEBUS_DATA_LIST("billConstraintsData", BillConstraintsListDataType, bill_constraints_data,
        &bill_constraints_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_constraints_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("billId", BillConstraintsListDataSelectorsType, bill_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg supported_bill_type_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(enum, sizeof(int32_t), NULL);

static const EebusDataCfg bill_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("billId", BillDescriptionDataType, bill_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_BOOL("billWriteable", BillDescriptionDataType, bill_writeable),
    EEBUS_DATA_BOOL("updateRequired", BillDescriptionDataType, update_required),
    EEBUS_DATA_LIST(
        "supportedBillType", BillDescriptionDataType, supported_bill_type, &supported_bill_type_element_data_cfg),
    EEBUS_DATA_UINT32("sessionId", BillDescriptionDataType, session_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("billId", BillDescriptionDataElementsType, bill_id),
    EEBUS_DATA_TAG("billWriteable", BillDescriptionDataElementsType, bill_writeable),
    EEBUS_DATA_TAG("updateRequired", BillDescriptionDataElementsType, update_required),
    EEBUS_DATA_TAG("supportedBillType", BillDescriptionDataElementsType, supported_bill_type),
    EEBUS_DATA_TAG("sessionId", BillDescriptionDataElementsType, session_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(BillDescriptionDataType), bill_description_data_cfg);

static const EebusDataCfg bill_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("billDescriptionData", BillDescriptionListDataType, bill_description_data,
        &bill_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg bill_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("billId", BillDescriptionListDataSelectorsType, bill_id),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_BILL_TYPES_INC_

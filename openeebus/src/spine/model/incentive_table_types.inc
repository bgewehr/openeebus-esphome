/**
 * @file
 * @brief SPINE Datagram Incentive Table payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_INCENTIVE_TABLE_TYPES_INC_
#define SRC_SPINE_MODEL_INCENTIVE_TABLE_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/incentive_table_types.h"
#include "src/spine/model/tariff_information_types.inc"
#include "src/spine/model/timetable_types.inc"

static const EebusDataCfg boundary_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TierBoundaryDataType), tier_boundary_data_cfg);

static const EebusDataCfg incentive_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(IncentiveDataType), incentive_data_cfg);

static const EebusDataCfg incentive_table_tier_cfg[] = {
    EEBUS_DATA_SEQUENCE("tier", IncentiveTableTierType, tier, tier_data_cfg),
    EEBUS_DATA_LIST("boundary", IncentiveTableTierType, boundary, &boundary_element_data_cfg),
    EEBUS_DATA_LIST("incentive", IncentiveTableTierType, incentive, &incentive_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(IncentiveTableTierType), incentive_table_tier_cfg);

static const EebusDataCfg incentive_table_incentive_slot_cfg[] = {
    EEBUS_DATA_SEQUENCE("timeInterval", IncentiveTableIncentiveSlotType, time_interval, time_table_data_cfg),
    EEBUS_DATA_LIST("tier", IncentiveTableIncentiveSlotType, tier, &tier_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_slot_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(IncentiveTableIncentiveSlotType), incentive_table_incentive_slot_cfg);

static const EebusDataCfg incentive_table_cfg[] = {
    EEBUS_DATA_SEQUENCE("tariff", IncentiveTableType, tariff, tariff_data_cfg),
    EEBUS_DATA_LIST("incentiveSlot", IncentiveTableType, incentive_slot, &incentive_slot_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_tier_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("tier", IncentiveTableTierElementsType, tier, tier_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("boundary", IncentiveTableTierElementsType, boundary, tier_boundary_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("incentive", IncentiveTableTierElementsType, incentive, incentive_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_incentive_slot_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "timeInterval", IncentiveTableIncentiveSlotElementsType, time_interval, time_table_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("tier", IncentiveTableIncentiveSlotElementsType, tier, incentive_table_tier_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("tariff", IncentiveTableElementsType, tariff, tariff_data_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "incentiveSlot", IncentiveTableElementsType, incentive_slot, incentive_table_incentive_slot_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(IncentiveTableType), incentive_table_cfg);

static const EebusDataCfg incentive_table_data_cfg[] = {
    EEBUS_DATA_LIST("incentiveTable", IncentiveTableDataType, incentive_table, &incentive_table_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "incentiveTable", IncentiveTableDataElementsType, incentive_table, incentive_table_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("tariff", IncentiveTableDataSelectorsType, tariff, tariff_list_data_selectors_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg boundary_description_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TierBoundaryDescriptionDataType), tier_boundary_description_data_cfg);

static const EebusDataCfg incentive_description_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(IncentiveDescriptionDataType), incentive_description_data_cfg);

static const EebusDataCfg incentive_table_description_tier_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "tierDescription", IncentiveTableDescriptionTierType, tier_description, tier_description_data_cfg),
    EEBUS_DATA_LIST("boundaryDescription", IncentiveTableDescriptionTierType, boundary_description,
        &boundary_description_element_data_cfg),
    EEBUS_DATA_LIST("incentiveDescription", IncentiveTableDescriptionTierType, incentive_description,
        &incentive_description_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_description_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "tariffDescription", IncentiveTableDescriptionType, tariff_description, tariff_description_data_cfg),
    EEBUS_DATA_LIST("tier", IncentiveTableDescriptionType, tier, &tier_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_description_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("tariffDescription", IncentiveTableDescriptionElementsType, tariff_description,
        tariff_description_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("tier", IncentiveTableDescriptionElementsType, tier, incentive_table_description_tier_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_description_tier_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("tierDescription", IncentiveTableDescriptionTierElementsType, tier_description,
        tier_description_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("boundaryDescription", IncentiveTableDescriptionTierElementsType, boundary_description,
        tier_boundary_description_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("incentiveDescription", IncentiveTableDescriptionTierElementsType, incentive_description,
        incentive_description_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_description_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(IncentiveTableDescriptionType), incentive_table_description_cfg);

static const EebusDataCfg incentive_table_description_data_cfg[] = {
    EEBUS_DATA_LIST("incentiveTableDescription", IncentiveTableDescriptionDataType, incentive_table_description,
        &incentive_table_description_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_description_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("incentiveTableDescription", IncentiveTableDescriptionDataElementsType,
        incentive_table_description, incentive_table_description_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_description_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("tariffDescription", IncentiveTableDescriptionDataSelectorsType, tariff_description,
        tariff_description_list_data_selectors_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_constraints_cfg[] = {
    EEBUS_DATA_SEQUENCE("tariff", IncentiveTableConstraintsType, tariff, tariff_data_cfg),
    EEBUS_DATA_SEQUENCE(
        "tariffConstraints", IncentiveTableConstraintsType, tariff_constraints, tariff_overall_constraints_data_cfg),
    EEBUS_DATA_SEQUENCE("incentiveSlotConstraints", IncentiveTableConstraintsType, incentive_slot_constraints,
        time_table_constraints_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_constraints_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("tariff", IncentiveTableConstraintsElementsType, tariff, tariff_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("tariffConstraints", IncentiveTableConstraintsElementsType, tariff_constraints,
        tariff_overall_constraints_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("incentiveSlotConstraints", IncentiveTableConstraintsElementsType, incentive_slot_constraints,
        time_table_constraints_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_constraints_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(IncentiveTableConstraintsType), incentive_table_constraints_cfg);

static const EebusDataCfg incentive_table_constraints_data_cfg[] = {
    EEBUS_DATA_LIST("incentiveTableConstraints", IncentiveTableConstraintsDataType, incentive_table_constraints,
        &incentive_table_constraints_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_constraints_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("incentiveTableConstraints", IncentiveTableConstraintsDataElementsType,
        incentive_table_constraints, incentive_table_constraints_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_table_constraints_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("tariff", IncentiveTableConstraintsDataSelectorsType, tariff, tariff_list_data_selectors_cfg),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_INCENTIVE_TABLE_TYPES_INC_

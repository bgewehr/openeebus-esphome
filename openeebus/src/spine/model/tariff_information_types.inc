/**
 * @file
 * @brief SPINE Datagram Tariff Information payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_TARIFF_INFORMATION_TYPES_INC_
#define SRC_SPINE_MODEL_TARIFF_INFORMATION_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/measurement_types.inc"
#include "src/spine/model/tariff_information_types.h"
#include "src/spine/model/timetable_types.inc"

static const EnumMapping tier_boundary_type_lut[] = {
    {"powerBoundary", kTierBoundaryTypeTypePowerBoundary},
    {"energyBoundary", kTierBoundaryTypeTypeEnergyBoundary},
    {"countBoundary", kTierBoundaryTypeTypeCountBoundary},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping tier_type_lut[] = {
    {"fixedCost", kTierTypeTypeFixedCost},
    {"dynamicCost", kTierTypeTypeDynamicCost},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping incentive_type_lut[] = {
    {"absoluteCost", kIncentiveTypeTypeAbsoluteCost},
    {"relativeCost", kIncentiveTypeTypeRelativeCost},
    {"renewableEnergyPercentage", kIncentiveTypeTypeRenewableEnergyPercentage},
    {"co2Emission", kIncentiveTypeTypeCo2Emission},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping incentive_value_type_lut[] = {
    {"value", kIncentiveValueTypeTypeValue},
    {"averageValue", kIncentiveValueTypeTypeAverageValue},
    {"minValue", kIncentiveValueTypeTypeMinvalue},
    {"maxValue", kIncentiveValueTypeTypeMaxvalue},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg tariff_overall_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32("maxTariffCount", TariffOverallConstraintsDataType, max_tariff_count),
    EEBUS_DATA_UINT32("maxBoundaryCount", TariffOverallConstraintsDataType, max_boundary_count),
    EEBUS_DATA_UINT32("maxTierCount", TariffOverallConstraintsDataType, max_tier_count),
    EEBUS_DATA_UINT32("maxIncentiveCount", TariffOverallConstraintsDataType, max_incentive_count),
    EEBUS_DATA_UINT32("maxBoundariesPerTariff", TariffOverallConstraintsDataType, max_boundaries_per_tariff),
    EEBUS_DATA_UINT32("maxTiersPerTariff", TariffOverallConstraintsDataType, max_tiers_per_tariff),
    EEBUS_DATA_UINT32("maxBoundariesPerTier", TariffOverallConstraintsDataType, max_boundaries_per_tier),
    EEBUS_DATA_UINT32("maxIncentivesPerTier", TariffOverallConstraintsDataType, max_incentives_per_tier),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_overall_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("maxTariffCount", TariffOverallConstraintsDataElementsType, max_tariff_count),
    EEBUS_DATA_TAG("maxBoundaryCount", TariffOverallConstraintsDataElementsType, max_boundary_count),
    EEBUS_DATA_TAG("maxTierCount", TariffOverallConstraintsDataElementsType, max_tier_count),
    EEBUS_DATA_TAG("maxIncentiveCount", TariffOverallConstraintsDataElementsType, max_incentive_count),
    EEBUS_DATA_TAG("maxBoundariesPerTariff", TariffOverallConstraintsDataElementsType, max_boundaries_per_tariff),
    EEBUS_DATA_TAG("maxTiersPerTariff", TariffOverallConstraintsDataElementsType, max_tiers_per_tariff),
    EEBUS_DATA_TAG("maxBoundariesPerTier", TariffOverallConstraintsDataElementsType, max_boundaries_per_tier),
    EEBUS_DATA_TAG("maxIncentivesPerTier", TariffOverallConstraintsDataElementsType, max_incentives_per_tier),
    EEBUS_DATA_END,
};

static const EebusDataCfg active_tier_id_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg tariff_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("tariffId", TariffDataType, tariff_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_LIST("activeTierId", TariffDataType, active_tier_id, &active_tier_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_data_elements_cfg[] = {
    EEBUS_DATA_TAG("tariffId", TariffDataElementsType, tariff_id),
    EEBUS_DATA_TAG("activeTierId", TariffDataElementsType, active_tier_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TariffDataType), tariff_data_cfg);

static const EebusDataCfg tariff_list_data_cfg[] = {
    EEBUS_DATA_LIST("tariffData", TariffListDataType, tariff_data, &tariff_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("tariffId", TariffListDataSelectorsType, tariff_id),
    EEBUS_DATA_UINT32("activeTierId", TariffListDataSelectorsType, active_tier_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_id_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg tariff_tier_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("tariffId", TariffTierRelationDataType, tariff_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_LIST("tierId", TariffTierRelationDataType, tier_id, &tier_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_tier_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("tariffId", TariffTierRelationDataElementsType, tariff_id),
    EEBUS_DATA_TAG("tierId", TariffTierRelationDataElementsType, tier_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_tier_relation_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TariffTierRelationDataType), tariff_tier_relation_data_cfg);

static const EebusDataCfg tariff_tier_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("tariffTierRelationData", TariffTierRelationListDataType, tariff_tier_relation_data,
        &tariff_tier_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_tier_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("tariffId", TariffTierRelationListDataSelectorsType, tariff_id),
    EEBUS_DATA_UINT32("tierId", TariffTierRelationListDataSelectorsType, tier_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg boundary_id_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg tariff_boundary_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("tariffId", TariffBoundaryRelationDataType, tariff_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_LIST("boundaryId", TariffBoundaryRelationDataType, boundary_id, &boundary_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_boundary_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("tariffId", TariffBoundaryRelationDataElementsType, tariff_id),
    EEBUS_DATA_TAG("boundaryId", TariffBoundaryRelationDataElementsType, boundary_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_boundary_relation_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TariffBoundaryRelationDataType), tariff_boundary_relation_data_cfg);

static const EebusDataCfg tariff_boundary_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("tariffBoundaryRelationData", TariffBoundaryRelationListDataType, tariff_boundary_relation_data,
        &tariff_boundary_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_boundary_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("tariffId", TariffBoundaryRelationListDataSelectorsType, tariff_id),
    EEBUS_DATA_UINT32("boundaryId", TariffBoundaryRelationListDataSelectorsType, boundary_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("tariffId", TariffDescriptionDataType, tariff_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("commodityId", TariffDescriptionDataType, commodity_id),
    EEBUS_DATA_UINT32("measurementId", TariffDescriptionDataType, measurement_id),
    EEBUS_DATA_BOOL("tariffWriteable", TariffDescriptionDataType, tariff_writeable),
    EEBUS_DATA_BOOL("updateRequired", TariffDescriptionDataType, update_required),
    EEBUS_DATA_ENUM("scopeType", TariffDescriptionDataType, scope_type, scope_type_lut),
    EEBUS_DATA_STRING("label", TariffDescriptionDataType, label),
    EEBUS_DATA_STRING("description", TariffDescriptionDataType, description),
    EEBUS_DATA_BOOL("slotIdSupport", TariffDescriptionDataType, slot_id_support),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("tariffId", TariffDescriptionDataElementsType, tariff_id),
    EEBUS_DATA_TAG("commodityId", TariffDescriptionDataElementsType, commodity_id),
    EEBUS_DATA_TAG("measurementId", TariffDescriptionDataElementsType, measurement_id),
    EEBUS_DATA_TAG("tariffWriteable", TariffDescriptionDataElementsType, tariff_writeable),
    EEBUS_DATA_TAG("updateRequired", TariffDescriptionDataElementsType, update_required),
    EEBUS_DATA_TAG("scopeType", TariffDescriptionDataElementsType, scope_type),
    EEBUS_DATA_TAG("label", TariffDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", TariffDescriptionDataElementsType, description),
    EEBUS_DATA_TAG("slotIdSupport", TariffDescriptionDataElementsType, slot_id_support),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TariffDescriptionDataType), tariff_description_data_cfg);

static const EebusDataCfg tariff_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("tariffDescriptionData", TariffDescriptionListDataType, tariff_description_data,
        &tariff_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tariff_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("tariffId", TariffDescriptionListDataSelectorsType, tariff_id),
    EEBUS_DATA_UINT32("commodityId", TariffDescriptionListDataSelectorsType, commodity_id),
    EEBUS_DATA_UINT32("measurementId", TariffDescriptionListDataSelectorsType, measurement_id),
    EEBUS_DATA_ENUM("scopeType", TariffDescriptionListDataSelectorsType, scope_type, scope_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_boundary_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("boundaryId", TierBoundaryDataType, boundary_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("timePeriod", TierBoundaryDataType, time_period, time_period_cfg),
    EEBUS_DATA_UINT32("timeTableId", TierBoundaryDataType, time_table_id),
    EEBUS_DATA_SEQUENCE("lowerBoundaryValue", TierBoundaryDataType, lower_boundary_value, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("upperBoundaryValue", TierBoundaryDataType, upper_boundary_value, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_boundary_data_elements_cfg[] = {
    EEBUS_DATA_TAG("boundaryId", TierBoundaryDataElementsType, boundary_id),
    EEBUS_DATA_SEQUENCE("timePeriod", TierBoundaryDataElementsType, time_period, time_period_elements_cfg),
    EEBUS_DATA_TAG("timeTableId", TierBoundaryDataElementsType, time_table_id),
    EEBUS_DATA_SEQUENCE(
        "lowerBoundaryValue", TierBoundaryDataElementsType, lower_boundary_value, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "upperBoundaryValue", TierBoundaryDataElementsType, upper_boundary_value, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_boundary_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TierBoundaryDataType), tier_boundary_data_cfg);

static const EebusDataCfg tier_boundary_list_data_cfg[] = {
    EEBUS_DATA_LIST(
        "tierBoundaryData", TierBoundaryListDataType, tier_boundary_data, &tier_boundary_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_boundary_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("boundaryId", TierBoundaryListDataSelectorsType, boundary_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_boundary_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "boundaryId", TierBoundaryDescriptionDataType, boundary_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("boundaryType", TierBoundaryDescriptionDataType, boundary_type, tier_boundary_type_lut),
    EEBUS_DATA_UINT32("validForTierId", TierBoundaryDescriptionDataType, valid_for_tier_id),
    EEBUS_DATA_UINT32("switchToTierIdWhenLower", TierBoundaryDescriptionDataType, switch_to_tier_id_when_lower),
    EEBUS_DATA_UINT32("switchToTierIdWhenHigher", TierBoundaryDescriptionDataType, switch_to_tier_id_when_higher),
    EEBUS_DATA_ENUM("boundaryUnit", TierBoundaryDescriptionDataType, boundary_unit, unit_of_measurement_lut),
    EEBUS_DATA_STRING("label", TierBoundaryDescriptionDataType, label),
    EEBUS_DATA_STRING("description", TierBoundaryDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_boundary_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("boundaryId", TierBoundaryDescriptionDataElementsType, boundary_id),
    EEBUS_DATA_TAG("boundaryType", TierBoundaryDescriptionDataElementsType, boundary_type),
    EEBUS_DATA_TAG("validForTierId", TierBoundaryDescriptionDataElementsType, valid_for_tier_id),
    EEBUS_DATA_TAG("switchToTierIdWhenLower", TierBoundaryDescriptionDataElementsType, switch_to_tier_id_when_lower),
    EEBUS_DATA_TAG("switchToTierIdWhenHigher", TierBoundaryDescriptionDataElementsType, switch_to_tier_id_when_higher),
    EEBUS_DATA_TAG("boundaryUnit", TierBoundaryDescriptionDataElementsType, boundary_unit),
    EEBUS_DATA_TAG("label", TierBoundaryDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", TierBoundaryDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_boundary_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TierBoundaryDescriptionDataType), tier_boundary_description_data_cfg);

static const EebusDataCfg tier_boundary_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("tierBoundaryDescriptionData", TierBoundaryDescriptionListDataType, tier_boundary_description_data,
        &tier_boundary_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_boundary_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("boundaryId", TierBoundaryDescriptionListDataSelectorsType, boundary_id),
    EEBUS_DATA_ENUM(
        "boundaryType", TierBoundaryDescriptionListDataSelectorsType, boundary_type, tier_boundary_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg commodity_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("commodityId", CommodityDataType, commodity_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("commodityType", CommodityDataType, commodity_type, commodity_type_lut),
    EEBUS_DATA_ENUM("positiveEnergyDirection", CommodityDataType, positive_energy_direction, energy_direction_lut),
    EEBUS_DATA_STRING("label", CommodityDataType, label),
    EEBUS_DATA_STRING("description", CommodityDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg commodity_data_elements_cfg[] = {
    EEBUS_DATA_TAG("commodityId", CommodityDataElementsType, commodity_id),
    EEBUS_DATA_TAG("commodityType", CommodityDataElementsType, commodity_type),
    EEBUS_DATA_TAG("positiveEnergyDirection", CommodityDataElementsType, positive_energy_direction),
    EEBUS_DATA_TAG("label", CommodityDataElementsType, label),
    EEBUS_DATA_TAG("description", CommodityDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg commodity_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(CommodityDataType), commodity_data_cfg);

static const EebusDataCfg commodity_list_data_cfg[] = {
    EEBUS_DATA_LIST("commodityData", CommodityListDataType, commodity_data, &commodity_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg commodity_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("commodityId", CommodityListDataSelectorsType, commodity_id),
    EEBUS_DATA_ENUM("commodityType", CommodityListDataSelectorsType, commodity_type, commodity_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg active_incentive_id_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg tier_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("tierId", TierDataType, tier_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("timePeriod", TierDataType, time_period, time_period_cfg),
    EEBUS_DATA_UINT32("timeTableId", TierDataType, time_table_id),
    EEBUS_DATA_LIST("activeIncentiveId", TierDataType, active_incentive_id, &active_incentive_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_data_elements_cfg[] = {
    EEBUS_DATA_TAG("tierId", TierDataElementsType, tier_id),
    EEBUS_DATA_TAG("timePeriod", TierDataElementsType, time_period),
    EEBUS_DATA_TAG("timeTableId", TierDataElementsType, time_table_id),
    EEBUS_DATA_TAG("activeIncentiveId", TierDataElementsType, active_incentive_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TierDataType), tier_data_cfg);

static const EebusDataCfg tier_list_data_cfg[] = {
    EEBUS_DATA_LIST("tierData", TierListDataType, tier_data, &tier_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("tierId", TierListDataSelectorsType, tier_id),
    EEBUS_DATA_UINT32("activeIncentiveId", TierListDataSelectorsType, active_incentive_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_id_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg tier_incentive_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("tierId", TierIncentiveRelationDataType, tier_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_LIST("incentiveId", TierIncentiveRelationDataType, incentive_id, &incentive_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_incentive_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("tierId", TierIncentiveRelationDataElementsType, tier_id),
    EEBUS_DATA_TAG("incentiveId", TierIncentiveRelationDataElementsType, incentive_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_incentive_relation_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TierIncentiveRelationDataType), tier_incentive_relation_data_cfg);

static const EebusDataCfg tier_incentive_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("tierIncentiveRelationData", TierIncentiveRelationListDataType, tier_incentive_relation_data,
        &tier_incentive_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_incentive_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("tierId", TierIncentiveRelationListDataSelectorsType, tier_id),
    EEBUS_DATA_UINT32("incentiveId", TierIncentiveRelationListDataSelectorsType, incentive_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("tierId", TierDescriptionDataType, tier_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("tierType", TierDescriptionDataType, tier_type, tier_type_lut),
    EEBUS_DATA_STRING("label", TierDescriptionDataType, label),
    EEBUS_DATA_STRING("description", TierDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("tierId", TierDescriptionDataElementsType, tier_id),
    EEBUS_DATA_TAG("tierType", TierDescriptionDataElementsType, tier_type),
    EEBUS_DATA_TAG("label", TierDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", TierDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TierDescriptionDataType), tier_description_data_cfg);

static const EebusDataCfg tier_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("tierDescriptionData", TierDescriptionListDataType, tier_description_data,
        &tier_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg tier_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("tierId", TierDescriptionListDataSelectorsType, tier_id),
    EEBUS_DATA_ENUM("tierType", TierDescriptionListDataSelectorsType, tier_type, tier_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("incentiveId", IncentiveDataType, incentive_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("valueType", IncentiveDataType, value_type, incentive_value_type_lut),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", IncentiveDataType, timestamp),
    EEBUS_DATA_SEQUENCE("timePeriod", IncentiveDataType, time_period, time_period_cfg),
    EEBUS_DATA_UINT32("timeTableId", IncentiveDataType, time_table_id),
    EEBUS_DATA_SEQUENCE("value", IncentiveDataType, value, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_data_elements_cfg[] = {
    EEBUS_DATA_TAG("incentiveId", IncentiveDataElementsType, incentive_id),
    EEBUS_DATA_TAG("valueType", IncentiveDataElementsType, value_type),
    EEBUS_DATA_TAG("timestamp", IncentiveDataElementsType, timestamp),
    EEBUS_DATA_SEQUENCE("timePeriod", IncentiveDataElementsType, time_period, time_period_elements_cfg),
    EEBUS_DATA_TAG("timeTableId", IncentiveDataElementsType, time_table_id),
    EEBUS_DATA_SEQUENCE("value", IncentiveDataElementsType, value, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(IncentiveDataType), incentive_data_cfg);

static const EebusDataCfg incentive_list_data_cfg[] = {
    EEBUS_DATA_LIST("incentiveData", IncentiveListDataType, incentive_data, &incentive_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("incentiveId", IncentiveListDataSelectorsType, incentive_id),
    EEBUS_DATA_ENUM("valueType", IncentiveListDataSelectorsType, value_type, incentive_value_type_lut),
    EEBUS_DATA_SEQUENCE(
        "timestampInterval", IncentiveListDataSelectorsType, timestamp_interval, timestamp_interval_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("incentiveId", IncentiveDescriptionDataType, incentive_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("incentiveType", IncentiveDescriptionDataType, incentive_type, incentive_type_lut),
    EEBUS_DATA_UINT32("incentivePriority", IncentiveDescriptionDataType, incentive_priority),
    EEBUS_DATA_ENUM("currency", IncentiveDescriptionDataType, currency, currency_lut),
    EEBUS_DATA_ENUM("unit", IncentiveDescriptionDataType, unit, unit_of_measurement_lut),
    EEBUS_DATA_STRING("label", IncentiveDescriptionDataType, label),
    EEBUS_DATA_STRING("description", IncentiveDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("incentiveId", IncentiveDescriptionDataElementsType, incentive_id),
    EEBUS_DATA_TAG("incentiveType", IncentiveDescriptionDataElementsType, incentive_type),
    EEBUS_DATA_TAG("incentivePriority", IncentiveDescriptionDataElementsType, incentive_priority),
    EEBUS_DATA_TAG("currency", IncentiveDescriptionDataElementsType, currency),
    EEBUS_DATA_TAG("unit", IncentiveDescriptionDataElementsType, unit),
    EEBUS_DATA_TAG("label", IncentiveDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", IncentiveDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(IncentiveDescriptionDataType), incentive_description_data_cfg);

static const EebusDataCfg incentive_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("incentiveDescriptionData", IncentiveDescriptionListDataType, incentive_description_data,
        &incentive_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg incentive_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("incentiveId", IncentiveDescriptionListDataSelectorsType, incentive_id),
    EEBUS_DATA_ENUM("incentiveType", IncentiveDescriptionListDataSelectorsType, incentive_type, incentive_type_lut),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_TARIFF_INFORMATION_TYPES_INC_

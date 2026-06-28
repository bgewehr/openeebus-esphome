/**
 * @file
 * @brief SPINE Datagram Usecase Information payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_USECASE_INFORMATION_TYPES_INC_
#define SRC_SPINE_MODEL_USECASE_INFORMATION_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/feature_types.inc"
#include "src/spine/model/usecase_information_types.h"

static const EnumMapping use_case_actor_lut[] = {
    {"Battery", kUseCaseActorTypeBattery},
    {"BatterySystem", kUseCaseActorTypeBatterySystem},
    {"CEM", kUseCaseActorTypeCEM},
    {"ConfigurationAppliance", kUseCaseActorTypeConfigurationAppliance},
    {"Compressor", kUseCaseActorTypeCompressor},
    {"ControllableSystem", kUseCaseActorTypeControllableSystem},
    {"DHWCircuit", kUseCaseActorTypeDHWCircuit},
    {"EnergyBroker", kUseCaseActorTypeEnergyBroker},
    {"EnergyConsumer", kUseCaseActorTypeEnergyConsumer},
    {"EnergyGuard", kUseCaseActorTypeEnergyGuard},
    {"EVSE", kUseCaseActorTypeEVSE},
    {"EV", kUseCaseActorTypeEV},
    {"GridConnectionPoint", kUseCaseActorTypeGridConnectionPoint},
    {"HeatPump", kUseCaseActorTypeHeatPump},
    {"HeatingCircuit", kUseCaseActorTypeHeatingCircuit},
    {"HeatingZone", kUseCaseActorTypeHeatingZone},
    {"HVACRoom", kUseCaseActorTypeHVACRoom},
    {"Inverter", kUseCaseActorTypeInverter},
    {"MonitoredUnit", kUseCaseActorTypeMonitoredUnit},
    {"MonitoringAppliance", kUseCaseActorTypeMonitoringAppliance},
    {"OutdoorTemperatureSensor", kUseCaseActorTypeOutdoorTemperatureSensor},
    {"PVString", kUseCaseActorTypePVString},
    {"PVSystem", kUseCaseActorTypePVSystem},
    {"SmartAppliance", kUseCaseActorTypeSmartAppliance},
    {"VisualizationAppliance", kUseCaseActorTypeVisualizationAppliance},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping use_case_name_lut[] = {
    {"configurationOfDhwSystemFunction", kUseCaseNameTypeConfigurationOfDhwSystemFunction},
    {"configurationOfDhwTemperature", kUseCaseNameTypeConfigurationOfDhwTemperature},
    {"configurationOfRoomCoolingSystemFunction", kUseCaseNameTypeConfigurationOfRoomCoolingSystemFunction},
    {"configurationOfRoomCoolingTemperature", kUseCaseNameTypeConfigurationOfRoomCoolingTemperature},
    {"configurationOfRoomHeatingSystemFunction", kUseCaseNameTypeConfigurationOfRoomHeatingSystemFunction},
    {"configurationOfRoomHeatingTemperature", kUseCaseNameTypeConfigurationOfRoomHeatingTemperature},
    {"controlOfBattery", kUseCaseNameTypeControlOfBattery},
    {"coordinatedEvCharging", kUseCaseNameTypeCoordinatedEVCharging},
    {"evChargingSummary", kUseCaseNameTypeEVChargingSummary},
    {"evCommissioningAndConfiguration", kUseCaseNameTypeEVCommissioningAndConfiguration},
    {"evseCommissioningAndConfiguration", kUseCaseNameTypeEVSECommissioningAndConfiguration},
    {"evStateOfCharge", kUseCaseNameTypeEVStateOfCharge},
    {"flexibleLoad", kUseCaseNameTypeFlexibleLoad},
    {"flexibleStartForWhiteGoods", kUseCaseNameTypeFlexibleStartForWhiteGoods},
    {"limitationOfPowerConsumption", kUseCaseNameTypeLimitationOfPowerConsumption},
    {"limitationOfPowerProduction", kUseCaseNameTypeLimitationOfPowerProduction},
    {"incentiveTableBasedPowerConsumptionManagement", kUseCaseNameTypeIncentiveTableBasedPowerConsumptionManagement},
    {"measurementOfElectricityDuringEvCharging", kUseCaseNameTypeMeasurementOfElectricityDuringEVCharging},
    {"monitoringAndControlOfSmartGridReadyConditions", kUseCaseNameTypeMonitoringAndControlOfSmartGridReadyConditions},
    {"monitoringOfBattery", kUseCaseNameTypeMonitoringOfBattery},
    {"monitoringOfDhwSystemFunction", kUseCaseNameTypeMonitoringOfDhwSystemFunction},
    {"monitoringOfDhwTemperature", kUseCaseNameTypeMonitoringOfDhwTemperature},
    {"monitoringOfGridConnectionPoint", kUseCaseNameTypeMonitoringOfGridConnectionPoint},
    {"monitoringOfInverter", kUseCaseNameTypeMonitoringOfInverter},
    {"monitoringOfOutdoorTemperature", kUseCaseNameTypeMonitoringOfOutdoorTemperature},
    {"monitoringOfPowerConsumption", kUseCaseNameTypeMonitoringOfPowerConsumption},
    {"monitoringOfPvString", kUseCaseNameTypeMonitoringOfPvString},
    {"monitoringOfRoomCoolingSystemFunction", kUseCaseNameTypeMonitoringOfRoomCoolingSystemFunction},
    {"monitoringOfRoomHeatingSystemFunction", kUseCaseNameTypeMonitoringOfRoomHeatingSystemFunction},
    {"monitoringOfRoomTemperature", kUseCaseNameTypeMonitoringOfRoomTemperature},
    {"optimizationOfSelfConsumptionByHeatPumpCompressorFlexibility",
     kUseCaseNameTypeOptimizationOfSelfConsumptionByHeatPumpCompressorFlexibility},
    {"optimizationOfSelfConsumptionDuringEvCharging", kUseCaseNameTypeOptimizationOfSelfConsumptionDuringEVCharging},
    {"overloadProtectionByEvChargingCurrentCurtailment",
     kUseCaseNameTypeOverloadProtectionByEVChargingCurrentCurtailment},
    {"visualizationOfAggregatedBatteryData", kUseCaseNameTypeVisualizationOfAggregatedBatteryData},
    {"visualizationOfAggregatedPhotovoltaicData", kUseCaseNameTypeVisualizationOfAggregatedPhotovoltaicData},
    {"visualizationOfHeatingAreaName", kUseCaseNameTypeVisualizationOfHeatingAreaName},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg scenario_support_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg use_case_support_cfg[] = {
    EEBUS_DATA_ENUM("useCaseName", UseCaseSupportType, use_case_name, use_case_name_lut),
    EEBUS_DATA_STRING("useCaseVersion", UseCaseSupportType, use_case_version),
    EEBUS_DATA_BOOL("useCaseAvailable", UseCaseSupportType, use_case_available),
    EEBUS_DATA_LIST("scenarioSupport", UseCaseSupportType, scenario_support, &scenario_support_element_data_cfg),
    EEBUS_DATA_STRING("useCaseDocumentSubRevision", UseCaseSupportType, use_case_document_sub_revision),
    EEBUS_DATA_END,
};

static const EebusDataCfg use_case_support_elements_cfg[] = {
    EEBUS_DATA_TAG("useCaseName", UseCaseSupportElementsType, use_case_name),
    EEBUS_DATA_TAG("useCaseVersion", UseCaseSupportElementsType, use_case_version),
    EEBUS_DATA_TAG("useCaseAvailable", UseCaseSupportElementsType, use_case_available),
    EEBUS_DATA_TAG("scenarioSupport", UseCaseSupportElementsType, scenario_support),
    EEBUS_DATA_TAG("useCaseDocumentSubRevision", UseCaseSupportElementsType, use_case_document_sub_revision),
    EEBUS_DATA_END,
};

static const EebusDataCfg use_case_support_selectors_cfg[] = {
    EEBUS_DATA_ENUM("useCaseName", UseCaseSupportSelectorsType, use_case_name, use_case_name_lut),
    EEBUS_DATA_STRING("useCaseVersion", UseCaseSupportSelectorsType, use_case_version),
    EEBUS_DATA_UINT32("scenarioSupport", UseCaseSupportSelectorsType, scenario_support),
    EEBUS_DATA_END,
};

static const EebusDataCfg use_case_support_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(UseCaseSupportType), use_case_support_cfg);

static const EebusDataCfg use_case_information_data_cfg[] = {
    EEBUS_DATA_SEQUENCE("address", UseCaseInformationDataType, address, feature_address_cfg),
    EEBUS_DATA_ENUM("actor", UseCaseInformationDataType, actor, use_case_actor_lut),
    EEBUS_DATA_LIST("useCaseSupport", UseCaseInformationDataType, use_case_support, &use_case_support_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg use_case_information_data_elements_cfg[] = {
    EEBUS_DATA_TAG("address", UseCaseInformationDataElementsType, address),
    EEBUS_DATA_TAG("actor", UseCaseInformationDataElementsType, actor),
    EEBUS_DATA_TAG("useCaseSupport", UseCaseInformationDataElementsType, use_case_support),
    EEBUS_DATA_END,
};

static const EebusDataCfg use_case_information_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(UseCaseInformationDataType), use_case_information_data_cfg);

static const EebusDataCfg use_case_information_list_data_cfg[] = {
    EEBUS_DATA_LIST("useCaseInformationData", UseCaseInformationListDataType, use_case_information_data,
        &use_case_information_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg use_case_information_list_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("address", UseCaseInformationListDataSelectorsType, address, feature_address_cfg),
    EEBUS_DATA_ENUM("actor", UseCaseInformationListDataSelectorsType, actor, use_case_actor_lut),
    EEBUS_DATA_SEQUENCE(
        "useCaseSupport", UseCaseInformationListDataSelectorsType, use_case_support, use_case_support_selectors_cfg),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_USECASE_INFORMATION_TYPES_INC_

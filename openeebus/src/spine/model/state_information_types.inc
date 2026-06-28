/**
 * @file
 * @brief SPINE Datagram State Information payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_STATE_INFORMATION_TYPES_INC_
#define SRC_SPINE_MODEL_STATE_INFORMATION_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/state_information_types.h"

static const EnumMapping state_information_lut[] = {
    {"externalOverrideFromGrid", kStateInformationTypeExternalOverrideFromGrid},
    {"autonomousGridSupport", kStateInformationTypeAutonomousGridSupport},
    {"islandingMode", kStateInformationTypeIslandingMode},
    {"balancing", kStateInformationTypeBalancing},
    {"trickleCharging", kStateInformationTypeTrickleCharging},
    {"calibration", kStateInformationTypeCalibration},
    {"commissioningMissing", kStateInformationTypeCommissioningMissing},
    {"sleeping", kStateInformationTypeSleeping},
    {"starting", kStateInformationTypeStarting},
    {"mppt", kStateInformationTypeMppt},
    {"throttled", kStateInformationTypeThrottled},
    {"shuttingDown", kStateInformationTypeShuttingDown},
    {"manualShutdown", kStateInformationTypeManualShutdown},
    {"inverterDefective", kStateInformationTypeInverterDefective},
    {"batteryOvercurrentProtection", kStateInformationTypeBatteryOvercurrentProtection},
    {"pvStringOvercurrentProtection", kStateInformationTypePvStringOvercurrentProtection},
    {"gridFault", kStateInformationTypeGridFault},
    {"groundFault", kStateInformationTypeGroundFault},
    {"acDisconnected", kStateInformationTypeAcDisconnected},
    {"dcDisconnected", kStateInformationTypeDcDisconnected},
    {"cabinetOpen", kStateInformationTypeCabinetOpen},
    {"overTemperature", kStateInformationTypeOverTemperature},
    {"underTemperature", kStateInformationTypeUnderTemperature},
    {"frequencyAboveLimit", kStateInformationTypeFrequencyAboveLimit},
    {"frequencyBelowLimit", kStateInformationTypeFrequencyBelowLimit},
    {"acVoltageAboveLimit", kStateInformationTypeAcVoltageAboveLimit},
    {"acVoltageBelowLimit", kStateInformationTypeAcVoltageBelowLimit},
    {"dcVoltageAboveLimit", kStateInformationTypeDcVoltageAboveLimit},
    {"dcVoltageBelowLimit", kStateInformationTypeDcVoltageBelowLimit},
    {"hardwareTestFailure", kStateInformationTypeHardwareTestFailure},
    {"genericInternalError", kStateInformationTypeGenericInternalError},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping state_information_functionality_lut[] = {
    {"externalOverrideFromGrid", kStateInformationFunctionalityTypeExternalOverrideFromGrid},
    {"autonomousGridSupport", kStateInformationFunctionalityTypeAutonomousGridSupport},
    {"islandingMode", kStateInformationFunctionalityTypeIslandingMode},
    {"balancing", kStateInformationFunctionalityTypeBalancing},
    {"trickleCharging", kStateInformationFunctionalityTypeTrickleCharging},
    {"calibration", kStateInformationFunctionalityTypeCalibration},
    {"commissioningMissing", kStateInformationFunctionalityTypeCommissioningMissing},
    {"sleeping", kStateInformationFunctionalityTypeSleeping},
    {"starting", kStateInformationFunctionalityTypeStarting},
    {"mppt", kStateInformationFunctionalityTypeMppt},
    {"throttled", kStateInformationFunctionalityTypeThrottled},
    {"shuttingDown", kStateInformationFunctionalityTypeShuttingDown},
    {"manualShutdown", kStateInformationFunctionalityTypeManualShutdown},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping state_information_failure_lut[] = {
    {"inverterDefective", kStateInformationFailureTypeInverterDefective},
    {"batteryOvercurrentProtection", kStateInformationFailureTypeBatteryOvercurrentProtection},
    {"pvStringOvercurrentProtection", kStateInformationFailureTypePvStringOvercurrentProtection},
    {"gridFault", kStateInformationFailureTypeGridFault},
    {"groundFault", kStateInformationFailureTypeGroundFault},
    {"acDisconnected", kStateInformationFailureTypeAcDisconnected},
    {"dcDisconnected", kStateInformationFailureTypeDcDisconnected},
    {"cabinetOpen", kStateInformationFailureTypeCabinetOpen},
    {"overTemperature", kStateInformationFailureTypeOverTemperature},
    {"underTemperature", kStateInformationFailureTypeUnderTemperature},
    {"frequencyAboveLimit", kStateInformationFailureTypeFrequencyAboveLimit},
    {"frequencyBelowLimit", kStateInformationFailureTypeFrequencyBelowLimit},
    {"acVoltageAboveLimit", kStateInformationFailureTypeAcVoltageAboveLimit},
    {"acVoltageBelowLimit", kStateInformationFailureTypeAcVoltageBelowLimit},
    {"dcVoltageAboveLimit", kStateInformationFailureTypeDcVoltageAboveLimit},
    {"dcVoltageBelowLimit", kStateInformationFailureTypeDcVoltageBelowLimit},
    {"hardwareTestFailure", kStateInformationFailureTypeHardwareTestFailure},
    {"genericInternalError", kStateInformationFailureTypeGenericInternalError},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping state_information_category_lut[] = {
    {"functionality", kStateInformationCategoryTypeFunctionality},
    {"failure", kStateInformationCategoryTypeFailure},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg state_information_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "stateInformationId", StateInformationDataType, state_information_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("stateInformation", StateInformationDataType, state_information, state_information_lut),
    EEBUS_DATA_BOOL("isActive", StateInformationDataType, is_active),
    EEBUS_DATA_ENUM("category", StateInformationDataType, category, state_information_category_lut),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timeOfLastChange", StateInformationDataType, time_of_last_change),
    EEBUS_DATA_END,
};

static const EebusDataCfg state_information_data_elements_cfg[] = {
    EEBUS_DATA_TAG("stateInformationId", StateInformationDataElementsType, state_information_id),
    EEBUS_DATA_TAG("stateInformation", StateInformationDataElementsType, state_information),
    EEBUS_DATA_TAG("isActive", StateInformationDataElementsType, is_active),
    EEBUS_DATA_TAG("category", StateInformationDataElementsType, category),
    EEBUS_DATA_TAG("timeOfLastChange", StateInformationDataElementsType, time_of_last_change),
    EEBUS_DATA_END,
};

static const EebusDataCfg state_information_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(StateInformationDataType), state_information_data_cfg);

static const EebusDataCfg state_information_list_data_cfg[] = {
    EEBUS_DATA_LIST("stateInformationData", StateInformationListDataType, state_information_data,
        &state_information_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg state_information_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("stateInformationId", StateInformationListDataSelectorsType, state_information_id),
    EEBUS_DATA_ENUM(
        "stateInformation", StateInformationListDataSelectorsType, state_information, state_information_lut),
    EEBUS_DATA_BOOL("isActive", StateInformationListDataSelectorsType, is_active),
    EEBUS_DATA_ENUM("category", StateInformationListDataSelectorsType, category, state_information_category_lut),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_STATE_INFORMATION_TYPES_INC_

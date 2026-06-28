/**
 * @file
 * @brief SPINE Datagram Device Configuration payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_DEVICE_CONFIGURATION_TYPES_INC_
#define SRC_SPINE_MODEL_DEVICE_CONFIGURATION_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/device_configuration_types.h"

static const EnumMapping device_configuration_key_value_string_lut[] = {
    {"iso15118-2ed1", kDeviceConfigurationKeyValueStringTypeISO151182ED1},
    {"iso15118-2ed2", kDeviceConfigurationKeyValueStringTypeISO151182ED2},
    {"iec61851", kDeviceConfigurationKeyValueStringTypeIEC61851},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping device_configuration_key_name_lut[] = {
    {"peakPowerOfPvSystem", kDeviceConfigurationKeyNameTypePeakPowerOfPVSystem},
    {"pvCurtailmentLimitFactor", kDeviceConfigurationKeyNameTypePvCurtailmentLimitFactor},
    {"asymmetricChargingSupported", kDeviceConfigurationKeyNameTypeAsymmetricChargingSupported},
    {"communicationsStandard", kDeviceConfigurationKeyNameTypeCommunicationsStandard},
    {"inverterGridCode", kDeviceConfigurationKeyNameTypeInverterGridCode},
    {"pvStringAvailabilityStatus", kDeviceConfigurationKeyNameTypePvStringAvailabilityStatus},
    {"batteryAvailabilityStatus", kDeviceConfigurationKeyNameTypeBatteryAvailabilityStatus},
    {"gridConnectionStatus", kDeviceConfigurationKeyNameTypeGridConnectionStatus},
    {"timeToAcChargePowerMax", kDeviceConfigurationKeyNameTypeTimeToAcChargePowerMax},
    {"timeToAcDischargePowerMax", kDeviceConfigurationKeyNameTypeTimeToAcDischargePowerMax},
    {"tilt", kDeviceConfigurationKeyNameTypeTilt},
    {"azimuth", kDeviceConfigurationKeyNameTypeAzimuth},
    {"batteryType", kDeviceConfigurationKeyNameTypeBatteryType},
    {"maxCycleCountPerDay", kDeviceConfigurationKeyNameTypeMaxCycleCountPerDay},
    {"failsafeConsumptionActivePowerLimit", kDeviceConfigurationKeyNameTypeFailsafeConsumptionActivePowerLimit},
    {"failsafeProductionActivePowerLimit", kDeviceConfigurationKeyNameTypeFailsafeProductionActivePowerLimit},
    {"failsafePositiveReactivePowerLimit", kDeviceConfigurationKeyNameTypeFailsafePositiveReactivePowerLimit},
    {"failsafeNegativeReactivePowerLimit", kDeviceConfigurationKeyNameTypeFailsafeNegativeReactivePowerLimit},
    {"failsafePositiveCosPhiLimit", kDeviceConfigurationKeyNameTypeFailsafePositiveCosPhiLimit},
    {"failsafeNegativeCosPhiLimit", kDeviceConfigurationKeyNameTypeFailsafeNegativeCosPhiLimit},
    {"maxAcChargePower", kDeviceConfigurationKeyNameTypeMaxAcChargePower},
    {"maxAcDischargePower", kDeviceConfigurationKeyNameTypeMaxAcDischargePower},
    {"maxDcChargePower", kDeviceConfigurationKeyNameTypeMaxDcChargePower},
    {"maxDcDischargePower", kDeviceConfigurationKeyNameTypeMaxDcDischargePower},
    {"batteryActiveControlMode", kDeviceConfigurationKeyNameTypeBatteryActiveControlMode},
    {"defaultAcPower", kDeviceConfigurationKeyNameTypeDefaultAcPower},
    {"defaultDcPower", kDeviceConfigurationKeyNameTypeDefaultDcPower},
    {"defaultPccPower", kDeviceConfigurationKeyNameTypeDefaultPccPower},
    {"failsafeAcPowerSetpoint", kDeviceConfigurationKeyNameTypeFailsafeAcPowerSetpoint},
    {"failsafeDcPowerSetpoint", kDeviceConfigurationKeyNameTypeFailsafeDcPowerSetpoint},
    {"failsafePccPowerSetpoint", kDeviceConfigurationKeyNameTypeFailsafePccPowerSetpoint},
    {"failsafeDurationMinimum", kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum},
    {"dischargingBelowTargetEnergyRequestPermitted",
     kDeviceConfigurationKeyNameTypeDischargingBelowTargetEnergyRequestPermitted},
    {"incentivesSimulationCyclesMax", kDeviceConfigurationKeyNameTypeIncentivesSimulationCyclesMax},
    {"incentivesSimulationConcurrent", kDeviceConfigurationKeyNameTypeIncentivesSimulationConcurrent},
    {"incentivesTimeoutIncentiveRequest", kDeviceConfigurationKeyNameTypeIncentivesTimeoutIncentiveRequest},
    {"incentivesWaitIncentiveWriteable", kDeviceConfigurationKeyNameTypeIncentivesWaitIncentiveWriteable},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping device_configuration_key_value_type_lut[] = {
    {"boolean", kDeviceConfigurationKeyValueTypeTypeBoolean},
    {"date", kDeviceConfigurationKeyValueTypeTypeDate},
    {"dateTime", kDeviceConfigurationKeyValueTypeTypeDateTime},
    {"duration", kDeviceConfigurationKeyValueTypeTypeDuration},
    {"string", kDeviceConfigurationKeyValueTypeTypeString},
    {"time", kDeviceConfigurationKeyValueTypeTypeTime},
    {"scaledNumber", kDeviceConfigurationKeyValueTypeTypeScaledNumber},
    {"integer", kDeviceConfigurationKeyValueTypeTypeInteger},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg device_configuration_key_value_value_cfg[] = {
    EEBUS_DATA_BOOL("boolean", DeviceConfigurationKeyValueValueType, boolean),
    EEBUS_DATA_DATE("date", DeviceConfigurationKeyValueValueType, date),
    EEBUS_DATA_DATE_TIME("dateTime", DeviceConfigurationKeyValueValueType, date_time),
    EEBUS_DATA_DURATION("duration", DeviceConfigurationKeyValueValueType, duration),
    EEBUS_DATA_ENUM("string", DeviceConfigurationKeyValueValueType, string, device_configuration_key_value_string_lut),
    EEBUS_DATA_TIME("time", DeviceConfigurationKeyValueValueType, time),
    EEBUS_DATA_SEQUENCE("scaledNumber", DeviceConfigurationKeyValueValueType, scaled_number, scaled_number_cfg),
    EEBUS_DATA_INT64("integer", DeviceConfigurationKeyValueValueType, integer),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_value_elements_cfg[] = {
    EEBUS_DATA_TAG("boolean", DeviceConfigurationKeyValueValueElementsType, boolean),
    EEBUS_DATA_TAG("date", DeviceConfigurationKeyValueValueElementsType, date),
    EEBUS_DATA_TAG("dateTime", DeviceConfigurationKeyValueValueElementsType, date_time),
    EEBUS_DATA_TAG("duration", DeviceConfigurationKeyValueValueElementsType, duration),
    EEBUS_DATA_TAG("string", DeviceConfigurationKeyValueValueElementsType, string),
    EEBUS_DATA_TAG("time", DeviceConfigurationKeyValueValueElementsType, time),
    EEBUS_DATA_SEQUENCE(
        "scaledNumber", DeviceConfigurationKeyValueValueElementsType, scaled_number, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("keyId", DeviceConfigurationKeyValueDataType, key_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("value", DeviceConfigurationKeyValueDataType, value, device_configuration_key_value_value_cfg),
    EEBUS_DATA_BOOL("isValueChangeable", DeviceConfigurationKeyValueDataType, is_value_changeable),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_data_elements_cfg[] = {
    EEBUS_DATA_TAG("keyId", DeviceConfigurationKeyValueDataElementsType, key_id),
    EEBUS_DATA_SEQUENCE(
        "value", DeviceConfigurationKeyValueDataElementsType, value, device_configuration_key_value_value_elements_cfg),
    EEBUS_DATA_TAG("isValueChangeable", DeviceConfigurationKeyValueDataElementsType, is_value_changeable),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(DeviceConfigurationKeyValueDataType), device_configuration_key_value_data_cfg);

static const EebusDataCfg device_configuration_key_value_list_data_cfg[] = {
    EEBUS_DATA_LIST("deviceConfigurationKeyValueData", DeviceConfigurationKeyValueListDataType,
        device_configuration_key_value_data, &device_configuration_key_value_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("keyId", DeviceConfigurationKeyValueListDataSelectorsType, key_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "keyId", DeviceConfigurationKeyValueDescriptionDataType, key_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM(
        "keyName", DeviceConfigurationKeyValueDescriptionDataType, key_name, device_configuration_key_name_lut),
    EEBUS_DATA_ENUM("valueType", DeviceConfigurationKeyValueDescriptionDataType, value_type,
        device_configuration_key_value_type_lut),
    EEBUS_DATA_ENUM("unit", DeviceConfigurationKeyValueDescriptionDataType, unit, unit_of_measurement_lut),
    EEBUS_DATA_STRING("label", DeviceConfigurationKeyValueDescriptionDataType, label),
    EEBUS_DATA_STRING("description", DeviceConfigurationKeyValueDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("keyId", DeviceConfigurationKeyValueDescriptionDataElementsType, key_id),
    EEBUS_DATA_TAG("keyName", DeviceConfigurationKeyValueDescriptionDataElementsType, key_name),
    EEBUS_DATA_TAG("valueType", DeviceConfigurationKeyValueDescriptionDataElementsType, value_type),
    EEBUS_DATA_TAG("unit", DeviceConfigurationKeyValueDescriptionDataElementsType, unit),
    EEBUS_DATA_TAG("label", DeviceConfigurationKeyValueDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", DeviceConfigurationKeyValueDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(DeviceConfigurationKeyValueDescriptionDataType),
        device_configuration_key_value_description_data_cfg);

static const EebusDataCfg device_configuration_key_value_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("deviceConfigurationKeyValueDescriptionData", DeviceConfigurationKeyValueDescriptionListDataType,
        device_configuration_key_value_description_data,
        &device_configuration_key_value_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("keyId", DeviceConfigurationKeyValueDescriptionListDataSelectorsType, key_id),
    EEBUS_DATA_ENUM("keyName", DeviceConfigurationKeyValueDescriptionListDataSelectorsType, key_name,
        device_configuration_key_name_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "keyId", DeviceConfigurationKeyValueConstraintsDataType, key_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("valueRangeMin", DeviceConfigurationKeyValueConstraintsDataType, value_range_min,
        device_configuration_key_value_value_cfg),
    EEBUS_DATA_SEQUENCE("valueRangeMax", DeviceConfigurationKeyValueConstraintsDataType, value_range_max,
        device_configuration_key_value_value_cfg),
    EEBUS_DATA_SEQUENCE("valueStepSize", DeviceConfigurationKeyValueConstraintsDataType, value_step_size,
        device_configuration_key_value_value_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("keyId", DeviceConfigurationKeyValueConstraintsDataElementsType, key_id),
    EEBUS_DATA_SEQUENCE("valueRangeMin", DeviceConfigurationKeyValueConstraintsDataElementsType, value_range_min,
        device_configuration_key_value_value_elements_cfg),
    EEBUS_DATA_SEQUENCE("valueRangeMax", DeviceConfigurationKeyValueConstraintsDataElementsType, value_range_max,
        device_configuration_key_value_value_elements_cfg),
    EEBUS_DATA_SEQUENCE("valueStepSize", DeviceConfigurationKeyValueConstraintsDataElementsType, value_step_size,
        device_configuration_key_value_value_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_constraints_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(DeviceConfigurationKeyValueConstraintsDataType),
        device_configuration_key_value_constraints_data_cfg);

static const EebusDataCfg device_configuration_key_value_constraints_list_data_cfg[] = {
    EEBUS_DATA_LIST("deviceConfigurationKeyValueConstraintsData", DeviceConfigurationKeyValueConstraintsListDataType,
        device_configuration_key_value_constraints_data,
        &device_configuration_key_value_constraints_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_configuration_key_value_constraints_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("keyId", DeviceConfigurationKeyValueConstraintsListDataSelectorsType, key_id),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_DEVICE_CONFIGURATION_TYPES_INC_

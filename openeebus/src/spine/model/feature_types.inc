/**
 * @file
 * @brief SPINE Datagram Feature payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_FEATURE_TYPES_INC_
#define SRC_SPINE_MODEL_FEATURE_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/feature_types.h"

static const EebusDataCfg entity_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg feature_address_cfg[] = {
    EEBUS_DATA_STRING("device", FeatureAddressType, device),
    EEBUS_DATA_LIST("entity", FeatureAddressType, entity, &entity_element_data_cfg),
    EEBUS_DATA_UINT32("feature", FeatureAddressType, feature),
    EEBUS_DATA_END,
};

static const EebusDataCfg feature_address_elements_cfg[] = {
    EEBUS_DATA_TAG("device", FeatureAddressElementsType, device),
    EEBUS_DATA_TAG("entity", FeatureAddressElementsType, entity),
    EEBUS_DATA_TAG("feature", FeatureAddressElementsType, feature),
    EEBUS_DATA_END,
};

static const EnumMapping role_lut[] = {
    {"client", kRoleTypeClient},
    {"server", kRoleTypeServer},
    {"special", kRoleTypeSpecial},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping feature_type_lut[] = {
    {"ActuatorLevel", kFeatureTypeTypeActuatorLevel},
    {"ActuatorSwitch", kFeatureTypeTypeActuatorSwitch},
    {"Alarm", kFeatureTypeTypeAlarm},
    {"DataTunneling", kFeatureTypeTypeDataTunneling},
    {"DeviceClassification", kFeatureTypeTypeDeviceClassification},
    {"DeviceDiagnosis", kFeatureTypeTypeDeviceDiagnosis},
    {"DirectControl", kFeatureTypeTypeDirectControl},
    {"ElectricalConnection", kFeatureTypeTypeElectricalConnection},
    {"Generic", kFeatureTypeTypeGeneric},
    {"HVAC", kFeatureTypeTypeHvac},
    {"LoadControl", kFeatureTypeTypeLoadControl},
    {"Measurement", kFeatureTypeTypeMeasurement},
    {"Messaging", kFeatureTypeTypeMessaging},
    {"NetworkManagement", kFeatureTypeTypeNetworkManagement},
    {"NodeManagement", kFeatureTypeTypeNodeManagement},
    {"OperatingConstraints", kFeatureTypeTypeOperatingConstraints},
    {"PowerSequences", kFeatureTypeTypePowerSequences},
    {"Sensing", kFeatureTypeTypeSensing},
    {"Setpoint", kFeatureTypeTypeSetpoint},
    {"SmartEnergyManagementPs", kFeatureTypeTypeSmartEnergyManagementPs},
    {"TaskManagement", kFeatureTypeTypeTaskManagement},
    {"Threshold", kFeatureTypeTypeThreshold},
    {"TimeInformation", kFeatureTypeTypeTimeInformation},
    {"TimeTable", kFeatureTypeTypeTimeTable},
    {"DeviceConfiguration", kFeatureTypeTypeDeviceConfiguration},
    {"SupplyCondition", kFeatureTypeTypeSupplyCondition},
    {"TimeSeries", kFeatureTypeTypeTimeSeries},
    {"TariffInformation", kFeatureTypeTypeTariffInformation},
    {"IncentiveTable", kFeatureTypeTypeIncentiveTable},
    {"Bill", kFeatureTypeTypeBill},
    {"Identification", kFeatureTypeTypeIdentification},
    {"StateInformation", kFeatureTypeTypeStateInformation},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping feature_specific_usage_lut[] = {
    {"History", FeatureSpecificUsageTypeHistory},
    {"RealTime", FeatureSpecificUsageTypeRealtime},
    {"OperationMode", FeatureSpecificUsageTypeOperationmode},
    {"Overrun", FeatureSpecificUsageTypeOverrun},
    {"Contact", FeatureSpecificUsageTypeContact},
    {"Electrical", FeatureSpecificUsageTypeElectrical},
    {"Heat", FeatureSpecificUsageTypeHeat},
    {"Level", FeatureSpecificUsageTypeLevel},
    {"Pressure", FeatureSpecificUsageTypePressure},
    {"Temperature", FeatureSpecificUsageTypeTemperature},
    {"FixedForecast", FeatureSpecificUsageTypeFixedForecast},
    {"FlexibleChosenForecast", FeatureSpecificUsageTypeFlexibleChosenForecast},
    {"FlexibleOptionalForecast", FeatureSpecificUsageTypeFlexibleOptionalForecast},
    {"OptionalSequenceBasedImmediateControl", FeatureSpecificUsageTypeOptionalSequenceBasedImmediateControl},
    EEBUS_ENUM_MAPPING_END,
};

#endif  // SRC_SPINE_MODEL_FEATURE_TYPES_INC_

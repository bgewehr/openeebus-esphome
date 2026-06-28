/**
 * @file
 * @brief SPINE Datagram Device Diagnosis payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_DEVICE_DIAGNOSIS_TYPES_INC_
#define SRC_SPINE_MODEL_DEVICE_DIAGNOSIS_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/device_diagnosis_types.h"

static const EnumMapping device_diagnosis_operating_state_lut[] = {
    {"normalOperation", kDeviceDiagnosisOperatingStateTypeNormalOperation},
    {"standby", kDeviceDiagnosisOperatingStateTypeStandby},
    {"failure", kDeviceDiagnosisOperatingStateTypeFailure},
    {"serviceNeeded", kDeviceDiagnosisOperatingStateTypeServiceNeeded},
    {"overrideDetected", kDeviceDiagnosisOperatingStateTypeOverrideDetected},
    {"inAlarm", kDeviceDiagnosisOperatingStateTypeInAlarm},
    {"notReachable", kDeviceDiagnosisOperatingStateTypeNotReachable},
    {"finished", kDeviceDiagnosisOperatingStateTypeFinished},
    {"temporarilyNotReady", kDeviceDiagnosisOperatingStateTypeTemporarilyNotReady},
    {"off", kDeviceDiagnosisOperatingStateTypeOff},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping power_supply_condition_lut[] = {
    {"good", kPowerSupplyConditionTypeGood},
    {"low", kPowerSupplyConditionTypeLow},
    {"critical", kPowerSupplyConditionTypeCritical},
    {"unknown", kPowerSupplyConditionTypeUnknown},
    {"error", kPowerSupplyConditionTypeError},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg device_diagnosis_state_data_cfg[] = {
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", DeviceDiagnosisStateDataType, timestamp),
    EEBUS_DATA_ENUM(
        "operatingState", DeviceDiagnosisStateDataType, operating_state, device_diagnosis_operating_state_lut),
    EEBUS_DATA_STRING("vendorStateCode", DeviceDiagnosisStateDataType, vendor_state_code),
    EEBUS_DATA_STRING("lastErrorCode", DeviceDiagnosisStateDataType, last_error_code),
    EEBUS_DATA_DURATION("upTime", DeviceDiagnosisStateDataType, up_time),
    EEBUS_DATA_DURATION("totalUpTime", DeviceDiagnosisStateDataType, total_up_time),
    EEBUS_DATA_ENUM(
        "powerSupplyCondition", DeviceDiagnosisStateDataType, power_supply_condition, power_supply_condition_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_diagnosis_state_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timestamp", DeviceDiagnosisStateDataElementsType, timestamp),
    EEBUS_DATA_TAG("operatingState", DeviceDiagnosisStateDataElementsType, operating_state),
    EEBUS_DATA_TAG("vendorStateCode", DeviceDiagnosisStateDataElementsType, vendor_state_code),
    EEBUS_DATA_TAG("lastErrorCode", DeviceDiagnosisStateDataElementsType, last_error_code),
    EEBUS_DATA_TAG("upTime", DeviceDiagnosisStateDataElementsType, up_time),
    EEBUS_DATA_TAG("totalUpTime", DeviceDiagnosisStateDataElementsType, total_up_time),
    EEBUS_DATA_TAG("powerSupplyCondition", DeviceDiagnosisStateDataElementsType, power_supply_condition),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_diagnosis_heartbeat_data_cfg[] = {
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", DeviceDiagnosisHeartbeatDataType, timestamp),
    EEBUS_DATA_UINT64("heartbeatCounter", DeviceDiagnosisHeartbeatDataType, heartbeat_counter),
    EEBUS_DATA_DURATION("heartbeatTimeout", DeviceDiagnosisHeartbeatDataType, heartbeat_timeout),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_diagnosis_heartbeat_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timestamp", DeviceDiagnosisHeartbeatDataElementsType, timestamp),
    EEBUS_DATA_TAG("heartbeatCounter", DeviceDiagnosisHeartbeatDataElementsType, heartbeat_counter),
    EEBUS_DATA_TAG("heartbeatTimeout", DeviceDiagnosisHeartbeatDataElementsType, heartbeat_timeout),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_diagnosis_service_data_cfg[] = {
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", DeviceDiagnosisServiceDataType, timestamp),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("installationTime", DeviceDiagnosisServiceDataType, installation_time),
    EEBUS_DATA_UINT64("bootCounter", DeviceDiagnosisServiceDataType, boot_counter),
    EEBUS_DATA_STRING("nextService", DeviceDiagnosisServiceDataType, next_service),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_diagnosis_service_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timestamp", DeviceDiagnosisServiceDataElementsType, timestamp),
    EEBUS_DATA_TAG("installationTime", DeviceDiagnosisServiceDataElementsType, installation_time),
    EEBUS_DATA_TAG("bootCounter", DeviceDiagnosisServiceDataElementsType, boot_counter),
    EEBUS_DATA_TAG("nextService", DeviceDiagnosisServiceDataElementsType, next_service),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_DEVICE_DIAGNOSIS_TYPES_INC_

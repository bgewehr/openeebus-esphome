/**
 * @file
 * @brief SPINE Datagram Device Classification payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_DEVICE_CLASSIFICATION_TYPES_INC_
#define SRC_SPINE_MODEL_DEVICE_CLASSIFICATION_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/device_classification_types.h"

static const EnumMapping power_source_lut[] = {
    {"unknown", kPowerSourceTypeUnknown},
    {"mainsSinglePhase", kPowerSourceTypeMainssinglephase},
    {"mains3Phase", kPowerSourceTypeMains3Phase},
    {"battery", kPowerSourceTypeBattery},
    {"dc", kPowerSourceTypeDc},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg device_classification_manufacturer_data_cfg[] = {
    EEBUS_DATA_STRING("deviceName", DeviceClassificationManufacturerDataType, device_name),
    EEBUS_DATA_STRING("deviceCode", DeviceClassificationManufacturerDataType, device_code),
    EEBUS_DATA_STRING("serialNumber", DeviceClassificationManufacturerDataType, serial_number),
    EEBUS_DATA_STRING("softwareRevision", DeviceClassificationManufacturerDataType, software_revision),
    EEBUS_DATA_STRING("hardwareRevision", DeviceClassificationManufacturerDataType, hardware_revision),
    EEBUS_DATA_STRING("vendorName", DeviceClassificationManufacturerDataType, vendor_name),
    EEBUS_DATA_STRING("vendorCode", DeviceClassificationManufacturerDataType, vendor_code),
    EEBUS_DATA_STRING("brandName", DeviceClassificationManufacturerDataType, brand_name),
    EEBUS_DATA_ENUM("powerSource", DeviceClassificationManufacturerDataType, power_source, power_source_lut),
    EEBUS_DATA_STRING(
        "manufacturerNodeIdentification", DeviceClassificationManufacturerDataType, manufacturer_node_identification),
    EEBUS_DATA_STRING("manufacturerLabel", DeviceClassificationManufacturerDataType, manufacturer_label),
    EEBUS_DATA_STRING("manufacturerDescription", DeviceClassificationManufacturerDataType, manufacturer_description),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_classification_manufacturer_data_elements_cfg[] = {
    EEBUS_DATA_TAG("deviceName", DeviceClassificationManufacturerDataElementsType, device_name),
    EEBUS_DATA_TAG("deviceCode", DeviceClassificationManufacturerDataElementsType, device_code),
    EEBUS_DATA_TAG("serialNumber", DeviceClassificationManufacturerDataElementsType, serial_number),
    EEBUS_DATA_TAG("softwareRevision", DeviceClassificationManufacturerDataElementsType, software_revision),
    EEBUS_DATA_TAG("hardwareRevision", DeviceClassificationManufacturerDataElementsType, hardware_revision),
    EEBUS_DATA_TAG("vendorName", DeviceClassificationManufacturerDataElementsType, vendor_name),
    EEBUS_DATA_TAG("vendorCode", DeviceClassificationManufacturerDataElementsType, vendor_code),
    EEBUS_DATA_TAG("brandName", DeviceClassificationManufacturerDataElementsType, brand_name),
    EEBUS_DATA_TAG("powerSource", DeviceClassificationManufacturerDataElementsType, power_source),
    EEBUS_DATA_TAG("manufacturerNodeIdentification", DeviceClassificationManufacturerDataElementsType,
        manufacturer_node_identification),
    EEBUS_DATA_TAG("manufacturerLabel", DeviceClassificationManufacturerDataElementsType, manufacturer_label),
    EEBUS_DATA_TAG(
        "manufacturerDescription", DeviceClassificationManufacturerDataElementsType, manufacturer_description),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_classification_user_data_cfg[] = {
    EEBUS_DATA_STRING("userNodeIdentification", DeviceClassificationUserDataType, user_node_identification),
    EEBUS_DATA_STRING("userLabel", DeviceClassificationUserDataType, user_label),
    EEBUS_DATA_STRING("userDescription", DeviceClassificationUserDataType, user_description),
    EEBUS_DATA_END,
};

static const EebusDataCfg device_classification_user_data_elements_cfg[] = {
    EEBUS_DATA_TAG("userNodeIdentification", DeviceClassificationUserDataElementsType, user_node_identification),
    EEBUS_DATA_TAG("userLabel", DeviceClassificationUserDataElementsType, user_label),
    EEBUS_DATA_TAG("userDescription", DeviceClassificationUserDataElementsType, user_description),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_DEVICE_CLASSIFICATION_TYPES_INC_

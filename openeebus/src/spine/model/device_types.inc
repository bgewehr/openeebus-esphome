/**
 * @file
 * @brief SPINE Datagram Device payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_DEVICE_TYPES_INC_
#define SRC_SPINE_MODEL_DEVICE_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/device_types.h"

static const EebusDataCfg device_address_cfg[] = {
    EEBUS_DATA_STRING("device", DeviceAddressType, device),
    EEBUS_DATA_END,
};

static const EnumMapping device_type_lut[] = {
    {"Dishwasher", kDeviceTypeTypeDishwasher},
    {"Dryer", kDeviceTypeTypeDryer},
    {"EnvironmentSensor", kDeviceTypeTypeEnvironmentSensor},
    {"Generic", kDeviceTypeTypeGeneric},
    {"HeatGenerationSystem", kDeviceTypeTypeHeatgenerationSystem},
    {"HeatSinkSystem", kDeviceTypeTypeHeatsinkSystem},
    {"HeatStorageSystem", kDeviceTypeTypeHeatstorageSystem},
    {"HVACController", kDeviceTypeTypeHVACController},
    {"SubMeter", kDeviceTypeTypeSubmeter},
    {"Washer", kDeviceTypeTypeWasher},
    {"ElectricitySupplySystem", kDeviceTypeTypeElectricitySupplySystem},
    {"EnergyManagementSystem", kDeviceTypeTypeEnergyManagementSystem},
    {"Inverter", kDeviceTypeTypeInverter},
    {"ChargingStation", kDeviceTypeTypeChargingStation},
    EEBUS_ENUM_MAPPING_END,
};

#endif  // SRC_SPINE_MODEL_DEVICE_TYPES_INC_

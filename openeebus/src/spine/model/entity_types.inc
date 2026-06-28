/**
 * @file
 * @brief SPINE Datagram Entity payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_ENTITY_TYPES_INC_
#define SRC_SPINE_MODEL_ENTITY_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/entity_types.h"
#include "src/spine/model/feature_types.inc"

static const EebusDataCfg entity_address_cfg[] = {
    EEBUS_DATA_STRING("device", EntityAddressType, device),
    EEBUS_DATA_LIST("entity", EntityAddressType, entity, &entity_element_data_cfg),
    EEBUS_DATA_END,
};

static const EnumMapping entity_type_lut[] = {
    {"Battery", kEntityTypeTypeBattery},
    {"Compressor", kEntityTypeTypeCompressor},
    {"DeviceInformation", kEntityTypeTypeDeviceInformation},
    {"DHWCircuit", kEntityTypeTypeDHWCircuit},
    {"DHWStorage", kEntityTypeTypeDHWStorage},
    {"Dishwasher", kEntityTypeTypeDishwasher},
    {"Dryer", kEntityTypeTypeDryer},
    {"ElectricalImmersionHeater", kEntityTypeTypeElectricalImmersionHeater},
    {"Fan", kEntityTypeTypeFan},
    {"GasHeatingAppliance", kEntityTypeTypeGasHeatingAppliance},
    {"Generic", kEntityTypeTypeGeneric},
    {"HeatingBufferStorage", kEntityTypeTypeHeatingBufferStorage},
    {"HeatingCircuit", kEntityTypeTypeHeatingCircuit},
    {"HeatingObject", kEntityTypeTypeHeatingObject},
    {"HeatingZone", kEntityTypeTypeHeatingZone},
    {"HeatPumpAppliance", kEntityTypeTypeHeatPumpAppliance},
    {"HeatSinkCircuit", kEntityTypeTypeHeatSinkCircuit},
    {"HeatSourceCircuit", kEntityTypeTypeHeatSourceCircuit},
    {"HeatSourceUnit", kEntityTypeTypeHeatSourceUnit},
    {"HVACController", kEntityTypeTypeHvacController},
    {"HVACRoom", kEntityTypeTypeHvacRoom},
    {"InstantDHWHeater", kEntityTypeTypeInstantDHWheater},
    {"Inverter", kEntityTypeTypeInverter},
    {"OilHeatingAppliance", kEntityTypeTypeOilHeatingAppliance},
    {"Pump", kEntityTypeTypePump},
    {"RefrigerantCircuit", kEntityTypeTypeRefrigerantCircuit},
    {"SmartEnergyAppliance", kEntityTypeTypeSmartEnergyAppliance},
    {"SolarDHWStorage", kEntityTypeTypeSolarDHWStorage},
    {"SolarThermalCircuit", kEntityTypeTypeSolarThermalCircuit},
    {"SubMeterElectricity", kEntityTypeTypeSubMeterElectricity},
    {"TemperatureSensor", kEntityTypeTypeTemperatureSensor},
    {"Washer", kEntityTypeTypeWasher},
    {"BatterySystem", kEntityTypeTypeBatterySystem},
    {"ElectricityGenerationSystem", kEntityTypeTypeElectricityGenerationSystem},
    {"ElectricityStorageSystem", kEntityTypeTypeElectricityStorageSystem},
    {"GridConnectionPointOfPremises", kEntityTypeTypeGridConnectionPointOfPremises},
    {"Household", kEntityTypeTypeHousehold},
    {"PVSystem", kEntityTypeTypePVSystem},
    {"EV", kEntityTypeTypeEV},
    {"EVSE", kEntityTypeTypeEVSE},
    {"ChargingOutlet", kEntityTypeTypeChargingOutlet},
    {"CEM", kEntityTypeTypeCEM},
    {"PV", kEntityTypeTypePV},
    {"PVESHybrid", kEntityTypeTypePVESHybrid},
    {"ElectricalStorage", kEntityTypeTypeElectricalStorage},
    {"PVString", kEntityTypeTypePVString},
    {"GridGuard", kEntityTypeTypeGridGuard},
    {"ControllableSystem", kEntityTypeTypeControllableSystem},
    EEBUS_ENUM_MAPPING_END,
};

#endif  // SRC_SPINE_MODEL_ENTITY_TYPES_INC_

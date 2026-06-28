/**
 * @file
 * @brief SPINE Datagram Electrical Connection payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_ELECTRICAL_CONNECTION_TYPES_INC_
#define SRC_SPINE_MODEL_ELECTRICAL_CONNECTION_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/electrical_connection_types.h"
#include "src/spine/model/measurement_types.inc"

static const EnumMapping electrical_connection_measurand_variant_lut[] = {
    {"amplitude", kElectricalConnectionMeasurandVariantTypeAmplitude},
    {"rms", kElectricalConnectionMeasurandVariantTypeRms},
    {"instantaneous", kElectricalConnectionMeasurandVariantTypeInstantaneous},
    {"angle", kElectricalConnectionMeasurandVariantTypeAngle},
    {"cosPhi", kElectricalConnectionMeasurandVariantTypeCosphi},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping electrical_connection_voltage_type_lut[] = {
    {"ac", kElectricalConnectionVoltageTypeTypeAc},
    {"dc", kElectricalConnectionVoltageTypeTypeDc},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping electrical_connection_ac_measurement_type_lut[] = {
    {"real", kElectricalConnectionAcMeasurementTypeTypeReal},
    {"reactive", kElectricalConnectionAcMeasurementTypeTypeReactive},
    {"apparent", kElectricalConnectionAcMeasurementTypeTypeApparent},
    {"phase", kElectricalConnectionAcMeasurementTypeTypePhase},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping electrical_connection_phase_name_lut[] = {
    {"a", kElectricalConnectionPhaseNameTypeA},
    {"b", kElectricalConnectionPhaseNameTypeB},
    {"c", kElectricalConnectionPhaseNameTypeC},
    {"ab", kElectricalConnectionPhaseNameTypeAb},
    {"bc", kElectricalConnectionPhaseNameTypeBc},
    {"ac", kElectricalConnectionPhaseNameTypeAc},
    {"abc", kElectricalConnectionPhaseNameTypeAbc},
    {"neutral", kElectricalConnectionPhaseNameTypeNeutral},
    {"ground", kElectricalConnectionPhaseNameTypeGround},
    {"none", kElectricalConnectionPhaseNameTypeNone},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping electrical_connection_connection_point_lut[] = {
    {"grid", kElectricalConnectionConnectionPointTypeGrid},
    {"home", kElectricalConnectionConnectionPointTypeHome},
    {"pv", kElectricalConnectionConnectionPointTypePv},
    {"sd", kElectricalConnectionConnectionPointTypeSd},
    {"other", kElectricalConnectionConnectionPointTypeOther},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping electrical_connection_characteristic_context_lut[] = {
    {"device", kElectricalConnectionCharacteristicContextTypeDevice},
    {"entity", kElectricalConnectionCharacteristicContextTypeEntity},
    {"inverter", kElectricalConnectionCharacteristicContextTypeInverter},
    {"pvString", kElectricalConnectionCharacteristicContextTypePvString},
    {"battery", kElectricalConnectionCharacteristicContextTypeBattery},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping electrical_connection_characteristic_type_lut[] = {
    {"powerConsumptionMin", kElectricalConnectionCharacteristicTypeTypePowerConsumptionMin},
    {"powerConsumptionMax", kElectricalConnectionCharacteristicTypeTypePowerConsumptionMax},
    {"powerConsumptionNominalMin", kElectricalConnectionCharacteristicTypeTypePowerConsumptionNominalMin},
    {"powerConsumptionNominalMax", kElectricalConnectionCharacteristicTypeTypePowerConsumptionNominalMax},
    {"powerProductionMin", kElectricalConnectionCharacteristicTypeTypePowerProductionMin},
    {"powerProductionMax", kElectricalConnectionCharacteristicTypeTypePowerProductionMax},
    {"powerProductionNominalMin", kElectricalConnectionCharacteristicTypeTypePowerProductionNominalMin},
    {"powerProductionNominalMax", kElectricalConnectionCharacteristicTypeTypePowerProductionNominalMax},
    {"energyCapacityNominalMax", kElectricalConnectionCharacteristicTypeTypeEnergyCapacityNominalMax},
    {"contractualConsumptionNominalMax", kElectricalConnectionCharacteristicTypeTypeContractualConsumptionNominalMax},
    {"contractualProductionNominalMax", kElectricalConnectionCharacteristicTypeTypeContractualProductionNominalMax},
    {"apparentPowerProductionNominalMax", kElectricalConnectionCharacteristicTypeTypeApparentPowerProductionNominalMax},
    {"apparentPowerConsumptionNominalMax",
     kElectricalConnectionCharacteristicTypeTypeApparentPowerConsumptionNominalMax},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg electrical_connection_parameter_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("electricalConnectionId", ElectricalConnectionParameterDescriptionDataType,
        electrical_connection_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "parameterId", ElectricalConnectionParameterDescriptionDataType, parameter_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("measurementId", ElectricalConnectionParameterDescriptionDataType, measurement_id),
    EEBUS_DATA_ENUM("voltageType", ElectricalConnectionParameterDescriptionDataType, voltage_type,
        electrical_connection_voltage_type_lut),
    EEBUS_DATA_ENUM("acMeasuredPhases", ElectricalConnectionParameterDescriptionDataType, ac_measured_phases,
        electrical_connection_phase_name_lut),
    EEBUS_DATA_ENUM("acMeasuredInReferenceTo", ElectricalConnectionParameterDescriptionDataType,
        ac_measured_in_reference_to, electrical_connection_phase_name_lut),
    EEBUS_DATA_ENUM("acMeasurementType", ElectricalConnectionParameterDescriptionDataType, ac_measurement_type,
        electrical_connection_ac_measurement_type_lut),
    EEBUS_DATA_ENUM("acMeasurementVariant", ElectricalConnectionParameterDescriptionDataType, ac_measurement_variant,
        electrical_connection_measurand_variant_lut),
    EEBUS_DATA_UINT8("acMeasuredHarmonic", ElectricalConnectionParameterDescriptionDataType, ac_measured_harmonic),
    EEBUS_DATA_ENUM("scopeType", ElectricalConnectionParameterDescriptionDataType, scope_type, scope_type_lut),
    EEBUS_DATA_STRING("label", ElectricalConnectionParameterDescriptionDataType, label),
    EEBUS_DATA_STRING("description", ElectricalConnectionParameterDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_parameter_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG(
        "electricalConnectionId", ElectricalConnectionParameterDescriptionDataElementsType, electrical_connection_id),
    EEBUS_DATA_TAG("parameterId", ElectricalConnectionParameterDescriptionDataElementsType, parameter_id),
    EEBUS_DATA_TAG("measurementId", ElectricalConnectionParameterDescriptionDataElementsType, measurement_id),
    EEBUS_DATA_TAG("voltageType", ElectricalConnectionParameterDescriptionDataElementsType, voltage_type),
    EEBUS_DATA_TAG("acMeasuredPhases", ElectricalConnectionParameterDescriptionDataElementsType, ac_measured_phases),
    EEBUS_DATA_TAG("acMeasuredInReferenceTo", ElectricalConnectionParameterDescriptionDataElementsType,
        ac_measured_in_reference_to),
    EEBUS_DATA_TAG("acMeasurementType", ElectricalConnectionParameterDescriptionDataElementsType, ac_measurement_type),
    EEBUS_DATA_TAG(
        "acMeasurementVariant", ElectricalConnectionParameterDescriptionDataElementsType, ac_measurement_variant),
    EEBUS_DATA_TAG(
        "acMeasuredHarmonic", ElectricalConnectionParameterDescriptionDataElementsType, ac_measured_harmonic),
    EEBUS_DATA_TAG("scopeType", ElectricalConnectionParameterDescriptionDataElementsType, scope_type),
    EEBUS_DATA_TAG("label", ElectricalConnectionParameterDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", ElectricalConnectionParameterDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_parameter_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(ElectricalConnectionParameterDescriptionDataType),
        electrical_connection_parameter_description_data_cfg);

static const EebusDataCfg electrical_connection_parameter_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("electricalConnectionParameterDescriptionData",
        ElectricalConnectionParameterDescriptionListDataType, electrical_connection_parameter_description_data,
        &electrical_connection_parameter_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_parameter_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("electricalConnectionId", ElectricalConnectionParameterDescriptionListDataSelectorsType,
        electrical_connection_id),
    EEBUS_DATA_UINT32("parameterId", ElectricalConnectionParameterDescriptionListDataSelectorsType, parameter_id),
    EEBUS_DATA_UINT32("measurementId", ElectricalConnectionParameterDescriptionListDataSelectorsType, measurement_id),
    EEBUS_DATA_ENUM(
        "scopeType", ElectricalConnectionParameterDescriptionListDataSelectorsType, scope_type, scope_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg permitted_value_set_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(ScaledNumberSetType), scaled_number_set_cfg);

static const EebusDataCfg electrical_connection_permitted_value_set_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("electricalConnectionId", ElectricalConnectionPermittedValueSetDataType,
        electrical_connection_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "parameterId", ElectricalConnectionPermittedValueSetDataType, parameter_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_LIST("permittedValueSet", ElectricalConnectionPermittedValueSetDataType, permitted_value_set,
        &permitted_value_set_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_permitted_value_set_data_elements_cfg[] = {
    EEBUS_DATA_TAG(
        "electricalConnectionId", ElectricalConnectionPermittedValueSetDataElementsType, electrical_connection_id),
    EEBUS_DATA_TAG("parameterId", ElectricalConnectionPermittedValueSetDataElementsType, parameter_id),
    EEBUS_DATA_TAG("permittedValueSet", ElectricalConnectionPermittedValueSetDataElementsType, permitted_value_set),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_permitted_value_set_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(ElectricalConnectionPermittedValueSetDataType),
        electrical_connection_permitted_value_set_data_cfg);

static const EebusDataCfg electrical_connection_permitted_value_set_list_data_cfg[] = {
    EEBUS_DATA_LIST("electricalConnectionPermittedValueSetData", ElectricalConnectionPermittedValueSetListDataType,
        electrical_connection_permitted_value_set_data,
        &electrical_connection_permitted_value_set_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_permitted_value_set_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32(
        "electricalConnectionId", ElectricalConnectionPermittedValueSetListDataSelectorsType, electrical_connection_id),
    EEBUS_DATA_UINT32("parameterId", ElectricalConnectionPermittedValueSetListDataSelectorsType, parameter_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_state_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("electricalConnectionId", ElectricalConnectionStateDataType, electrical_connection_id,
        kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", ElectricalConnectionStateDataType, timestamp),
    EEBUS_DATA_ENUM("currentEnergyMode", ElectricalConnectionStateDataType, current_energy_mode, energy_mode_lut),
    EEBUS_DATA_DURATION("consumptionTime", ElectricalConnectionStateDataType, consumption_time),
    EEBUS_DATA_DURATION("productionTime", ElectricalConnectionStateDataType, production_time),
    EEBUS_DATA_DURATION("totalConsumptionTime", ElectricalConnectionStateDataType, total_consumption_time),
    EEBUS_DATA_DURATION("totalProductionTime", ElectricalConnectionStateDataType, total_production_time),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_state_data_elements_cfg[] = {
    EEBUS_DATA_TAG("electricalConnectionId", ElectricalConnectionStateDataElementsType, electrical_connection_id),
    EEBUS_DATA_TAG("timestamp", ElectricalConnectionStateDataElementsType, timestamp),
    EEBUS_DATA_TAG("currentEnergyMode", ElectricalConnectionStateDataElementsType, current_energy_mode),
    EEBUS_DATA_TAG("consumptionTime", ElectricalConnectionStateDataElementsType, consumption_time),
    EEBUS_DATA_TAG("productionTime", ElectricalConnectionStateDataElementsType, production_time),
    EEBUS_DATA_TAG("totalConsumptionTime", ElectricalConnectionStateDataElementsType, total_consumption_time),
    EEBUS_DATA_TAG("totalProductionTime", ElectricalConnectionStateDataElementsType, total_production_time),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_state_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(ElectricalConnectionStateDataType), electrical_connection_state_data_cfg);

static const EebusDataCfg electrical_connection_state_list_data_cfg[] = {
    EEBUS_DATA_LIST("electricalConnectionStateData", ElectricalConnectionStateListDataType,
        electrical_connection_state_data, &electrical_connection_state_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_state_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32(
        "electricalConnectionId", ElectricalConnectionStateListDataSelectorsType, electrical_connection_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("electricalConnectionId", ElectricalConnectionDescriptionDataType,
        electrical_connection_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("powerSupplyType", ElectricalConnectionDescriptionDataType, power_supply_type,
        electrical_connection_voltage_type_lut),
    EEBUS_DATA_ENUM("acConnectedPhases", ElectricalConnectionDescriptionDataType, ac_connected_phases,
        electrical_connection_phase_name_lut),
    EEBUS_DATA_DURATION("acRmsPeriodDuration", ElectricalConnectionDescriptionDataType, ac_rms_period_duration),
    EEBUS_DATA_ENUM("positiveEnergyDirection", ElectricalConnectionDescriptionDataType, positive_energy_direction,
        energy_direction_lut),
    EEBUS_DATA_ENUM("scopeType", ElectricalConnectionDescriptionDataType, scope_type, scope_type_lut),
    EEBUS_DATA_STRING("label", ElectricalConnectionDescriptionDataType, label),
    EEBUS_DATA_STRING("description", ElectricalConnectionDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("electricalConnectionId", ElectricalConnectionDescriptionDataElementsType, electrical_connection_id),
    EEBUS_DATA_TAG("powerSupplyType", ElectricalConnectionDescriptionDataElementsType, power_supply_type),
    EEBUS_DATA_TAG("acConnectedPhases", ElectricalConnectionDescriptionDataElementsType, ac_connected_phases),
    EEBUS_DATA_TAG("acRmsPeriodDuration", ElectricalConnectionDescriptionDataElementsType, ac_rms_period_duration),
    EEBUS_DATA_TAG(
        "positiveEnergyDirection", ElectricalConnectionDescriptionDataElementsType, positive_energy_direction),
    EEBUS_DATA_TAG("scopeType", ElectricalConnectionDescriptionDataElementsType, scope_type),
    EEBUS_DATA_TAG("label", ElectricalConnectionDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", ElectricalConnectionDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_description_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(ElectricalConnectionDescriptionDataType), electrical_connection_description_data_cfg);

static const EebusDataCfg electrical_connection_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("electricalConnectionDescriptionData", ElectricalConnectionDescriptionListDataType,
        electrical_connection_description_data, &electrical_connection_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32(
        "electricalConnectionId", ElectricalConnectionDescriptionListDataSelectorsType, electrical_connection_id),
    EEBUS_DATA_ENUM("scopeType", ElectricalConnectionDescriptionListDataSelectorsType, scope_type, scope_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_characteristic_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("electricalConnectionId", ElectricalConnectionCharacteristicDataType,
        electrical_connection_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "parameterId", ElectricalConnectionCharacteristicDataType, parameter_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "characteristicId", ElectricalConnectionCharacteristicDataType, characteristic_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("characteristicContext", ElectricalConnectionCharacteristicDataType, characteristic_context,
        electrical_connection_characteristic_context_lut),
    EEBUS_DATA_ENUM("characteristicType", ElectricalConnectionCharacteristicDataType, characteristic_type,
        electrical_connection_characteristic_type_lut),
    EEBUS_DATA_SEQUENCE("value", ElectricalConnectionCharacteristicDataType, value, scaled_number_cfg),
    EEBUS_DATA_ENUM("unit", ElectricalConnectionCharacteristicDataType, unit, unit_of_measurement_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_characteristic_data_elements_cfg[] = {
    EEBUS_DATA_TAG(
        "electricalConnectionId", ElectricalConnectionCharacteristicDataElementsType, electrical_connection_id),
    EEBUS_DATA_TAG("parameterId", ElectricalConnectionCharacteristicDataElementsType, parameter_id),
    EEBUS_DATA_TAG("characteristicId", ElectricalConnectionCharacteristicDataElementsType, characteristic_id),
    EEBUS_DATA_TAG("characteristicContext", ElectricalConnectionCharacteristicDataElementsType, characteristic_context),
    EEBUS_DATA_TAG("characteristicType", ElectricalConnectionCharacteristicDataElementsType, characteristic_type),
    EEBUS_DATA_SEQUENCE("value", ElectricalConnectionCharacteristicDataElementsType, value, scaled_number_elements_cfg),
    EEBUS_DATA_TAG("unit", ElectricalConnectionCharacteristicDataElementsType, unit),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_characteristic_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(ElectricalConnectionCharacteristicDataType), electrical_connection_characteristic_data_cfg);

static const EebusDataCfg electrical_connection_characteristic_list_data_cfg[] = {
    EEBUS_DATA_LIST("electricalConnectionCharacteristicData", ElectricalConnectionCharacteristicListDataType,
        electrical_connection_characteristic_data, &electrical_connection_characteristic_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg electrical_connection_characteristic_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32(
        "electricalConnectionId", ElectricalConnectionCharacteristicListDataSelectorsType, electrical_connection_id),
    EEBUS_DATA_UINT32("parameterId", ElectricalConnectionCharacteristicListDataSelectorsType, parameter_id),
    EEBUS_DATA_UINT32("characteristicId", ElectricalConnectionCharacteristicListDataSelectorsType, characteristic_id),
    EEBUS_DATA_ENUM("characteristicContext", ElectricalConnectionCharacteristicListDataSelectorsType,
        characteristic_context, electrical_connection_characteristic_context_lut),
    EEBUS_DATA_ENUM("characteristicType", ElectricalConnectionCharacteristicListDataSelectorsType, characteristic_type,
        electrical_connection_characteristic_type_lut),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_ELECTRICAL_CONNECTION_TYPES_INC_

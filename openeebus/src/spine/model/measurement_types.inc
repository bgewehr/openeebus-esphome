/**
 * @file
 * @brief SPINE Datagram Measurement payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_MEASUREMENT_TYPES_INC_
#define SRC_SPINE_MODEL_MEASUREMENT_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/measurement_types.h"
#include "src/spine/model/threshold_types.inc"

static const EnumMapping measurement_type_lut[] = {
    {"acceleration", kMeasurementTypeTypeAcceleration},
    {"angle", kMeasurementTypeTypeAngle},
    {"angularVelocity", kMeasurementTypeTypeAngularVelocity},
    {"area", kMeasurementTypeTypeArea},
    {"atmosphericPressure", kMeasurementTypeTypeAtmosphericPressure},
    {"capacity", kMeasurementTypeTypeCapacity},
    {"concentration", kMeasurementTypeTypeConcentration},
    {"count", kMeasurementTypeTypeCount},
    {"current", kMeasurementTypeTypeCurrent},
    {"density", kMeasurementTypeTypeDensity},
    {"distance", kMeasurementTypeTypeDistance},
    {"electricField", kMeasurementTypeTypeElectricField},
    {"energy", kMeasurementTypeTypeEnergy},
    {"force", kMeasurementTypeTypeForce},
    {"frequency", kMeasurementTypeTypeFrequency},
    {"harmonicDistortion", kMeasurementTypeTypeHarmonicDistortion},
    {"heat", kMeasurementTypeTypeHeat},
    {"heatFlux", kMeasurementTypeTypeHeatFlux},
    {"illuminance", kMeasurementTypeTypeIlluminance},
    {"impulse", kMeasurementTypeTypeImpulse},
    {"level", kMeasurementTypeTypeLevel},
    {"magneticField", kMeasurementTypeTypeMagneticField},
    {"mass", kMeasurementTypeTypeMass},
    {"massFlow", kMeasurementTypeTypeMassFlow},
    {"particles", kMeasurementTypeTypeParticles},
    {"percentage", kMeasurementTypeTypePercentage},
    {"power", kMeasurementTypeTypePower},
    {"powerFactor", kMeasurementTypeTypePowerFactor},
    {"pressure", kMeasurementTypeTypePressure},
    {"radonActivity", kMeasurementTypeTypeRadonActivity},
    {"relativeHumidity", kMeasurementTypeTypeRelativeHumidity},
    {"resistance", kMeasurementTypeTypeResistance},
    {"solarRadiation", kMeasurementTypeTypeSolarRadiation},
    {"speed", kMeasurementTypeTypeSpeed},
    {"temperature", kMeasurementTypeTypeTemperature},
    {"time", kMeasurementTypeTypeTime},
    {"torque", kMeasurementTypeTypeTorque},
    {"unknown", kMeasurementTypeTypeUnknown},
    {"velocity", kMeasurementTypeTypeVelocity},
    {"voltage", kMeasurementTypeTypeVoltage},
    {"volume", kMeasurementTypeTypeVolume},
    {"volumetricFlow", kMeasurementTypeTypeVolumetricFlow},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping measurement_value_type_lut[] = {
    {"value", kMeasurementValueTypeTypeValue},
    {"averageValue", kMeasurementValueTypeTypeAverageValue},
    {"minValue", kMeasurementValueTypeTypeMinValue},
    {"maxValue", kMeasurementValueTypeTypeMaxValue},
    {"standardDeviation", kMeasurementValueTypeTypeStandardDeviation},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping measurement_value_source_lut[] = {
    {"measuredValue", kMeasurementValueSourceTypeMeasuredValue},
    {"calculatedValue", kMeasurementValueSourceTypeCalculatedValue},
    {"empiricalValue", kMeasurementValueSourceTypeEmpiricalValue},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping measurement_value_tendency_lut[] = {
    {"rising", kMeasurementValueTendencyTypeRising},
    {"stable", kMeasurementValueTendencyTypeStable},
    {"falling", kMeasurementValueTendencyTypeFalling},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping measurement_value_state_lut[] = {
    {"normal", kMeasurementValueStateTypeNormal},
    {"outOfRange", kMeasurementValueStateTypeOutofrange},
    {"error", kMeasurementValueStateTypeError},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg measurement_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("measurementId", MeasurementDataType, measurement_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM_WITH_FLAGS(
        "valueType", MeasurementDataType, value_type, measurement_value_type_lut, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", MeasurementDataType, timestamp),
    EEBUS_DATA_SEQUENCE("value", MeasurementDataType, value, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("evaluationPeriod", MeasurementDataType, evaluation_period, time_period_cfg),
    EEBUS_DATA_ENUM("valueSource", MeasurementDataType, value_source, measurement_value_source_lut),
    EEBUS_DATA_ENUM("valueTendency", MeasurementDataType, value_tendency, measurement_value_tendency_lut),
    EEBUS_DATA_ENUM("valueState", MeasurementDataType, value_state, measurement_value_state_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_data_elements_cfg[] = {
    EEBUS_DATA_TAG("measurementId", MeasurementDataElementsType, measurement_id),
    EEBUS_DATA_TAG("valueType", MeasurementDataElementsType, value_type),
    EEBUS_DATA_TAG("timestamp", MeasurementDataElementsType, timestamp),
    EEBUS_DATA_TAG("value", MeasurementDataElementsType, value),
    EEBUS_DATA_TAG("evaluationPeriod", MeasurementDataElementsType, evaluation_period),
    EEBUS_DATA_TAG("valueSource", MeasurementDataElementsType, value_source),
    EEBUS_DATA_TAG("valueTendency", MeasurementDataElementsType, value_tendency),
    EEBUS_DATA_TAG("valueState", MeasurementDataElementsType, value_state),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(MeasurementDataType), measurement_data_cfg);

static const EebusDataCfg measurement_list_data_cfg[] = {
    EEBUS_DATA_LIST("measurementData", MeasurementListDataType, measurement_data, &measurement_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("measurementId", MeasurementListDataSelectorsType, measurement_id),
    EEBUS_DATA_ENUM("valueType", MeasurementListDataSelectorsType, value_type, measurement_value_type_lut),
    EEBUS_DATA_SEQUENCE(
        "timestampInterval", MeasurementListDataSelectorsType, timestamp_interval, timestamp_interval_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_series_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "measurementId", MeasurementSeriesDataType, measurement_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM_WITH_FLAGS(
        "valueType", MeasurementSeriesDataType, value_type, measurement_value_type_lut, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", MeasurementSeriesDataType, timestamp),
    EEBUS_DATA_SEQUENCE("value", MeasurementSeriesDataType, value, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("evaluationPeriod", MeasurementSeriesDataType, evaluation_period, time_period_cfg),
    EEBUS_DATA_ENUM("valueSource", MeasurementSeriesDataType, value_source, measurement_value_source_lut),
    EEBUS_DATA_ENUM("valueTendency", MeasurementSeriesDataType, value_tendency, measurement_value_tendency_lut),
    EEBUS_DATA_ENUM("valueState", MeasurementSeriesDataType, value_state, measurement_value_state_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_series_data_elements_cfg[] = {
    EEBUS_DATA_TAG("measurementId", MeasurementSeriesDataElementsType, measurement_id),
    EEBUS_DATA_TAG("valueType", MeasurementSeriesDataElementsType, value_type),
    EEBUS_DATA_TAG("timestamp", MeasurementSeriesDataElementsType, timestamp),
    EEBUS_DATA_TAG("value", MeasurementSeriesDataElementsType, value),
    EEBUS_DATA_TAG("evaluationPeriod", MeasurementSeriesDataElementsType, evaluation_period),
    EEBUS_DATA_TAG("valueSource", MeasurementSeriesDataElementsType, value_source),
    EEBUS_DATA_TAG("valueTendency", MeasurementSeriesDataElementsType, value_tendency),
    EEBUS_DATA_TAG("valueState", MeasurementSeriesDataElementsType, value_state),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_series_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(MeasurementSeriesDataType), measurement_series_data_cfg);

static const EebusDataCfg measurement_series_list_data_cfg[] = {
    EEBUS_DATA_LIST("measurementSeriesData", MeasurementSeriesListDataType, measurement_series_data,
        &measurement_series_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_series_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("measurementId", MeasurementSeriesListDataSelectorsType, measurement_id),
    EEBUS_DATA_ENUM("valueType", MeasurementSeriesListDataSelectorsType, value_type, measurement_value_type_lut),
    EEBUS_DATA_SEQUENCE(
        "timestampInterval", MeasurementSeriesListDataSelectorsType, timestamp_interval, timestamp_interval_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "measurementId", MeasurementConstraintsDataType, measurement_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("valueRangeMin", MeasurementConstraintsDataType, value_range_min, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("valueRangeMax", MeasurementConstraintsDataType, value_range_max, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("valueStepSize", MeasurementConstraintsDataType, value_step_size, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("measurementId", MeasurementConstraintsDataElementsType, measurement_id),
    EEBUS_DATA_SEQUENCE(
        "valueRangeMin", MeasurementConstraintsDataElementsType, value_range_min, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "valueRangeMax", MeasurementConstraintsDataElementsType, value_range_max, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "valueStepSize", MeasurementConstraintsDataElementsType, value_step_size, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_constraints_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(MeasurementConstraintsDataType), measurement_constraints_data_cfg);

static const EebusDataCfg measurement_constraints_list_data_cfg[] = {
    EEBUS_DATA_LIST("measurementConstraintsData", MeasurementConstraintsListDataType, measurement_constraints_data,
        &measurement_constraints_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_constraints_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("measurementId", MeasurementConstraintsListDataSelectorsType, measurement_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "measurementId", MeasurementDescriptionDataType, measurement_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("measurementType", MeasurementDescriptionDataType, measurement_type, measurement_type_lut),
    EEBUS_DATA_ENUM("commodityType", MeasurementDescriptionDataType, commodity_type, commodity_type_lut),
    EEBUS_DATA_ENUM("unit", MeasurementDescriptionDataType, unit, unit_of_measurement_lut),
    EEBUS_DATA_SEQUENCE("calibrationValue", MeasurementDescriptionDataType, calibration_value, scaled_number_cfg),
    EEBUS_DATA_ENUM("scopeType", MeasurementDescriptionDataType, scope_type, scope_type_lut),
    EEBUS_DATA_STRING("label", MeasurementDescriptionDataType, label),
    EEBUS_DATA_STRING("description", MeasurementDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("measurementId", MeasurementDescriptionDataElementsType, measurement_id),
    EEBUS_DATA_TAG("measurementType", MeasurementDescriptionDataElementsType, measurement_type),
    EEBUS_DATA_TAG("commodityType", MeasurementDescriptionDataElementsType, commodity_type),
    EEBUS_DATA_TAG("unit", MeasurementDescriptionDataElementsType, unit),
    EEBUS_DATA_SEQUENCE(
        "calibrationValue", MeasurementDescriptionDataElementsType, calibration_value, scaled_number_elements_cfg),
    EEBUS_DATA_TAG("scopeType", MeasurementDescriptionDataElementsType, scope_type),
    EEBUS_DATA_TAG("label", MeasurementDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", MeasurementDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(MeasurementDescriptionDataType), measurement_description_data_cfg);

static const EebusDataCfg measurement_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("measurementDescriptionData", MeasurementDescriptionListDataType, measurement_description_data,
        &measurement_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("measurementId", MeasurementDescriptionListDataSelectorsType, measurement_id),
    EEBUS_DATA_ENUM(
        "measurementType", MeasurementDescriptionListDataSelectorsType, measurement_type, measurement_type_lut),
    EEBUS_DATA_ENUM("commodityType", MeasurementDescriptionListDataSelectorsType, commodity_type, commodity_type_lut),
    EEBUS_DATA_ENUM("scopeType", MeasurementDescriptionListDataSelectorsType, scope_type, scope_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg threshold_id_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg measurement_threshold_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "measurementId", MeasurementThresholdRelationDataType, measurement_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_LIST("thresholdId", MeasurementThresholdRelationDataType, threshold_id, &threshold_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_threshold_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("measurementId", MeasurementThresholdRelationDataElementsType, measurement_id),
    EEBUS_DATA_TAG("thresholdId", MeasurementThresholdRelationDataElementsType, threshold_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_threshold_relation_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(MeasurementThresholdRelationDataType), measurement_threshold_relation_data_cfg);

static const EebusDataCfg measurement_threshold_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("measurementThresholdRelationData", MeasurementThresholdRelationListDataType,
        measurement_threshold_relation_data, &measurement_threshold_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_threshold_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("measurementId", MeasurementThresholdRelationListDataSelectorsType, measurement_id),
    EEBUS_DATA_UINT32("thresholdId", MeasurementThresholdRelationListDataSelectorsType, threshold_id),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_MEASUREMENT_TYPES_INC_

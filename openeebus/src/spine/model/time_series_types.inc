/**
 * @file
 * @brief SPINE Datagram Time Series payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_TIME_SERIES_TYPES_INC_
#define SRC_SPINE_MODEL_TIME_SERIES_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/measurement_types.inc"
#include "src/spine/model/time_series_types.h"

static const EnumMapping time_series_type_lut[] = {
    {"plan", kTimeSeriesTypeTypePlan},
    {"singleDemand", kTimeSeriesTypeTypeSingleDemand},
    {"constraints", kTimeSeriesTypeTypeConstraints},
    {"energyRequest", kTimeSeriesTypeTypeEnergyRequest},
    {"dischargingEnergyRequest", kTimeSeriesTypeTypeDischargingEnergyRequest},
    {"consumptionLimitCurve", kTimeSeriesTypeTypeConsumptionLimitCurve},
    {"productionLimitCurve", kTimeSeriesTypeTypeProductionLimitCurve},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg time_series_slot_cfg[] = {
    EEBUS_DATA_UINT32("timeSeriesSlotId", TimeSeriesSlotType, time_series_slot_id),
    EEBUS_DATA_SEQUENCE("timePeriod", TimeSeriesSlotType, time_period, time_period_cfg),
    EEBUS_DATA_DURATION("duration", TimeSeriesSlotType, duration),
    EEBUS_DATA_SEQUENCE(
        "recurrenceInformation", TimeSeriesSlotType, recurrence_information, absolute_or_recurring_time_cfg),
    EEBUS_DATA_SEQUENCE("value", TimeSeriesSlotType, value, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("minValue", TimeSeriesSlotType, min_value, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("maxValue", TimeSeriesSlotType, max_value, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_slot_elements_cfg[] = {
    EEBUS_DATA_TAG("timeSeriesSlotId", TimeSeriesSlotElementsType, time_series_slot_id),
    EEBUS_DATA_TAG("timePeriod", TimeSeriesSlotElementsType, time_period),
    EEBUS_DATA_TAG("duration", TimeSeriesSlotElementsType, duration),
    EEBUS_DATA_SEQUENCE("recurrenceInformation", TimeSeriesSlotElementsType, recurrence_information,
        absolute_or_recurring_time_elements_cfg),
    EEBUS_DATA_SEQUENCE("value", TimeSeriesSlotElementsType, value, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE("minValue", TimeSeriesSlotElementsType, min_value, scaled_number_elements_cfg),
    EEBUS_DATA_SEQUENCE("maxValue", TimeSeriesSlotElementsType, max_value, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_slot_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TimeSeriesSlotType), time_series_slot_cfg);

static const EebusDataCfg time_series_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("timeSeriesId", TimeSeriesDataType, time_series_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("timePeriod", TimeSeriesDataType, time_period, time_period_cfg),
    EEBUS_DATA_LIST("timeSeriesSlot", TimeSeriesDataType, time_series_slot, &time_series_slot_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timeSeriesId", TimeSeriesDataElementsType, time_series_id),
    EEBUS_DATA_SEQUENCE("timePeriod", TimeSeriesDataElementsType, time_period, time_period_elements_cfg),
    EEBUS_DATA_SEQUENCE("timeSeriesSlot", TimeSeriesDataElementsType, time_series_slot, time_series_slot_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TimeSeriesDataType), time_series_data_cfg);

static const EebusDataCfg time_series_list_data_cfg[] = {
    EEBUS_DATA_LIST("timeSeriesData", TimeSeriesListDataType, time_series_data, &time_series_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("timeSeriesId", TimeSeriesListDataSelectorsType, time_series_id),
    EEBUS_DATA_UINT32("timeSeriesSlotId", TimeSeriesListDataSelectorsType, time_series_slot_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "timeSeriesId", TimeSeriesDescriptionDataType, time_series_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("timeSeriesType", TimeSeriesDescriptionDataType, time_series_type, time_series_type_lut),
    EEBUS_DATA_BOOL("timeSeriesWriteable", TimeSeriesDescriptionDataType, time_series_writeable),
    EEBUS_DATA_BOOL("updateRequired", TimeSeriesDescriptionDataType, update_required),
    EEBUS_DATA_UINT32("measurementId", TimeSeriesDescriptionDataType, measurement_id),
    EEBUS_DATA_ENUM("currency", TimeSeriesDescriptionDataType, currency, currency_lut),
    EEBUS_DATA_ENUM("unit", TimeSeriesDescriptionDataType, unit, unit_of_measurement_lut),
    EEBUS_DATA_STRING("label", TimeSeriesDescriptionDataType, label),
    EEBUS_DATA_STRING("description", TimeSeriesDescriptionDataType, description),
    EEBUS_DATA_ENUM("scopeType", TimeSeriesDescriptionDataType, scope_type, scope_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timeSeriesId", TimeSeriesDescriptionDataElementsType, time_series_id),
    EEBUS_DATA_TAG("timeSeriesType", TimeSeriesDescriptionDataElementsType, time_series_type),
    EEBUS_DATA_TAG("timeSeriesWriteable", TimeSeriesDescriptionDataElementsType, time_series_writeable),
    EEBUS_DATA_TAG("updateRequired", TimeSeriesDescriptionDataElementsType, update_required),
    EEBUS_DATA_TAG("measurementId", TimeSeriesDescriptionDataElementsType, measurement_id),
    EEBUS_DATA_TAG("currency", TimeSeriesDescriptionDataElementsType, currency),
    EEBUS_DATA_TAG("unit", TimeSeriesDescriptionDataElementsType, unit),
    EEBUS_DATA_TAG("label", TimeSeriesDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", TimeSeriesDescriptionDataElementsType, description),
    EEBUS_DATA_TAG("scopeType", TimeSeriesDescriptionDataElementsType, scope_type),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TimeSeriesDescriptionDataType), time_series_description_data_cfg);

static const EebusDataCfg time_series_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("timeSeriesDescriptionData", TimeSeriesDescriptionListDataType, time_series_description_data,
        &time_series_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("timeSeriesId", TimeSeriesDescriptionListDataSelectorsType, time_series_id),
    EEBUS_DATA_ENUM(
        "timeSeriesType", TimeSeriesDescriptionListDataSelectorsType, time_series_type, time_series_type_lut),
    EEBUS_DATA_UINT32("measurementId", TimeSeriesDescriptionListDataSelectorsType, measurement_id),
    EEBUS_DATA_ENUM("scopeType", TimeSeriesDescriptionListDataSelectorsType, scope_type, scope_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "timeSeriesId", TimeSeriesConstraintsDataType, time_series_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("slotCountMin", TimeSeriesConstraintsDataType, slot_count_min),
    EEBUS_DATA_UINT32("slotCountMax", TimeSeriesConstraintsDataType, slot_count_max),
    EEBUS_DATA_DURATION("slotDurationMin", TimeSeriesConstraintsDataType, slot_duration_min),
    EEBUS_DATA_DURATION("slotDurationMax", TimeSeriesConstraintsDataType, slot_duration_max),
    EEBUS_DATA_DURATION("slotDurationStepSize", TimeSeriesConstraintsDataType, slot_duration_step_size),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME(
        "earliestTimeSeriesStartTime", TimeSeriesConstraintsDataType, earliest_time_series_start_time),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME(
        "latestTimeSeriesEndTime", TimeSeriesConstraintsDataType, latest_time_series_end_time),
    EEBUS_DATA_SEQUENCE("slotValueMin", TimeSeriesConstraintsDataType, slot_value_min, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("slotValueMax", TimeSeriesConstraintsDataType, slot_value_max, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("slotValueStepSize", TimeSeriesConstraintsDataType, slot_value_step_size, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timeSeriesId", TimeSeriesConstraintsDataElementsType, time_series_id),
    EEBUS_DATA_TAG("slotCountMin", TimeSeriesConstraintsDataElementsType, slot_count_min),
    EEBUS_DATA_TAG("slotCountMax", TimeSeriesConstraintsDataElementsType, slot_count_max),
    EEBUS_DATA_TAG("slotDurationMin", TimeSeriesConstraintsDataElementsType, slot_duration_min),
    EEBUS_DATA_TAG("slotDurationMax", TimeSeriesConstraintsDataElementsType, slot_duration_max),
    EEBUS_DATA_TAG("slotDurationStepSize", TimeSeriesConstraintsDataElementsType, slot_duration_step_size),
    EEBUS_DATA_TAG(
        "earliestTimeSeriesStartTime", TimeSeriesConstraintsDataElementsType, earliest_time_series_start_time),
    EEBUS_DATA_TAG("latestTimeSeriesEndTime", TimeSeriesConstraintsDataElementsType, latest_time_series_end_time),
    EEBUS_DATA_TAG("slotValueMin", TimeSeriesConstraintsDataElementsType, slot_value_min),
    EEBUS_DATA_TAG("slotValueMax", TimeSeriesConstraintsDataElementsType, slot_value_max),
    EEBUS_DATA_TAG("slotValueStepSize", TimeSeriesConstraintsDataElementsType, slot_value_step_size),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_constraints_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TimeSeriesConstraintsDataType), time_series_constraints_data_cfg);

static const EebusDataCfg time_series_constraints_list_data_cfg[] = {
    EEBUS_DATA_LIST("timeSeriesConstraintsData", TimeSeriesConstraintsListDataType, time_series_constraints_data,
        &time_series_constraints_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_series_constraints_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("timeSeriesId", TimeSeriesConstraintsListDataSelectorsType, time_series_id),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_TIME_SERIES_TYPES_INC_

/**
 * @file
 * @brief SPINE Datagram Power Sequences payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_POWER_SEQUENCES_TYPES_INC_
#define SRC_SPINE_MODEL_POWER_SEQUENCES_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/measurement_types.inc"
#include "src/spine/model/power_sequences_types.h"

static const EnumMapping power_time_slot_value_type_lut[] = {
    {"power", kPowerTimeSlotValueTypeTypePower},
    {"powerMin", kPowerTimeSlotValueTypeTypePowerMin},
    {"powerMax", kPowerTimeSlotValueTypeTypePowerMax},
    {"powerExpectedValue", kPowerTimeSlotValueTypeTypePowerExpectedValue},
    {"powerStandardDeviation", kPowerTimeSlotValueTypeTypePowerStandardDeviation},
    {"powerSkewness", kPowerTimeSlotValueTypeTypePowerSkewness},
    {"energy", kPowerTimeSlotValueTypeTypeEnergy},
    {"energyMin", kPowerTimeSlotValueTypeTypeEnergyMin},
    {"energyMax", kPowerTimeSlotValueTypeTypeEnergyMax},
    {"energyExpectedValue", kPowerTimeSlotValueTypeTypeEnergyExpectedValue},
    {"energyStandardDeviation", kPowerTimeSlotValueTypeTypeEnergyStandardDeviation},
    {"energySkewness", kPowerTimeSlotValueTypeTypeEnergySkewness},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping power_sequence_scope_lut[] = {
    {"forecast", kPowerSequenceScopeTypeForecast},
    {"measurement", kPowerSequenceScopeTypeMeasurement},
    {"recommendation", kPowerSequenceScopeTypeRecommendation},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping power_sequence_state_lut[] = {
    {"running", kPowerSequenceStateTypeRunning},
    {"paused", kPowerSequenceStateTypePaused},
    {"scheduled", kPowerSequenceStateTypeScheduled},
    {"scheduledPaused", kPowerSequenceStateTypeScheduledPaused},
    {"pending", kPowerSequenceStateTypePending},
    {"inactive", kPowerSequenceStateTypeInactive},
    {"completed", kPowerSequenceStateTypeCompleted},
    {"invalid", kPowerSequenceStateTypeInvalid},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg power_time_slot_schedule_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("sequenceId", PowerTimeSlotScheduleDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32_WITH_FLAGS("slotNumber", PowerTimeSlotScheduleDataType, slot_number, kEebusDataFlagIsSubIdentifier),
    EEBUS_DATA_SEQUENCE("timePeriod", PowerTimeSlotScheduleDataType, time_period, time_period_cfg),
    EEBUS_DATA_DURATION("defaultDuration", PowerTimeSlotScheduleDataType, default_duration),
    EEBUS_DATA_DURATION("durationUncertainty", PowerTimeSlotScheduleDataType, duration_uncertainty),
    EEBUS_DATA_BOOL("slotActivated", PowerTimeSlotScheduleDataType, slot_activated),
    EEBUS_DATA_STRING("description", PowerTimeSlotScheduleDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_schedule_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerTimeSlotScheduleDataElementsType, sequence_id),
    EEBUS_DATA_TAG("slotNumber", PowerTimeSlotScheduleDataElementsType, slot_number),
    EEBUS_DATA_TAG("timePeriod", PowerTimeSlotScheduleDataElementsType, time_period),
    EEBUS_DATA_TAG("defaultDuration", PowerTimeSlotScheduleDataElementsType, default_duration),
    EEBUS_DATA_TAG("durationUncertainty", PowerTimeSlotScheduleDataElementsType, duration_uncertainty),
    EEBUS_DATA_TAG("slotActivated", PowerTimeSlotScheduleDataElementsType, slot_activated),
    EEBUS_DATA_TAG("description", PowerTimeSlotScheduleDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_schedule_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(PowerTimeSlotScheduleDataType), power_time_slot_schedule_data_cfg);

static const EebusDataCfg power_time_slot_schedule_list_data_cfg[] = {
    EEBUS_DATA_LIST("powerTimeSlotScheduleData", PowerTimeSlotScheduleListDataType, power_time_slot_schedule_data,
        &power_time_slot_schedule_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_schedule_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", PowerTimeSlotScheduleListDataSelectorsType, sequence_id),
    EEBUS_DATA_UINT32("slotNumber", PowerTimeSlotScheduleListDataSelectorsType, slot_number),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_value_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("sequenceId", PowerTimeSlotValueDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("slotNumber", PowerTimeSlotValueDataType, slot_number),
    EEBUS_DATA_ENUM("valueType", PowerTimeSlotValueDataType, value_type, power_time_slot_value_type_lut),
    EEBUS_DATA_SEQUENCE("value", PowerTimeSlotValueDataType, value, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_value_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerTimeSlotValueDataElementsType, sequence_id),
    EEBUS_DATA_TAG("slotNumber", PowerTimeSlotValueDataElementsType, slot_number),
    EEBUS_DATA_TAG("valueType", PowerTimeSlotValueDataElementsType, value_type),
    EEBUS_DATA_SEQUENCE("value", PowerTimeSlotValueDataElementsType, value, scaled_number_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_value_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(PowerTimeSlotValueDataType), power_time_slot_value_data_cfg);

static const EebusDataCfg power_time_slot_value_list_data_cfg[] = {
    EEBUS_DATA_LIST("powerTimeSlotValueListData", PowerTimeSlotValueListDataType, power_time_slot_value_data,
        &power_time_slot_value_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_value_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", PowerTimeSlotValueListDataSelectorsType, sequence_id),
    EEBUS_DATA_UINT32("slotNumber", PowerTimeSlotValueListDataSelectorsType, slot_number),
    EEBUS_DATA_ENUM("valueType", PowerTimeSlotValueListDataSelectorsType, value_type, power_time_slot_value_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_schedule_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sequenceId", PowerTimeSlotScheduleConstraintsDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("slotNumber", PowerTimeSlotScheduleConstraintsDataType, slot_number),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME(
        "earliestStartTime", PowerTimeSlotScheduleConstraintsDataType, earliest_start_time),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("latestEndTime", PowerTimeSlotScheduleConstraintsDataType, latest_end_time),
    EEBUS_DATA_DURATION("minDuration", PowerTimeSlotScheduleConstraintsDataType, min_duration),
    EEBUS_DATA_DURATION("maxDuration", PowerTimeSlotScheduleConstraintsDataType, max_duration),
    EEBUS_DATA_BOOL("optionalSlot", PowerTimeSlotScheduleConstraintsDataType, optional_slot),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_schedule_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerTimeSlotScheduleConstraintsDataElementsType, sequence_id),
    EEBUS_DATA_TAG("slotNumber", PowerTimeSlotScheduleConstraintsDataElementsType, slot_number),
    EEBUS_DATA_TAG("earliestStartTime", PowerTimeSlotScheduleConstraintsDataElementsType, earliest_start_time),
    EEBUS_DATA_TAG("latestEndTime", PowerTimeSlotScheduleConstraintsDataElementsType, latest_end_time),
    EEBUS_DATA_TAG("minDuration", PowerTimeSlotScheduleConstraintsDataElementsType, min_duration),
    EEBUS_DATA_TAG("maxDuration", PowerTimeSlotScheduleConstraintsDataElementsType, max_duration),
    EEBUS_DATA_TAG("optionalSlot", PowerTimeSlotScheduleConstraintsDataElementsType, optional_slot),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_schedule_constraints_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(PowerTimeSlotScheduleConstraintsDataType), power_time_slot_schedule_constraints_data_cfg);

static const EebusDataCfg power_time_slot_schedule_constraints_list_data_cfg[] = {
    EEBUS_DATA_LIST("powerTimeSlotScheduleConstraintsData", PowerTimeSlotScheduleConstraintsListDataType,
        power_time_slot_schedule_constraints_data, &power_time_slot_schedule_constraints_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_schedule_constraints_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", PowerTimeSlotScheduleConstraintsListDataSelectorsType, sequence_id),
    EEBUS_DATA_UINT32("slotNumber", PowerTimeSlotScheduleConstraintsListDataSelectorsType, slot_number),
    EEBUS_DATA_END,
};

static const EebusDataCfg sequence_id_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg power_sequence_alternatives_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "alternativesId", PowerSequenceAlternativesRelationDataType, alternatives_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_LIST(
        "sequenceId", PowerSequenceAlternativesRelationDataType, sequence_id, &sequence_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_alternatives_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("alternativesId", PowerSequenceAlternativesRelationDataElementsType, alternatives_id),
    EEBUS_DATA_TAG("sequenceId", PowerSequenceAlternativesRelationDataElementsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_alternatives_relation_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(PowerSequenceAlternativesRelationDataType), power_sequence_alternatives_relation_data_cfg);

static const EebusDataCfg power_sequence_alternatives_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("powerSequenceAlternativesRelationData", PowerSequenceAlternativesRelationListDataType,
        power_sequence_alternatives_relation_data, &power_sequence_alternatives_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_alternatives_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("alternativesId", PowerSequenceAlternativesRelationListDataSelectorsType, alternatives_id),
    EEBUS_DATA_LIST("sequenceId", PowerSequenceAlternativesRelationListDataSelectorsType, sequence_id,
        &sequence_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sequenceId", PowerSequenceDescriptionDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_STRING("description", PowerSequenceDescriptionDataType, description),
    EEBUS_DATA_ENUM(
        "positiveEnergyDirection", PowerSequenceDescriptionDataType, positive_energy_direction, energy_direction_lut),
    EEBUS_DATA_ENUM("powerUnit", PowerSequenceDescriptionDataType, power_unit, unit_of_measurement_lut),
    EEBUS_DATA_ENUM("energyUnit", PowerSequenceDescriptionDataType, energy_unit, unit_of_measurement_lut),
    EEBUS_DATA_ENUM("valueSource", PowerSequenceDescriptionDataType, value_source, measurement_value_source_lut),
    EEBUS_DATA_ENUM("scope", PowerSequenceDescriptionDataType, scope, power_sequence_scope_lut),
    EEBUS_DATA_UINT32("taskIdentifier", PowerSequenceDescriptionDataType, task_identifier),
    EEBUS_DATA_UINT32("repetitionsTotal", PowerSequenceDescriptionDataType, repetitions_total),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerSequenceDescriptionDataElementsType, sequence_id),
    EEBUS_DATA_TAG("description", PowerSequenceDescriptionDataElementsType, description),
    EEBUS_DATA_TAG("positiveEnergyDirection", PowerSequenceDescriptionDataElementsType, positive_energy_direction),
    EEBUS_DATA_TAG("powerUnit", PowerSequenceDescriptionDataElementsType, power_unit),
    EEBUS_DATA_TAG("energyUnit", PowerSequenceDescriptionDataElementsType, energy_unit),
    EEBUS_DATA_TAG("valueSource", PowerSequenceDescriptionDataElementsType, value_source),
    EEBUS_DATA_TAG("scope", PowerSequenceDescriptionDataElementsType, scope),
    EEBUS_DATA_TAG("taskIdentifier", PowerSequenceDescriptionDataElementsType, task_identifier),
    EEBUS_DATA_TAG("repetitionsTotal", PowerSequenceDescriptionDataElementsType, repetitions_total),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(PowerSequenceDescriptionDataType), power_sequence_description_data_cfg);

static const EebusDataCfg power_sequence_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("powerSequenceDescriptionData", PowerSequenceDescriptionListDataType,
        power_sequence_description_data, &power_sequence_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_LIST(
        "sequenceId", PowerSequenceDescriptionListDataSelectorsType, sequence_id, &sequence_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_state_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("sequenceId", PowerSequenceStateDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("state", PowerSequenceStateDataType, state, power_sequence_state_lut),
    EEBUS_DATA_UINT32("activeSlotNumber", PowerSequenceStateDataType, active_slot_number),
    EEBUS_DATA_DURATION("elapsedSlotTime", PowerSequenceStateDataType, elapsed_slot_time),
    EEBUS_DATA_DURATION("remainingSlotTime", PowerSequenceStateDataType, remaining_slot_time),
    EEBUS_DATA_BOOL("sequenceRemoteControllable", PowerSequenceStateDataType, sequence_remote_controllable),
    EEBUS_DATA_UINT32("activeRepetitionNumber", PowerSequenceStateDataType, active_repetition_number),
    EEBUS_DATA_DURATION("remainingPauseTime", PowerSequenceStateDataType, remaining_pause_time),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_state_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerSequenceStateDataElementsType, sequence_id),
    EEBUS_DATA_TAG("state", PowerSequenceStateDataElementsType, state),
    EEBUS_DATA_TAG("activeSlotNumber", PowerSequenceStateDataElementsType, active_slot_number),
    EEBUS_DATA_TAG("elapsedSlotTime", PowerSequenceStateDataElementsType, elapsed_slot_time),
    EEBUS_DATA_TAG("remainingSlotTime", PowerSequenceStateDataElementsType, remaining_slot_time),
    EEBUS_DATA_TAG("sequenceRemoteControllable", PowerSequenceStateDataElementsType, sequence_remote_controllable),
    EEBUS_DATA_TAG("activeRepetitionNumber", PowerSequenceStateDataElementsType, active_repetition_number),
    EEBUS_DATA_TAG("remainingPauseTime", PowerSequenceStateDataElementsType, remaining_pause_time),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_state_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(PowerSequenceStateDataType), power_sequence_state_data_cfg);

static const EebusDataCfg power_sequence_state_list_data_cfg[] = {
    EEBUS_DATA_LIST("powerSequenceStateData", PowerSequenceStateListDataType, power_sequence_state_data,
        &power_sequence_state_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_state_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", PowerSequenceStateListDataSelectorsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("sequenceId", PowerSequenceScheduleDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("startTime", PowerSequenceScheduleDataType, start_time),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("endTime", PowerSequenceScheduleDataType, end_time),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerSequenceScheduleDataElementsType, sequence_id),
    EEBUS_DATA_TAG("startTime", PowerSequenceScheduleDataElementsType, start_time),
    EEBUS_DATA_TAG("endTime", PowerSequenceScheduleDataElementsType, end_time),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(PowerSequenceScheduleDataType), power_sequence_schedule_data_cfg);

static const EebusDataCfg power_sequence_schedule_list_data_cfg[] = {
    EEBUS_DATA_LIST("powerSequenceScheduleData", PowerSequenceScheduleListDataType, power_sequence_schedule_data,
        &power_sequence_schedule_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", PowerSequenceScheduleListDataSelectorsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sequenceId", PowerSequenceScheduleConstraintsDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME(
        "earliestStartTime", PowerSequenceScheduleConstraintsDataType, earliest_start_time),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME(
        "latestStartTime", PowerSequenceScheduleConstraintsDataType, latest_start_time),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME(
        "earliestEndTime", PowerSequenceScheduleConstraintsDataType, earliest_end_time),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("latestEndTime", PowerSequenceScheduleConstraintsDataType, latest_end_time),
    EEBUS_DATA_BOOL("optionalSequence", PowerSequenceScheduleConstraintsDataType, optional_sequence),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerSequenceScheduleConstraintsDataElementsType, sequence_id),
    EEBUS_DATA_TAG("earliestStartTime", PowerSequenceScheduleConstraintsDataElementsType, earliest_start_time),
    EEBUS_DATA_TAG("latestStartTime", PowerSequenceScheduleConstraintsDataElementsType, latest_start_time),
    EEBUS_DATA_TAG("earliestEndTime", PowerSequenceScheduleConstraintsDataElementsType, earliest_end_time),
    EEBUS_DATA_TAG("latestEndTime", PowerSequenceScheduleConstraintsDataElementsType, latest_end_time),
    EEBUS_DATA_TAG("optionalSequence", PowerSequenceScheduleConstraintsDataElementsType, optional_sequence),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_constraints_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(PowerSequenceScheduleConstraintsDataType), power_sequence_schedule_constraints_data_cfg);

static const EebusDataCfg power_sequence_schedule_constraints_list_data_cfg[] = {
    EEBUS_DATA_LIST("powerSequenceScheduleConstraintsData", PowerSequenceScheduleConstraintsListDataType,
        power_sequence_schedule_constraints_data, &power_sequence_schedule_constraints_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_constraints_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", PowerSequenceScheduleConstraintsListDataSelectorsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_price_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("sequenceId", PowerSequencePriceDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("potentialStartTime", PowerSequencePriceDataType, potential_start_time),
    EEBUS_DATA_SEQUENCE("price", PowerSequencePriceDataType, price, scaled_number_cfg),
    EEBUS_DATA_ENUM("currency", PowerSequencePriceDataType, currency, currency_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_price_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerSequencePriceDataElementsType, sequence_id),
    EEBUS_DATA_TAG("potentialStartTime", PowerSequencePriceDataElementsType, potential_start_time),
    EEBUS_DATA_TAG("price", PowerSequencePriceDataElementsType, price),
    EEBUS_DATA_TAG("currency", PowerSequencePriceDataElementsType, currency),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_price_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(PowerSequencePriceDataType), power_sequence_price_data_cfg);

static const EebusDataCfg power_sequence_price_list_data_cfg[] = {
    EEBUS_DATA_LIST("powerSequencePriceData", PowerSequencePriceListDataType, power_sequence_price_data,
        &power_sequence_price_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_price_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", PowerSequencePriceListDataSelectorsType, sequence_id),
    EEBUS_DATA_SEQUENCE("potentialStartTimeInterval", PowerSequencePriceListDataSelectorsType,
        potential_start_time_interval, timestamp_interval_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_preference_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sequenceId", PowerSequenceSchedulePreferenceDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_BOOL("greenest", PowerSequenceSchedulePreferenceDataType, greenest),
    EEBUS_DATA_BOOL("cheapest", PowerSequenceSchedulePreferenceDataType, cheapest),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_preference_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerSequenceSchedulePreferenceDataElementsType, sequence_id),
    EEBUS_DATA_TAG("greenest", PowerSequenceSchedulePreferenceDataElementsType, greenest),
    EEBUS_DATA_TAG("cheapest", PowerSequenceSchedulePreferenceDataElementsType, cheapest),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_preference_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(PowerSequenceSchedulePreferenceDataType), power_sequence_schedule_preference_data_cfg);

static const EebusDataCfg power_sequence_schedule_preference_list_data_cfg[] = {
    EEBUS_DATA_LIST("powerSequenceSchedulePreferenceData", PowerSequenceSchedulePreferenceListDataType,
        power_sequence_schedule_preference_data, &power_sequence_schedule_preference_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_preference_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", PowerSequenceSchedulePreferenceListDataSelectorsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_node_schedule_information_data_cfg[] = {
    EEBUS_DATA_BOOL("nodeRemoteControllable", PowerSequenceNodeScheduleInformationDataType, node_remote_controllable),
    EEBUS_DATA_BOOL("supportsSingleSlotSchedulingOnly", PowerSequenceNodeScheduleInformationDataType,
        supports_single_slot_scheduling_only),
    EEBUS_DATA_UINT32("alternativesCount", PowerSequenceNodeScheduleInformationDataType, alternatives_count),
    EEBUS_DATA_UINT32(
        "totalSequencesCountMax", PowerSequenceNodeScheduleInformationDataType, total_sequences_count_max),
    EEBUS_DATA_BOOL("supportsReselection", PowerSequenceNodeScheduleInformationDataType, supports_reselection),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_node_schedule_information_data_elements_cfg[] = {
    EEBUS_DATA_TAG(
        "nodeRemoteControllable", PowerSequenceNodeScheduleInformationDataElementsType, node_remote_controllable),
    EEBUS_DATA_TAG("supportsSingleSlotSchedulingOnly", PowerSequenceNodeScheduleInformationDataElementsType,
        supports_single_slot_scheduling_only),
    EEBUS_DATA_TAG("alternativesCount", PowerSequenceNodeScheduleInformationDataElementsType, alternatives_count),
    EEBUS_DATA_TAG(
        "totalSequencesCountMax", PowerSequenceNodeScheduleInformationDataElementsType, total_sequences_count_max),
    EEBUS_DATA_TAG("supportsReselection", PowerSequenceNodeScheduleInformationDataElementsType, supports_reselection),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_configuration_request_call_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", PowerSequenceScheduleConfigurationRequestCallType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_schedule_configuration_request_call_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerSequenceScheduleConfigurationRequestCallElementsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_price_calculation_request_call_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", PowerSequencePriceCalculationRequestCallType, sequence_id),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME(
        "potentialStartTime", PowerSequencePriceCalculationRequestCallType, potential_start_time),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_price_calculation_request_call_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", PowerSequencePriceCalculationRequestCallElementsType, sequence_id),
    EEBUS_DATA_TAG("potentialStartTime", PowerSequencePriceCalculationRequestCallElementsType, potential_start_time),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_POWER_SEQUENCES_TYPES_INC_

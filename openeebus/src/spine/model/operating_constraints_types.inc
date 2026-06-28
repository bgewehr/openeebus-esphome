/**
 * @file
 * @brief SPINE Datagram Operating Constraints payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_OPERATING_CONSTRAINTS_TYPES_INC_
#define SRC_SPINE_MODEL_OPERATING_CONSTRAINTS_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/operating_constraints_types.h"
#include "src/spine/model/power_sequences_types.inc"

static const EebusDataCfg operating_constraints_interrupt_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sequenceId", OperatingConstraintsInterruptDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_BOOL("isPausable", OperatingConstraintsInterruptDataType, is_pausable),
    EEBUS_DATA_BOOL("isStoppable", OperatingConstraintsInterruptDataType, is_stoppable),
    EEBUS_DATA_BOOL(
        "notInterruptibleAtHighPower", OperatingConstraintsInterruptDataType, not_interruptible_at_high_power),
    EEBUS_DATA_UINT32("maxCyclesPerDay", OperatingConstraintsInterruptDataType, max_cycles_per_day),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_interrupt_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", OperatingConstraintsInterruptDataElementsType, sequence_id),
    EEBUS_DATA_TAG("isPausable", OperatingConstraintsInterruptDataElementsType, is_pausable),
    EEBUS_DATA_TAG("isStoppable", OperatingConstraintsInterruptDataElementsType, is_stoppable),
    EEBUS_DATA_TAG(
        "notInterruptibleAtHighPower", OperatingConstraintsInterruptDataElementsType, not_interruptible_at_high_power),
    EEBUS_DATA_TAG("maxCyclesPerDay", OperatingConstraintsInterruptDataElementsType, max_cycles_per_day),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_interrupt_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(OperatingConstraintsInterruptDataType), operating_constraints_interrupt_data_cfg);

static const EebusDataCfg operating_constraints_interrupt_list_data_cfg[] = {
    EEBUS_DATA_LIST("operatingConstraintsInterruptData", OperatingConstraintsInterruptListDataType,
        operating_constraints_interrupt_data, &operating_constraints_interrupt_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_interrupt_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", OperatingConstraintsInterruptListDataSelectorsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_duration_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sequenceId", OperatingConstraintsDurationDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_DURATION("activeDurationMin", OperatingConstraintsDurationDataType, active_duration_min),
    EEBUS_DATA_DURATION("activeDurationMax", OperatingConstraintsDurationDataType, active_duration_max),
    EEBUS_DATA_DURATION("pauseDurationMin", OperatingConstraintsDurationDataType, pause_duration_min),
    EEBUS_DATA_DURATION("pauseDurationMax", OperatingConstraintsDurationDataType, pause_duration_max),
    EEBUS_DATA_DURATION("activeDurationSumMin", OperatingConstraintsDurationDataType, active_duration_sum_min),
    EEBUS_DATA_DURATION("activeDurationSumMax", OperatingConstraintsDurationDataType, active_duration_sum_max),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_duration_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", OperatingConstraintsDurationDataElementsType, sequence_id),
    EEBUS_DATA_TAG("activeDurationMin", OperatingConstraintsDurationDataElementsType, active_duration_min),
    EEBUS_DATA_TAG("activeDurationMax", OperatingConstraintsDurationDataElementsType, active_duration_max),
    EEBUS_DATA_TAG("pauseDurationMin", OperatingConstraintsDurationDataElementsType, pause_duration_min),
    EEBUS_DATA_TAG("pauseDurationMax", OperatingConstraintsDurationDataElementsType, pause_duration_max),
    EEBUS_DATA_TAG("activeDurationSumMin", OperatingConstraintsDurationDataElementsType, active_duration_sum_min),
    EEBUS_DATA_TAG("activeDurationSumMax", OperatingConstraintsDurationDataElementsType, active_duration_sum_max),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_duration_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(OperatingConstraintsDurationDataType), operating_constraints_duration_data_cfg);

static const EebusDataCfg operating_constraints_duration_list_data_cfg[] = {
    EEBUS_DATA_LIST("operatingConstraintsDurationData", OperatingConstraintsDurationListDataType,
        operating_constraints_duration_data, &operating_constraints_duration_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_duration_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", OperatingConstraintsDurationListDataSelectorsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sequenceId", OperatingConstraintsPowerDescriptionDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("positiveEnergyDirection", OperatingConstraintsPowerDescriptionDataType, positive_energy_direction,
        energy_direction_lut),
    EEBUS_DATA_ENUM("powerUnit", OperatingConstraintsPowerDescriptionDataType, power_unit, unit_of_measurement_lut),
    EEBUS_DATA_ENUM("energyUnit", OperatingConstraintsPowerDescriptionDataType, energy_unit, unit_of_measurement_lut),
    EEBUS_DATA_STRING("description", OperatingConstraintsPowerDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", OperatingConstraintsPowerDescriptionDataElementsType, sequence_id),
    EEBUS_DATA_TAG(
        "positiveEnergyDirection", OperatingConstraintsPowerDescriptionDataElementsType, positive_energy_direction),
    EEBUS_DATA_TAG("powerUnit", OperatingConstraintsPowerDescriptionDataElementsType, power_unit),
    EEBUS_DATA_TAG("energyUnit", OperatingConstraintsPowerDescriptionDataElementsType, energy_unit),
    EEBUS_DATA_TAG("description", OperatingConstraintsPowerDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_description_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(OperatingConstraintsPowerDescriptionDataType), operating_constraints_power_description_data_cfg);

static const EebusDataCfg operating_constraints_power_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("operatingConstraintsPowerDescriptionData", OperatingConstraintsPowerDescriptionListDataType,
        operating_constraints_power_description_data, &operating_constraints_power_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", OperatingConstraintsPowerDescriptionListDataSelectorsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_range_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sequenceId", OperatingConstraintsPowerRangeDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("powerMin", OperatingConstraintsPowerRangeDataType, power_min, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("powerMax", OperatingConstraintsPowerRangeDataType, power_max, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("energyMin", OperatingConstraintsPowerRangeDataType, energy_min, scaled_number_cfg),
    EEBUS_DATA_SEQUENCE("energyMax", OperatingConstraintsPowerRangeDataType, energy_max, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_range_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", OperatingConstraintsPowerRangeDataElementsType, sequence_id),
    EEBUS_DATA_TAG("powerMin", OperatingConstraintsPowerRangeDataElementsType, power_min),
    EEBUS_DATA_TAG("powerMax", OperatingConstraintsPowerRangeDataElementsType, power_max),
    EEBUS_DATA_TAG("energyMin", OperatingConstraintsPowerRangeDataElementsType, energy_min),
    EEBUS_DATA_TAG("energyMax", OperatingConstraintsPowerRangeDataElementsType, energy_max),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_range_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(OperatingConstraintsPowerRangeDataType), operating_constraints_power_range_data_cfg);

static const EebusDataCfg operating_constraints_power_range_list_data_cfg[] = {
    EEBUS_DATA_LIST("operatingConstraintsPowerRangeData", OperatingConstraintsPowerRangeListDataType,
        operating_constraints_power_range_data, &operating_constraints_power_range_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_range_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", OperatingConstraintsPowerRangeListDataSelectorsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_level_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sequenceId", OperatingConstraintsPowerLevelDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("power", OperatingConstraintsPowerLevelDataType, power, scaled_number_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_level_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", OperatingConstraintsPowerLevelDataElementsType, sequence_id),
    EEBUS_DATA_TAG("power", OperatingConstraintsPowerLevelDataElementsType, power),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_level_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(OperatingConstraintsPowerLevelDataType), operating_constraints_power_level_data_cfg);

static const EebusDataCfg operating_constraints_power_level_list_data_cfg[] = {
    EEBUS_DATA_LIST("operatingConstraintsPowerLevelData", OperatingConstraintsPowerLevelListDataType,
        operating_constraints_power_level_data, &operating_constraints_power_level_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_power_level_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", OperatingConstraintsPowerLevelListDataSelectorsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_resume_implication_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sequenceId", OperatingConstraintsResumeImplicationDataType, sequence_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("resumeEnergyEstimated", OperatingConstraintsResumeImplicationDataType, resume_energy_estimated,
        scaled_number_cfg),
    EEBUS_DATA_ENUM("energyUnit", OperatingConstraintsResumeImplicationDataType, energy_unit, unit_of_measurement_lut),
    EEBUS_DATA_SEQUENCE(
        "resumeCostEstimated", OperatingConstraintsResumeImplicationDataType, resume_cost_estimated, scaled_number_cfg),
    EEBUS_DATA_ENUM("currency", OperatingConstraintsResumeImplicationDataType, currency, currency_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_resume_implication_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", OperatingConstraintsResumeImplicationDataElementsType, sequence_id),
    EEBUS_DATA_SEQUENCE("resumeEnergyEstimated", OperatingConstraintsResumeImplicationDataElementsType,
        resume_energy_estimated, scaled_number_elements_cfg),
    EEBUS_DATA_TAG("energyUnit", OperatingConstraintsResumeImplicationDataElementsType, energy_unit),
    EEBUS_DATA_SEQUENCE("resumeCostEstimated", OperatingConstraintsResumeImplicationDataElementsType,
        resume_cost_estimated, scaled_number_elements_cfg),
    EEBUS_DATA_TAG("currency", OperatingConstraintsResumeImplicationDataElementsType, currency),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_resume_implication_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(OperatingConstraintsResumeImplicationDataType), operating_constraints_resume_implication_data_cfg);

static const EebusDataCfg operating_constraints_resume_implication_list_data_cfg[] = {
    EEBUS_DATA_LIST("operatingConstraintsResumeImplicationData", OperatingConstraintsResumeImplicationListDataType,
        operating_constraints_resume_implication_data, &operating_constraints_resume_implication_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg operating_constraints_resume_implication_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", OperatingConstraintsResumeImplicationListDataSelectorsType, sequence_id),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_OPERATING_CONSTRAINTS_TYPES_INC_

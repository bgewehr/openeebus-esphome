/**
 * @file
 * @brief SPINE Datagram Direct Control payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_DIRECT_CONTROL_TYPES_INC_
#define SRC_SPINE_MODEL_DIRECT_CONTROL_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/direct_control_types.h"
#include "src/spine/model/measurement_types.inc"
#include "src/spine/model/power_sequences_types.inc"

static const EebusDataCfg direct_control_activity_data_cfg[] = {
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", DirectControlActivityDataType, timestamp),
    EEBUS_DATA_STRING("activityState", DirectControlActivityDataType, activity_state),
    EEBUS_DATA_BOOL("isActivityStateChangeable", DirectControlActivityDataType, is_activity_state_changeable),
    EEBUS_DATA_ENUM("energyMode", DirectControlActivityDataType, energy_mode, energy_mode_lut),
    EEBUS_DATA_BOOL("isEnergyModeChangeable", DirectControlActivityDataType, is_energy_mode_changeable),
    EEBUS_DATA_SEQUENCE("power", DirectControlActivityDataType, power, scaled_number_cfg),
    EEBUS_DATA_BOOL("isPowerChangeable", DirectControlActivityDataType, is_power_changeable),
    EEBUS_DATA_SEQUENCE("energy", DirectControlActivityDataType, energy, scaled_number_cfg),
    EEBUS_DATA_BOOL("isEnergyChangeable", DirectControlActivityDataType, is_energy_changeable),
    EEBUS_DATA_UINT32("sequence_id", DirectControlActivityDataType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg direct_control_activity_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timestamp", DirectControlActivityDataElementsType, timestamp),
    EEBUS_DATA_TAG("activityState", DirectControlActivityDataElementsType, activity_state),
    EEBUS_DATA_TAG("isActivityStateChangeable", DirectControlActivityDataElementsType, is_activity_state_changeable),
    EEBUS_DATA_TAG("energyMode", DirectControlActivityDataElementsType, energy_mode),
    EEBUS_DATA_TAG("isEnergyModeChangeable", DirectControlActivityDataElementsType, is_energy_mode_changeable),
    EEBUS_DATA_SEQUENCE("power", DirectControlActivityDataElementsType, power, scaled_number_elements_cfg),
    EEBUS_DATA_TAG("isPowerChangeable", DirectControlActivityDataElementsType, is_power_changeable),
    EEBUS_DATA_SEQUENCE("energy", DirectControlActivityDataElementsType, energy, scaled_number_elements_cfg),
    EEBUS_DATA_TAG("isEnergyChangeable", DirectControlActivityDataElementsType, is_energy_changeable),
    EEBUS_DATA_TAG("sequence_id", DirectControlActivityDataElementsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg direct_control_activity_data_elements_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(DirectControlActivityDataType), direct_control_activity_data_cfg);

static const EebusDataCfg direct_control_activity_list_data_cfg[] = {
    EEBUS_DATA_LIST("directControlActivityDataElements", DirectControlActivityListDataType,
        direct_control_activity_data_elements, &direct_control_activity_data_elements_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg direct_control_activity_list_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "timestampInterval", DirectControlActivityListDataSelectorsType, timestamp_interval, timestamp_interval_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg direct_control_description_data_cfg[] = {
    EEBUS_DATA_ENUM(
        "positiveEnergyDirection", DirectControlDescriptionDataType, positive_energy_direction, energy_direction_lut),
    EEBUS_DATA_ENUM("powerUnit", DirectControlDescriptionDataType, power_unit, unit_of_measurement_lut),
    EEBUS_DATA_ENUM("energyUnit", DirectControlDescriptionDataType, energy_unit, unit_of_measurement_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg direct_control_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("positiveEnergyDirection", DirectControlDescriptionDataElementsType, positive_energy_direction),
    EEBUS_DATA_TAG("powerUnit", DirectControlDescriptionDataElementsType, power_unit),
    EEBUS_DATA_TAG("energyUnit", DirectControlDescriptionDataElementsType, energy_unit),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_DIRECT_CONTROL_TYPES_INC_

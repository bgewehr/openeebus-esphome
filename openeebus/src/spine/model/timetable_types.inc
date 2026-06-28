/**
 * @file
 * @brief SPINE Datagram Timetable payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_TIMETABLE_TYPES_INC_
#define SRC_SPINE_MODEL_TIMETABLE_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/timetable_types.h"

static const EnumMapping time_slot_time_mode_lut[] = {
    {"absolute", kTimeSlotTimeModeTypeAbsolute},
    {"recurring", kTimeSlotTimeModeTypeRecurring},
    {"both", kTimeSlotTimeModeTypeBoth},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg time_table_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("timeTableId", TimeTableDataType, time_table_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("timeSlotId", TimeTableDataType, time_slot_id),
    EEBUS_DATA_SEQUENCE("recurrenceInformation", TimeTableDataType, recurrence_information, recurrence_information_cfg),
    EEBUS_DATA_SEQUENCE("startTime", TimeTableDataType, start_time, absolute_or_recurring_time_cfg),
    EEBUS_DATA_SEQUENCE("endTime", TimeTableDataType, end_time, absolute_or_recurring_time_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timeTableId", TimeTableDataElementsType, time_table_id),
    EEBUS_DATA_TAG("timeSlotId", TimeTableDataElementsType, time_slot_id),
    EEBUS_DATA_SEQUENCE("recurrenceInformation", TimeTableDataElementsType, recurrence_information,
        recurrence_information_elements_cfg),
    EEBUS_DATA_SEQUENCE("startTime", TimeTableDataElementsType, start_time, absolute_or_recurring_time_elements_cfg),
    EEBUS_DATA_SEQUENCE("endTime", TimeTableDataElementsType, end_time, absolute_or_recurring_time_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TimeTableDataType), time_table_data_cfg);

static const EebusDataCfg time_table_list_data_cfg[] = {
    EEBUS_DATA_LIST("timeTableData", TimeTableListDataType, time_table_data, &time_table_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("timeTableId", TimeTableListDataSelectorsType, time_table_id),
    EEBUS_DATA_UINT32("timeSlotId", TimeTableListDataSelectorsType, time_slot_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_constraints_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "timeTableId", TimeTableConstraintsDataType, time_table_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("slotCountMin", TimeTableConstraintsDataType, slot_count_min),
    EEBUS_DATA_UINT32("slotCountMax", TimeTableConstraintsDataType, slot_count_max),
    EEBUS_DATA_DURATION("slotDurationMin", TimeTableConstraintsDataType, slot_duration_min),
    EEBUS_DATA_DURATION("slotDurationMax", TimeTableConstraintsDataType, slot_duration_max),
    EEBUS_DATA_DURATION("slotDurationStepSize", TimeTableConstraintsDataType, slot_duration_step_size),
    EEBUS_DATA_DURATION("slotShiftStepSize", TimeTableConstraintsDataType, slot_shift_step_size),
    EEBUS_DATA_TIME("firstSlotBeginsAt", TimeTableConstraintsDataType, first_slot_begins_at),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_constraints_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timeTableId", TimeTableConstraintsDataElementsType, time_table_id),
    EEBUS_DATA_TAG("slotCountMin", TimeTableConstraintsDataElementsType, slot_count_min),
    EEBUS_DATA_TAG("slotCountMax", TimeTableConstraintsDataElementsType, slot_count_max),
    EEBUS_DATA_TAG("slotDurationMin", TimeTableConstraintsDataElementsType, slot_duration_min),
    EEBUS_DATA_TAG("slotDurationMax", TimeTableConstraintsDataElementsType, slot_duration_max),
    EEBUS_DATA_TAG("slotDurationStepSize", TimeTableConstraintsDataElementsType, slot_duration_step_size),
    EEBUS_DATA_TAG("slotShiftStepSize", TimeTableConstraintsDataElementsType, slot_shift_step_size),
    EEBUS_DATA_TAG("firstSlotBeginsAt", TimeTableConstraintsDataElementsType, first_slot_begins_at),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_constraints_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TimeTableConstraintsDataType), time_table_constraints_data_cfg);

static const EebusDataCfg time_table_constraints_list_data_cfg[] = {
    EEBUS_DATA_LIST("timeTableConstraintsData", TimeTableConstraintsListDataType, time_table_constraints_data,
        &time_table_constraints_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_constraints_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("timeTableId", TimeTableConstraintsListDataSelectorsType, time_table_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "timeTableId", TimeTableDescriptionDataType, time_table_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_BOOL("timeSlotCountChangeable", TimeTableDescriptionDataType, time_slot_count_changeable),
    EEBUS_DATA_BOOL("timeSlotTimesChangeable", TimeTableDescriptionDataType, time_slot_times_changeable),
    EEBUS_DATA_ENUM("timeSlotTimeMode", TimeTableDescriptionDataType, time_slot_time_mode, time_slot_time_mode_lut),
    EEBUS_DATA_STRING("label", TimeTableDescriptionDataType, label),
    EEBUS_DATA_STRING("description", TimeTableDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timeTableId", TimeTableDescriptionDataElementsType, time_table_id),
    EEBUS_DATA_TAG("timeSlotCountChangeable", TimeTableDescriptionDataElementsType, time_slot_count_changeable),
    EEBUS_DATA_TAG("timeSlotTimesChangeable", TimeTableDescriptionDataElementsType, time_slot_times_changeable),
    EEBUS_DATA_TAG("timeSlotTimeMode", TimeTableDescriptionDataElementsType, time_slot_time_mode),
    EEBUS_DATA_TAG("label", TimeTableDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", TimeTableDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_description_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TimeTableDescriptionDataType), time_table_description_data_cfg);

static const EebusDataCfg time_table_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("timeTableDescriptionData", TimeTableDescriptionListDataType, time_table_description_data,
        &time_table_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_table_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("timeTableId", TimeTableDescriptionListDataSelectorsType, time_table_id),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_TIMETABLE_TYPES_INC_

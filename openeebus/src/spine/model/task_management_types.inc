/**
 * @file
 * @brief SPINE Datagram Task Management payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_TASK_MANAGEMENT_TYPES_INC_
#define SRC_SPINE_MODEL_TASK_MANAGEMENT_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/hvac_types.inc"
#include "src/spine/model/loadcontrol_types.inc"
#include "src/spine/model/power_sequences_types.inc"
#include "src/spine/model/task_management_types.h"

static const EnumMapping task_management_job_state_lut[] = {
    {"Running", kTaskManagementJobStateTypeRunning},
    {"paused", kTaskManagementJobStateTypePaused},
    {"inactive", kTaskManagementJobStateTypeInactive},
    {"active", kTaskManagementJobStateTypeActive},
    {"finished", kTaskManagementJobStateTypeFinished},
    {"eventAccepted", kTaskManagementJobStateTypeEventAccepted},
    {"eventStarted", kTaskManagementJobStateTypeEventStarted},
    {"eventStopped", kTaskManagementJobStateTypeEventStopped},
    {"eventRejected", kTaskManagementJobStateTypeEventRejected},
    {"eventCancelled", kTaskManagementJobStateTypeEventCancelled},
    {"eventError", kTaskManagementJobStateTypeEventError},
    {"scheduled", kTaskManagementJobStateTypeScheduled},
    {"scheduledPaused", kTaskManagementJobStateTypeScheduledPaused},
    {"pending", kTaskManagementJobStateTypePending},
    {"completed", kTaskManagementJobStateTypeCompleted},
    {"invalid", kTaskManagementJobStateTypeInvalid},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping task_management_job_source_lut[] = {
    {"InternalMechanism", kTaskManagementJobSourceTypeInternalMechanism},
    {"UserInteraction", kTaskManagementJobSourceTypeUserInteraction},
    {"ExternalConfiguration", kTaskManagementJobSourceTypeExternalConfiguration},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg task_management_hvac_related_cfg[] = {
    EEBUS_DATA_UINT32("overrunId", TaskManagementHvacRelatedType, overrun_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_hvac_related_elements_cfg[] = {
    EEBUS_DATA_TAG("overrunId", TaskManagementHvacRelatedElementsType, overrun_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_load_control_releated_cfg[] = {
    EEBUS_DATA_UINT32("eventId", TaskManagementLoadControlReleatedType, event_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_load_control_releated_elements_cfg[] = {
    EEBUS_DATA_TAG("eventId", TaskManagementLoadControlReleatedElementsType, event_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_power_sequences_related_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", TaskManagementPowerSequencesRelatedType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_power_sequences_related_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", TaskManagementPowerSequencesRelatedElementsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_smart_energy_management_ps_related_cfg[] = {
    EEBUS_DATA_UINT32("sequenceId", TaskManagementSmartEnergyManagementPsRelatedType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_smart_energy_management_ps_related_elements_cfg[] = {
    EEBUS_DATA_TAG("sequenceId", TaskManagementSmartEnergyManagementPsRelatedElementsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("jobId", TaskManagementJobDataType, job_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", TaskManagementJobDataType, timestamp),
    EEBUS_DATA_ENUM("jobState", TaskManagementJobDataType, job_state, task_management_job_state_lut),
    EEBUS_DATA_DURATION("elapsedTime", TaskManagementJobDataType, elapsed_time),
    EEBUS_DATA_DURATION("remainingTime", TaskManagementJobDataType, remaining_time),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_data_elements_cfg[] = {
    EEBUS_DATA_TAG("jobId", TaskManagementJobDataElementsType, job_id),
    EEBUS_DATA_TAG("timestamp", TaskManagementJobDataElementsType, timestamp),
    EEBUS_DATA_TAG("jobState", TaskManagementJobDataElementsType, job_state),
    EEBUS_DATA_TAG("elapsedTime", TaskManagementJobDataElementsType, elapsed_time),
    EEBUS_DATA_TAG("remainingTime", TaskManagementJobDataElementsType, remaining_time),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(TaskManagementJobDataType), task_management_job_data_cfg);

static const EebusDataCfg task_management_job_list_data_cfg[] = {
    EEBUS_DATA_LIST("taskManagementJobData", TaskManagementJobListDataType, task_management_job_data,
        &task_management_job_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("jobId", TaskManagementJobListDataSelectorsType, job_id),
    EEBUS_DATA_ENUM("jobState", TaskManagementJobListDataSelectorsType, job_state, task_management_job_state_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("jobId", TaskManagementJobRelationDataType, job_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_TAG("directControlRelated", TaskManagementJobRelationDataType, direct_control_related),
    EEBUS_DATA_SEQUENCE(
        "hvacRelated", TaskManagementJobRelationDataType, hvac_related, task_management_hvac_related_cfg),
    EEBUS_DATA_SEQUENCE("loadControlReleated", TaskManagementJobRelationDataType, load_control_releated,
        task_management_load_control_releated_cfg),
    EEBUS_DATA_SEQUENCE("powerSequencesRelated", TaskManagementJobRelationDataType, power_sequences_related,
        task_management_power_sequences_related_cfg),
    EEBUS_DATA_SEQUENCE("smartEnergyManagementPsRelated", TaskManagementJobRelationDataType,
        smart_energy_management_ps_related, task_management_smart_energy_management_ps_related_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("jobId", TaskManagementJobRelationDataElementsType, job_id),
    EEBUS_DATA_TAG("directControlRelated", TaskManagementJobRelationDataElementsType, direct_control_related),
    EEBUS_DATA_SEQUENCE("hvacRelated", TaskManagementJobRelationDataElementsType, hvac_related,
        task_management_hvac_related_elements_cfg),
    EEBUS_DATA_SEQUENCE("loadControlReleated", TaskManagementJobRelationDataElementsType, load_control_releated,
        task_management_load_control_releated_elements_cfg),
    EEBUS_DATA_SEQUENCE("powerSequencesRelated", TaskManagementJobRelationDataElementsType, power_sequences_related,
        task_management_power_sequences_related_elements_cfg),
    EEBUS_DATA_SEQUENCE("smartEnergyManagementPsRelated", TaskManagementJobRelationDataElementsType,
        smart_energy_management_ps_related, task_management_smart_energy_management_ps_related_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_relation_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(TaskManagementJobRelationDataType), task_management_job_relation_data_cfg);

static const EebusDataCfg task_management_job_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("taskManagementJobRelationData", TaskManagementJobRelationListDataType,
        task_management_job_relation_data, &task_management_job_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("jobId", TaskManagementJobRelationListDataSelectorsType, job_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_description_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("jobId", TaskManagementJobDescriptionDataType, job_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("jobSource", TaskManagementJobDescriptionDataType, job_source, task_management_job_source_lut),
    EEBUS_DATA_STRING("label", TaskManagementJobDescriptionDataType, label),
    EEBUS_DATA_STRING("description", TaskManagementJobDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("jobId", TaskManagementJobDescriptionDataElementsType, job_id),
    EEBUS_DATA_TAG("jobSource", TaskManagementJobDescriptionDataElementsType, job_source),
    EEBUS_DATA_TAG("label", TaskManagementJobDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", TaskManagementJobDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_description_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(TaskManagementJobDescriptionDataType), task_management_job_description_data_cfg);

static const EebusDataCfg task_management_job_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("taskManagementJobDescriptionData", TaskManagementJobDescriptionListDataType,
        task_management_job_description_data, &task_management_job_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_job_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("jobId", TaskManagementJobDescriptionListDataSelectorsType, job_id),
    EEBUS_DATA_ENUM(
        "jobSource", TaskManagementJobDescriptionListDataSelectorsType, job_source, task_management_job_source_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_overview_data_cfg[] = {
    EEBUS_DATA_BOOL("remoteControllable", TaskManagementOverviewDataType, remote_controllable),
    EEBUS_DATA_BOOL("jobsActive", TaskManagementOverviewDataType, jobs_active),
    EEBUS_DATA_END,
};

static const EebusDataCfg task_management_overview_data_elements_cfg[] = {
    EEBUS_DATA_TAG("remoteControllable", TaskManagementOverviewDataElementsType, remote_controllable),
    EEBUS_DATA_TAG("jobsActive", TaskManagementOverviewDataElementsType, jobs_active),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_TASK_MANAGEMENT_TYPES_INC_

/**
 * @file
 * @brief SPINE Datagram Identification payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_IDENTIFICATION_TYPES_INC_
#define SRC_SPINE_MODEL_IDENTIFICATION_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/identification_types.h"
#include "src/spine/model/measurement_types.inc"

static const EnumMapping identification_type_lut[] = {
    {"eui48", kIdentificationTypeTypeEui48},
    {"eui64", kIdentificationTypeTypeEui64},
    {"userRfidTag", kIdentificationTypeTypeUserrfidtag},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg identification_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "identificationId", IdentificationDataType, identification_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("identificationType", IdentificationDataType, identification_type, identification_type_lut),
    EEBUS_DATA_STRING("identificationValue", IdentificationDataType, identification_value),
    EEBUS_DATA_BOOL("authorized", IdentificationDataType, authorized),
    EEBUS_DATA_END,
};

static const EebusDataCfg identification_data_elements_cfg[] = {
    EEBUS_DATA_TAG("identificationId", IdentificationDataElementsType, identification_id),
    EEBUS_DATA_TAG("identificationType", IdentificationDataElementsType, identification_type),
    EEBUS_DATA_TAG("identificationValue", IdentificationDataElementsType, identification_value),
    EEBUS_DATA_TAG("authorized", IdentificationDataElementsType, authorized),
    EEBUS_DATA_END,
};

static const EebusDataCfg identification_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(IdentificationDataType), identification_data_cfg);

static const EebusDataCfg identification_list_data_cfg[] = {
    EEBUS_DATA_LIST(
        "identificationData", IdentificationListDataType, identification_data, &identification_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg identification_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("identificationId", IdentificationListDataSelectorsType, identification_id),
    EEBUS_DATA_ENUM(
        "identificationType", IdentificationListDataSelectorsType, identification_type, identification_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg session_identification_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("sessionId", SessionIdentificationDataType, session_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_UINT32("identificationId", SessionIdentificationDataType, identification_id),
    EEBUS_DATA_BOOL("isLatestSession", SessionIdentificationDataType, is_latest_session),
    EEBUS_DATA_SEQUENCE("timePeriod", SessionIdentificationDataType, time_period, time_period_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg session_identification_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sessionId", SessionIdentificationDataElementsType, session_id),
    EEBUS_DATA_TAG("identificationId", SessionIdentificationDataElementsType, identification_id),
    EEBUS_DATA_TAG("isLatestSession", SessionIdentificationDataElementsType, is_latest_session),
    EEBUS_DATA_SEQUENCE("timePeriod", SessionIdentificationDataElementsType, time_period, time_period_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg session_identification_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(SessionIdentificationDataType), session_identification_data_cfg);

static const EebusDataCfg session_identification_list_data_cfg[] = {
    EEBUS_DATA_LIST("sessionIdentificationData", SessionIdentificationListDataType, session_identification_data,
        &session_identification_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg session_identification_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sessionId", SessionIdentificationListDataSelectorsType, session_id),
    EEBUS_DATA_UINT32("identificationId", SessionIdentificationListDataSelectorsType, identification_id),
    EEBUS_DATA_BOOL("isLatestSession", SessionIdentificationListDataSelectorsType, is_latest_session),
    EEBUS_DATA_SEQUENCE("timePeriod", SessionIdentificationListDataSelectorsType, time_period, time_period_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg measurement_id_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint32_t), &json_num_conv_uint32);

static const EebusDataCfg session_measurement_relation_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "sessionId", SessionMeasurementRelationDataType, session_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_LIST(
        "measurementId", SessionMeasurementRelationDataType, measurement_id, &measurement_id_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg session_measurement_relation_data_elements_cfg[] = {
    EEBUS_DATA_TAG("sessionId", SessionMeasurementRelationDataElementsType, session_id),
    EEBUS_DATA_TAG("measurementId", SessionMeasurementRelationDataElementsType, measurement_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg session_measurement_relation_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(SessionMeasurementRelationDataType), session_measurement_relation_data_cfg);

static const EebusDataCfg session_measurement_relation_list_data_cfg[] = {
    EEBUS_DATA_LIST("sessionMeasurementRelationData", SessionMeasurementRelationListDataType,
        session_measurement_relation_data, &session_measurement_relation_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg session_measurement_relation_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("sessionId", SessionMeasurementRelationListDataSelectorsType, session_id),
    EEBUS_DATA_UINT32("measurementId", SessionMeasurementRelationListDataSelectorsType, measurement_id),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_IDENTIFICATION_TYPES_INC_

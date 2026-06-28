/**
 * @file
 * @brief SPINE Datagram Messaging payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_MESSAGING_TYPES_INC_
#define SRC_SPINE_MODEL_MESSAGING_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/messaging_types.h"

static const EnumMapping messaging_type_lut[] = {
    {"logging", kMessagingTypeTypeLogging},
    {"information", kMessagingTypeTypeInformation},
    {"warning", kMessagingTypeTypeWarning},
    {"alarm", kMessagingTypeTypeAlarm},
    {"emergency", kMessagingTypeTypeEmergency},
    {"obsolete", kMessagingTypeTypeObsolete},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg messaging_data_cfg[] = {
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", MessagingDataType, timestamp),
    EEBUS_DATA_UINT32_WITH_FLAGS("messagingNumber", MessagingDataType, messaging_number, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("type", MessagingDataType, messaging_type, messaging_type_lut),
    EEBUS_DATA_STRING("text", MessagingDataType, text),
    EEBUS_DATA_END,
};

static const EebusDataCfg messaging_data_elements_cfg[] = {
    EEBUS_DATA_TAG("timestamp", MessagingDataElementsType, timestamp),
    EEBUS_DATA_TAG("messagingNumber", MessagingDataElementsType, messaging_number),
    EEBUS_DATA_TAG("type", MessagingDataElementsType, messaging_type),
    EEBUS_DATA_TAG("text", MessagingDataElementsType, text),
    EEBUS_DATA_END,
};

static const EebusDataCfg messaging_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(MessagingDataType), messaging_data_cfg);

static const EebusDataCfg messaging_list_data_cfg[] = {
    EEBUS_DATA_LIST("messagingData", MessagingListDataType, messaging_data, &messaging_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg messaging_list_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "timestampInterval", MessagingListDataSelectorsType, timestamp_interval, timestamp_interval_cfg),
    EEBUS_DATA_UINT32("messagingNumber", MessagingListDataSelectorsType, messaging_number),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_MESSAGING_TYPES_INC_

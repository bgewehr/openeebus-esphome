/**
 * @file
 * @brief SPINE Datagram Subscription Management payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_SUBSCRIPTION_MANAGEMENT_TYPES_INC_
#define SRC_SPINE_MODEL_SUBSCRIPTION_MANAGEMENT_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/feature_types.inc"
#include "src/spine/model/subscription_management_types.h"

static const EebusDataCfg subscription_management_entry_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS(
        "subscriptionId", SubscriptionManagementEntryDataType, subscription_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("clientAddress", SubscriptionManagementEntryDataType, client_address, feature_address_cfg),
    EEBUS_DATA_SEQUENCE("serverAddress", SubscriptionManagementEntryDataType, server_address, feature_address_cfg),
    EEBUS_DATA_STRING("label", SubscriptionManagementEntryDataType, label),
    EEBUS_DATA_STRING("description", SubscriptionManagementEntryDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg subscription_management_entry_data_elements_cfg[] = {
    EEBUS_DATA_TAG("subscriptionId", SubscriptionManagementEntryDataElementsType, subscription_id),
    EEBUS_DATA_SEQUENCE(
        "clientAddress", SubscriptionManagementEntryDataElementsType, client_address, feature_address_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "serverAddress", SubscriptionManagementEntryDataElementsType, server_address, feature_address_elements_cfg),
    EEBUS_DATA_TAG("label", SubscriptionManagementEntryDataElementsType, label),
    EEBUS_DATA_TAG("description", SubscriptionManagementEntryDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg subscription_management_entry_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(SubscriptionManagementEntryDataType), subscription_management_entry_data_cfg);

static const EebusDataCfg subscription_management_entry_list_data_cfg[] = {
    EEBUS_DATA_LIST("subscriptionManagementEntryData", SubscriptionManagementEntryListDataType,
        subscription_management_entry_data, &subscription_management_entry_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg subscription_management_entry_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("subscriptionId", SubscriptionManagementEntryListDataSelectorsType, subscription_id),
    EEBUS_DATA_SEQUENCE(
        "clientAddress", SubscriptionManagementEntryListDataSelectorsType, client_address, feature_address_cfg),
    EEBUS_DATA_SEQUENCE(
        "serverAddress", SubscriptionManagementEntryListDataSelectorsType, server_address, feature_address_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg subscription_management_request_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("clientAddress", SubscriptionManagementRequestCallType, client_address, feature_address_cfg),
    EEBUS_DATA_SEQUENCE("serverAddress", SubscriptionManagementRequestCallType, server_address, feature_address_cfg),
    EEBUS_DATA_ENUM("serverFeatureType", SubscriptionManagementRequestCallType, server_feature_type, feature_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg subscription_management_request_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "clientAddress", SubscriptionManagementRequestCallElementsType, client_address, feature_address_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "serverAddress", SubscriptionManagementRequestCallElementsType, server_address, feature_address_elements_cfg),
    EEBUS_DATA_TAG("serverFeatureType", SubscriptionManagementRequestCallElementsType, server_feature_type),
    EEBUS_DATA_END,
};

static const EebusDataCfg subscription_management_delete_call_cfg[] = {
    EEBUS_DATA_UINT32("subscriptionId", SubscriptionManagementDeleteCallType, subscription_id),
    EEBUS_DATA_SEQUENCE("clientAddress", SubscriptionManagementDeleteCallType, client_address, feature_address_cfg),
    EEBUS_DATA_SEQUENCE("serverAddress", SubscriptionManagementDeleteCallType, server_address, feature_address_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg subscription_management_delete_call_elements_cfg[] = {
    EEBUS_DATA_TAG("subscriptionId", SubscriptionManagementDeleteCallElementsType, subscription_id),
    EEBUS_DATA_SEQUENCE(
        "clientAddress", SubscriptionManagementDeleteCallElementsType, client_address, feature_address_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "serverAddress", SubscriptionManagementDeleteCallElementsType, server_address, feature_address_elements_cfg),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_SUBSCRIPTION_MANAGEMENT_TYPES_INC_

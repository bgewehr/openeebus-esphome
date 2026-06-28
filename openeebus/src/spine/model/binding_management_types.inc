/**
 * @file
 * @brief SPINE Datagram Binding Management payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_BINDING_MANAGEMENT_TYPES_INC_
#define SRC_SPINE_MODEL_BINDING_MANAGEMENT_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/binding_management_types.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/feature_types.inc"

static const EebusDataCfg binding_management_entry_data_cfg[] = {
    EEBUS_DATA_UINT32_WITH_FLAGS("bindingId", BindingManagementEntryDataType, binding_id, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_SEQUENCE("clientAddress", BindingManagementEntryDataType, client_address, feature_address_cfg),
    EEBUS_DATA_SEQUENCE("serverAddress", BindingManagementEntryDataType, server_address, feature_address_cfg),
    EEBUS_DATA_STRING("label", BindingManagementEntryDataType, label),
    EEBUS_DATA_STRING("description", BindingManagementEntryDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg binding_management_entry_data_elements_cfg[] = {
    EEBUS_DATA_TAG("bindingId", BindingManagementEntryDataElementsType, binding_id),
    EEBUS_DATA_TAG("clientAddress", BindingManagementEntryDataElementsType, client_address),
    EEBUS_DATA_TAG("serverAddress", BindingManagementEntryDataElementsType, server_address),
    EEBUS_DATA_TAG("label", BindingManagementEntryDataElementsType, label),
    EEBUS_DATA_TAG("description", BindingManagementEntryDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg binding_management_entry_data_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(BindingManagementEntryDataType), binding_management_entry_data_cfg);

static const EebusDataCfg binding_management_entry_list_data_cfg[] = {
    EEBUS_DATA_LIST("bindingManagementEntryData", BindingManagementEntryListDataType, binding_management_entry_data,
        &binding_management_entry_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg binding_management_entry_list_data_selectors_cfg[] = {
    EEBUS_DATA_UINT32("bindingId", BindingManagementEntryListDataSelectorsType, binding_id),
    EEBUS_DATA_SEQUENCE(
        "clientAddress", BindingManagementEntryListDataSelectorsType, client_address, feature_address_cfg),
    EEBUS_DATA_SEQUENCE(
        "serverAddress", BindingManagementEntryListDataSelectorsType, server_address, feature_address_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg binding_management_request_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("clientAddress", BindingManagementRequestCallType, client_address, feature_address_cfg),
    EEBUS_DATA_SEQUENCE("serverAddress", BindingManagementRequestCallType, server_address, feature_address_cfg),
    EEBUS_DATA_ENUM("serverFeatureType", BindingManagementRequestCallType, server_feature_type, feature_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg binding_management_request_call_elements_cfg[] = {
    EEBUS_DATA_TAG("clientAddress", BindingManagementRequestCallElementsType, client_address),
    EEBUS_DATA_TAG("serverAddress", BindingManagementRequestCallElementsType, server_address),
    EEBUS_DATA_TAG("serverFeatureType", BindingManagementRequestCallElementsType, server_feature_type),
    EEBUS_DATA_END,
};

static const EebusDataCfg binding_management_delete_call_cfg[] = {
    EEBUS_DATA_UINT32("bindingId", BindingManagementDeleteCallType, binding_id),
    EEBUS_DATA_SEQUENCE("clientAddress", BindingManagementDeleteCallType, client_address, feature_address_cfg),
    EEBUS_DATA_SEQUENCE("serverAddress", BindingManagementDeleteCallType, server_address, feature_address_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg binding_management_delete_call_elements_cfg[] = {
    EEBUS_DATA_TAG("bindingId", BindingManagementDeleteCallElementsType, binding_id),
    EEBUS_DATA_TAG("clientAddress", BindingManagementDeleteCallElementsType, client_address),
    EEBUS_DATA_TAG("serverAddress", BindingManagementDeleteCallElementsType, server_address),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_BINDING_MANAGEMENT_TYPES_INC_

/**
 * @file
 * @brief SPINE Datagram Network Management payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_NETWORK_MANAGEMENT_TYPES_INC_
#define SRC_SPINE_MODEL_NETWORK_MANAGEMENT_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/device_types.inc"
#include "src/spine/model/entity_types.inc"
#include "src/spine/model/feature_types.inc"
#include "src/spine/model/function_types.inc"
#include "src/spine/model/network_management_types.h"

static const EnumMapping network_management_feature_set_lut[] = {
    {"gateway", kNetworkManagementFeatureSetTypeGateway},
    {"router", kNetworkManagementFeatureSetTypeRouter},
    {"smart", kNetworkManagementFeatureSetTypeSmart},
    {"simple", kNetworkManagementFeatureSetTypeSimple},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping network_management_process_state_state_lut[] = {
    {"succeeded", kNetworkManagementProcessStateStateTypeSucceeded},
    {"failed", kNetworkManagementProcessStateStateTypeFailed},
    {"aborted", kNetworkManagementProcessStateStateTypeAborted},
    EEBUS_ENUM_MAPPING_END,
};

static const EnumMapping network_management_state_change_lut[] = {
    {"added", kNetworkManagementStateChangeTypeAdded},
    {"removed", kNetworkManagementStateChangeTypeRemoved},
    {"modified", kNetworkManagementStateChangeTypeModified},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg network_management_add_node_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("nodeAddress", NetworkManagementAddNodeCallType, node_address, feature_address_cfg),
    EEBUS_DATA_STRING("nativeSetup", NetworkManagementAddNodeCallType, native_setup),
    EEBUS_DATA_DURATION("timeout", NetworkManagementAddNodeCallType, timeout),
    EEBUS_DATA_STRING("label", NetworkManagementAddNodeCallType, label),
    EEBUS_DATA_STRING("description", NetworkManagementAddNodeCallType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_add_node_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "nodeAddress", NetworkManagementAddNodeCallElementsType, node_address, feature_address_elements_cfg),
    EEBUS_DATA_TAG("nativeSetup", NetworkManagementAddNodeCallElementsType, native_setup),
    EEBUS_DATA_TAG("timeout", NetworkManagementAddNodeCallElementsType, timeout),
    EEBUS_DATA_TAG("label", NetworkManagementAddNodeCallElementsType, label),
    EEBUS_DATA_TAG("description", NetworkManagementAddNodeCallElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_remove_node_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("nodeAddress", NetworkManagementRemoveNodeCallType, node_address, feature_address_cfg),
    EEBUS_DATA_DURATION("timeout", NetworkManagementRemoveNodeCallType, timeout),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_remove_node_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "nodeAddress", NetworkManagementRemoveNodeCallElementsType, node_address, feature_address_elements_cfg),
    EEBUS_DATA_TAG("timeout", NetworkManagementRemoveNodeCallElementsType, timeout),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_modify_node_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("nodeAddress", NetworkManagementModifyNodeCallType, node_address, feature_address_cfg),
    EEBUS_DATA_STRING("nativeSetup", NetworkManagementModifyNodeCallType, native_setup),
    EEBUS_DATA_DURATION("timeout", NetworkManagementModifyNodeCallType, timeout),
    EEBUS_DATA_STRING("label", NetworkManagementModifyNodeCallType, label),
    EEBUS_DATA_STRING("description", NetworkManagementModifyNodeCallType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_modify_node_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "nodeAddress", NetworkManagementModifyNodeCallElementsType, node_address, feature_address_elements_cfg),
    EEBUS_DATA_TAG("nativeSetup", NetworkManagementModifyNodeCallElementsType, native_setup),
    EEBUS_DATA_TAG("timeout", NetworkManagementModifyNodeCallElementsType, timeout),
    EEBUS_DATA_TAG("label", NetworkManagementModifyNodeCallElementsType, label),
    EEBUS_DATA_TAG("description", NetworkManagementModifyNodeCallElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_scan_network_call_cfg[] = {
    EEBUS_DATA_STRING("scanSetup", NetworkManagementScanNetworkCallType, scan_setup),
    EEBUS_DATA_DURATION("timeout", NetworkManagementScanNetworkCallType, timeout),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_scan_network_call_elements_cfg[] = {
    EEBUS_DATA_TAG("scanSetup", NetworkManagementScanNetworkCallElementsType, scan_setup),
    EEBUS_DATA_TAG("timeout", NetworkManagementScanNetworkCallElementsType, timeout),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_discover_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("discoverAddress", NetworkManagementDiscoverCallType, discover_address, feature_address_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_discover_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "discoverAddress", NetworkManagementDiscoverCallElementsType, discover_address, feature_address_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_process_state_data_cfg[] = {
    EEBUS_DATA_ENUM("state", NetworkManagementProcessStateDataType, state, network_management_process_state_state_lut),
    EEBUS_DATA_STRING("description", NetworkManagementProcessStateDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_process_state_data_elements_cfg[] = {
    EEBUS_DATA_TAG("state", NetworkManagementProcessStateDataElementsType, state),
    EEBUS_DATA_TAG("description", NetworkManagementProcessStateDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_joining_mode_data_cfg[] = {
    EEBUS_DATA_STRING("setup", NetworkManagementJoiningModeDataType, setup),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_joining_mode_data_elements_cfg[] = {
    EEBUS_DATA_TAG("setup", NetworkManagementJoiningModeDataElementsType, setup),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_report_candidate_data_cfg[] = {
    EEBUS_DATA_STRING("candidateSetup", NetworkManagementReportCandidateDataType, candidate_setup),
    EEBUS_DATA_BOOL("setupUsableForAdd", NetworkManagementReportCandidateDataType, setup_usable_for_add),
    EEBUS_DATA_STRING("label", NetworkManagementReportCandidateDataType, label),
    EEBUS_DATA_STRING("description", NetworkManagementReportCandidateDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_report_candidate_data_elements_cfg[] = {
    EEBUS_DATA_TAG("candidateSetup", NetworkManagementReportCandidateDataElementsType, candidate_setup),
    EEBUS_DATA_TAG("setupUsableForAdd", NetworkManagementReportCandidateDataElementsType, setup_usable_for_add),
    EEBUS_DATA_TAG("label", NetworkManagementReportCandidateDataElementsType, label),
    EEBUS_DATA_TAG("description", NetworkManagementReportCandidateDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_device_description_data_cfg[] = {
    EEBUS_DATA_SEQUENCE_WITH_FLAGS("deviceAddress", NetworkManagementDeviceDescriptionDataType, device_address,
        device_address_cfg, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("deviceType", NetworkManagementDeviceDescriptionDataType, device_type, device_type_lut),
    EEBUS_DATA_SEQUENCE("networkManagementResponsibleAddress", NetworkManagementDeviceDescriptionDataType,
        network_management_responsible_address, feature_address_cfg),
    EEBUS_DATA_STRING("nativeSetup", NetworkManagementDeviceDescriptionDataType, native_setup),
    EEBUS_DATA_STRING("technologyAddress", NetworkManagementDeviceDescriptionDataType, technology_address),
    EEBUS_DATA_STRING("communicationsTechnologyInformation", NetworkManagementDeviceDescriptionDataType,
        communications_technology_information),
    EEBUS_DATA_ENUM("networkFeatureSet", NetworkManagementDeviceDescriptionDataType, network_feature_set,
        network_management_feature_set_lut),
    EEBUS_DATA_ENUM("lastStateChange", NetworkManagementDeviceDescriptionDataType, last_state_change,
        network_management_state_change_lut),
    EEBUS_DATA_STRING("minimumTrustLevel", NetworkManagementDeviceDescriptionDataType, minimum_trust_level),
    EEBUS_DATA_STRING("label", NetworkManagementDeviceDescriptionDataType, label),
    EEBUS_DATA_STRING("description", NetworkManagementDeviceDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_device_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("deviceAddress", NetworkManagementDeviceDescriptionDataElementsType, device_address),
    EEBUS_DATA_TAG("deviceType", NetworkManagementDeviceDescriptionDataElementsType, device_type),
    EEBUS_DATA_TAG("networkManagementResponsibleAddress", NetworkManagementDeviceDescriptionDataElementsType,
        network_management_responsible_address),
    EEBUS_DATA_TAG("nativeSetup", NetworkManagementDeviceDescriptionDataElementsType, native_setup),
    EEBUS_DATA_TAG("technologyAddress", NetworkManagementDeviceDescriptionDataElementsType, technology_address),
    EEBUS_DATA_TAG("communicationsTechnologyInformation", NetworkManagementDeviceDescriptionDataElementsType,
        communications_technology_information),
    EEBUS_DATA_TAG("networkFeatureSet", NetworkManagementDeviceDescriptionDataElementsType, network_feature_set),
    EEBUS_DATA_TAG("lastStateChange", NetworkManagementDeviceDescriptionDataElementsType, last_state_change),
    EEBUS_DATA_TAG("minimumTrustLevel", NetworkManagementDeviceDescriptionDataElementsType, minimum_trust_level),
    EEBUS_DATA_TAG("label", NetworkManagementDeviceDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", NetworkManagementDeviceDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_device_description_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(NetworkManagementDeviceDescriptionDataType), network_management_device_description_data_cfg);

static const EebusDataCfg network_management_device_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("networkManagementDeviceDescriptionData", NetworkManagementDeviceDescriptionListDataType,
        network_management_device_description_data, &network_management_device_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_device_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "deviceAddress", NetworkManagementDeviceDescriptionListDataSelectorsType, device_address, device_address_cfg),
    EEBUS_DATA_ENUM(
        "deviceType", NetworkManagementDeviceDescriptionListDataSelectorsType, device_type, device_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_entity_description_data_cfg[] = {
    EEBUS_DATA_SEQUENCE_WITH_FLAGS("entityAddress", NetworkManagementEntityDescriptionDataType, entity_address,
        entity_address_cfg, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("entityType", NetworkManagementEntityDescriptionDataType, entity_type, entity_type_lut),
    EEBUS_DATA_ENUM("lastStateChange", NetworkManagementEntityDescriptionDataType, last_state_change,
        network_management_state_change_lut),
    EEBUS_DATA_STRING("minimumTrustLevel", NetworkManagementEntityDescriptionDataType, minimum_trust_level),
    EEBUS_DATA_STRING("label", NetworkManagementEntityDescriptionDataType, label),
    EEBUS_DATA_STRING("description", NetworkManagementEntityDescriptionDataType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_entity_description_data_elements_cfg[] = {
    EEBUS_DATA_TAG("entityAddress", NetworkManagementEntityDescriptionDataElementsType, entity_address),
    EEBUS_DATA_TAG("entityType", NetworkManagementEntityDescriptionDataElementsType, entity_type),
    EEBUS_DATA_TAG("lastStateChange", NetworkManagementEntityDescriptionDataElementsType, last_state_change),
    EEBUS_DATA_TAG("minimumTrustLevel", NetworkManagementEntityDescriptionDataElementsType, minimum_trust_level),
    EEBUS_DATA_TAG("label", NetworkManagementEntityDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", NetworkManagementEntityDescriptionDataElementsType, description),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_entity_description_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(NetworkManagementEntityDescriptionDataType), network_management_entity_description_data_cfg);

static const EebusDataCfg network_management_entity_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("networkManagementEntityDescriptionData", NetworkManagementEntityDescriptionListDataType,
        network_management_entity_description_data, &network_management_entity_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_entity_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "entityAddress", NetworkManagementEntityDescriptionListDataSelectorsType, entity_address, entity_address_cfg),
    EEBUS_DATA_ENUM(
        "entityType", NetworkManagementEntityDescriptionListDataSelectorsType, entity_type, entity_type_lut),
    EEBUS_DATA_END,
};

static const EebusDataCfg specific_usage_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(enum, sizeof(int32_t), NULL);

static const EebusDataCfg supported_function_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(FunctionPropertyType), function_property_cfg);

static const EebusDataCfg network_management_feature_description_data_cfg[] = {
    EEBUS_DATA_SEQUENCE_WITH_FLAGS("featureAddress", NetworkManagementFeatureDescriptionDataType, feature_address,
        feature_address_cfg, kEebusDataFlagIsIdentifier),
    EEBUS_DATA_ENUM("featureType", NetworkManagementFeatureDescriptionDataType, feature_type, feature_type_lut),
    EEBUS_DATA_LIST(
        "specificUsage", NetworkManagementFeatureDescriptionDataType, specific_usage, &specific_usage_element_data_cfg),
    EEBUS_DATA_STRING("featureGroup", NetworkManagementFeatureDescriptionDataType, feature_group),
    EEBUS_DATA_ENUM("role", NetworkManagementFeatureDescriptionDataType, role, role_lut),
    EEBUS_DATA_LIST("supportedFunction", NetworkManagementFeatureDescriptionDataType, supported_function,
        &supported_function_element_data_cfg),
    EEBUS_DATA_ENUM("lastStateChange", NetworkManagementFeatureDescriptionDataType, last_state_change,
        network_management_state_change_lut),
    EEBUS_DATA_STRING("minimumTrustLevel", NetworkManagementFeatureDescriptionDataType, minimum_trust_level),
    EEBUS_DATA_STRING("label", NetworkManagementFeatureDescriptionDataType, label),
    EEBUS_DATA_STRING("description", NetworkManagementFeatureDescriptionDataType, description),
    EEBUS_DATA_DURATION("maxResponseDelay", NetworkManagementFeatureDescriptionDataType, max_response_delay),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_feature_description_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("featureAddress", NetworkManagementFeatureDescriptionDataElementsType, feature_address,
        feature_address_elements_cfg),
    EEBUS_DATA_TAG("featureType", NetworkManagementFeatureDescriptionDataElementsType, feature_type),
    EEBUS_DATA_TAG("specificUsage", NetworkManagementFeatureDescriptionDataElementsType, specific_usage),
    EEBUS_DATA_TAG("featureGroup", NetworkManagementFeatureDescriptionDataElementsType, feature_group),
    EEBUS_DATA_TAG("role", NetworkManagementFeatureDescriptionDataElementsType, role),
    EEBUS_DATA_SEQUENCE("supportedFunction", NetworkManagementFeatureDescriptionDataElementsType, supported_function,
        function_property_elements_cfg),
    EEBUS_DATA_TAG("lastStateChange", NetworkManagementFeatureDescriptionDataElementsType, last_state_change),
    EEBUS_DATA_TAG("minimumTrustLevel", NetworkManagementFeatureDescriptionDataElementsType, minimum_trust_level),
    EEBUS_DATA_TAG("label", NetworkManagementFeatureDescriptionDataElementsType, label),
    EEBUS_DATA_TAG("description", NetworkManagementFeatureDescriptionDataElementsType, description),
    EEBUS_DATA_TAG("maxResponseDelay", NetworkManagementFeatureDescriptionDataElementsType, max_response_delay),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_feature_description_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(NetworkManagementFeatureDescriptionDataType), network_management_feature_description_data_cfg);

static const EebusDataCfg network_management_feature_description_list_data_cfg[] = {
    EEBUS_DATA_LIST("networkManagementFeatureDescriptionData", NetworkManagementFeatureDescriptionListDataType,
        network_management_feature_description_data, &network_management_feature_description_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg network_management_feature_description_list_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("featureAddress", NetworkManagementFeatureDescriptionListDataSelectorsType, feature_address,
        feature_address_cfg),
    EEBUS_DATA_ENUM(
        "featureType", NetworkManagementFeatureDescriptionListDataSelectorsType, feature_type, feature_type_lut),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_NETWORK_MANAGEMENT_TYPES_INC_

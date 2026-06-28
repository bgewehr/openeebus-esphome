/**
 * @file
 * @brief SPINE Datagram Node Management payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_NODE_MANAGEMENT_TYPES_INC_
#define SRC_SPINE_MODEL_NODE_MANAGEMENT_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/binding_management_types.inc"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/network_management_types.inc"
#include "src/spine/model/node_management_types.h"
#include "src/spine/model/subscription_management_types.inc"
#include "src/spine/model/usecase_information_types.inc"
#include "src/spine/model/version_types.inc"

static const EebusDataCfg specification_version_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(string, 0, NULL);

static const EebusDataCfg node_management_specification_version_list_cfg[] = {
    EEBUS_DATA_LIST("specificationVersion", NodeManagementSpecificationVersionListType, specification_version,
        &specification_version_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_specification_version_list_elements_cfg[] = {
    EEBUS_DATA_TAG("specificationVersion", NodeManagementSpecificationVersionListElementsType, specification_version),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_detailed_discovery_device_information_cfg[] = {
    EEBUS_DATA_SEQUENCE("description", NodeManagementDetailedDiscoveryDeviceInformationType, description,
        network_management_device_description_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_detailed_discovery_device_information_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("description", NodeManagementDetailedDiscoveryDeviceInformationElementsType, description,
        network_management_device_description_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_detailed_discovery_entity_information_cfg[] = {
    EEBUS_DATA_SEQUENCE("description", NodeManagementDetailedDiscoveryEntityInformationType, description,
        network_management_entity_description_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_detailed_discovery_entity_information_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("description", NodeManagementDetailedDiscoveryEntityInformationElementsType, description,
        network_management_entity_description_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_detailed_discovery_feature_information_cfg[] = {
    EEBUS_DATA_SEQUENCE("description", NodeManagementDetailedDiscoveryFeatureInformationType, description,
        network_management_feature_description_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_detailed_discovery_feature_information_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("description", NodeManagementDetailedDiscoveryFeatureInformationElementsType, description,
        network_management_feature_description_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg entity_information_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(NodeManagementDetailedDiscoveryEntityInformationType),
        node_management_detailed_discovery_entity_information_cfg);

static const EebusDataCfg feature_information_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(NodeManagementDetailedDiscoveryFeatureInformationType),
        node_management_detailed_discovery_feature_information_cfg);

static const EebusDataCfg node_management_detailed_discovery_data_cfg[] = {
    EEBUS_DATA_SEQUENCE("specificationVersionList", NodeManagementDetailedDiscoveryDataType, specification_version_list,
        node_management_specification_version_list_cfg),
    EEBUS_DATA_SEQUENCE("deviceInformation", NodeManagementDetailedDiscoveryDataType, device_information,
        node_management_detailed_discovery_device_information_cfg),
    EEBUS_DATA_LIST("entityInformation", NodeManagementDetailedDiscoveryDataType, entity_information,
        &entity_information_element_data_cfg),
    EEBUS_DATA_LIST("featureInformation", NodeManagementDetailedDiscoveryDataType, feature_information,
        &feature_information_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_detailed_discovery_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("specificationVersionList", NodeManagementDetailedDiscoveryDataElementsType,
        specification_version_list, node_management_specification_version_list_elements_cfg),
    EEBUS_DATA_SEQUENCE("deviceInformation", NodeManagementDetailedDiscoveryDataElementsType, device_information,
        node_management_detailed_discovery_device_information_elements_cfg),
    EEBUS_DATA_SEQUENCE("entityInformation", NodeManagementDetailedDiscoveryDataElementsType, entity_information,
        node_management_detailed_discovery_entity_information_elements_cfg),
    EEBUS_DATA_SEQUENCE("featureInformation", NodeManagementDetailedDiscoveryDataElementsType, feature_information,
        node_management_detailed_discovery_feature_information_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_detailed_discovery_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("deviceInformation", NodeManagementDetailedDiscoveryDataSelectorsType, device_information,
        network_management_device_description_list_data_selectors_cfg),
    EEBUS_DATA_SEQUENCE("entityInformation", NodeManagementDetailedDiscoveryDataSelectorsType, entity_information,
        network_management_entity_description_list_data_selectors_cfg),
    EEBUS_DATA_SEQUENCE("featureInformation", NodeManagementDetailedDiscoveryDataSelectorsType, feature_information,
        network_management_feature_description_list_data_selectors_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg binding_entry_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(BindingManagementEntryDataType), binding_management_entry_data_cfg);

static const EebusDataCfg node_management_binding_data_cfg[] = {
    EEBUS_DATA_LIST("bindingEntry", NodeManagementBindingDataType, binding_entry, &binding_entry_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_binding_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("bindingEntry", NodeManagementBindingDataElementsType, binding_entry,
        binding_management_entry_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_binding_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("bindingEntry", NodeManagementBindingDataSelectorsType, binding_entry,
        binding_management_entry_list_data_selectors_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_binding_request_call_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "bindingRequest", NodeManagementBindingRequestCallType, binding_request, binding_management_request_call_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_binding_request_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("bindingRequest", NodeManagementBindingRequestCallElementsType, binding_request,
        binding_management_request_call_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_binding_delete_call_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "bindingDelete", NodeManagementBindingDeleteCallType, binding_delete, binding_management_delete_call_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_binding_delete_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("bindingDelete", NodeManagementBindingDeleteCallElementsType, binding_delete,
        binding_management_delete_call_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg subscription_entry_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(SubscriptionManagementEntryDataType), subscription_management_entry_data_cfg);

static const EebusDataCfg node_management_subscription_data_cfg[] = {
    EEBUS_DATA_LIST("subscriptionEntry", NodeManagementSubscriptionDataType, subscription_entry,
        &subscription_entry_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_subscription_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("subscriptionEntry", NodeManagementSubscriptionDataElementsType, subscription_entry,
        subscription_management_entry_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_subscription_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("subscriptionEntry", NodeManagementSubscriptionDataSelectorsType, subscription_entry,
        subscription_management_entry_list_data_selectors_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_subscription_request_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("subscriptionRequest", NodeManagementSubscriptionRequestCallType, subscription_request,
        subscription_management_request_call_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_subscription_request_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("subscriptionRequest", NodeManagementSubscriptionRequestCallElementsType, subscription_request,
        subscription_management_request_call_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_subscription_delete_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("subscriptionDelete", NodeManagementSubscriptionDeleteCallType, subscription_delete,
        subscription_management_delete_call_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_subscription_delete_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("subscriptionDelete", NodeManagementSubscriptionDeleteCallElementsType, subscription_delete,
        subscription_management_delete_call_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_destination_data_cfg[] = {
    EEBUS_DATA_SEQUENCE("deviceDescription", NodeManagementDestinationDataType, device_description,
        network_management_device_description_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_destination_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("deviceDescription", NodeManagementDestinationDataElementsType, device_description,
        network_management_device_description_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_destination_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(NodeManagementDestinationDataType), node_management_destination_data_cfg);

static const EebusDataCfg node_management_destination_list_data_cfg[] = {
    EEBUS_DATA_LIST("nodeManagementDestinationData", NodeManagementDestinationListDataType,
        node_management_destination_data, &node_management_destination_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_destination_list_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("deviceDescription", NodeManagementDestinationListDataSelectorsType, device_description,
        network_management_device_description_list_data_selectors_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg use_case_information_element_data_cfg
    = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(UseCaseInformationDataType), use_case_information_data_cfg);

static const EebusDataCfg node_management_use_case_data_cfg[] = {
    EEBUS_DATA_LIST("useCaseInformation", NodeManagementUseCaseDataType, use_case_information,
        &use_case_information_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_use_case_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("useCaseInformation", NodeManagementUseCaseDataElementsType, use_case_information,
        use_case_information_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg node_management_use_case_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("useCaseInformation", NodeManagementUseCaseDataSelectorsType, use_case_information,
        use_case_information_list_data_selectors_cfg),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_NODE_MANAGEMENT_TYPES_INC_

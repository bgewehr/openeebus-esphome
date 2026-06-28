/*
 * Copyright 2025 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file
 * @brief Feature Info Client
 */

#ifndef SRC_USE_CASE_FEATURE_INFO_FEATURE_INFO_CLIENT_H_
#define SRC_USE_CASE_FEATURE_INFO_FEATURE_INFO_CLIENT_H_

#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/device_remote_interface.h"
#include "src/spine/api/entity_local_interface.h"
#include "src/spine/api/entity_remote_interface.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/feature_remote_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct FeatureInfoClient FeatureInfoClient;

struct FeatureInfoClient {
  FeatureTypeType feature_type;

  RoleType local_role;
  DeviceLocalObject* local_device;
  EntityLocalObject* local_entity;
  FeatureLocalObject* local_feature;

  RoleType remote_role;
  FeatureRemoteObject* remote_feature;
  DeviceRemoteObject* remote_device;
  EntityRemoteObject* remote_entity;
};

EebusError FeatureInfoClientConstruct(
    FeatureInfoClient* self,
    FeatureTypeType feature_type,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
);

/**
 * @brief check if there is a subscription to the remote feature
 */
bool HasSubscription(FeatureInfoClient* self);

/**
 * @brief Subscribe to the feature of the entity
 */
EebusError Subscribe(FeatureInfoClient* self);

/**
 * @brief unssubscribe to the feature of the entity
 */
EebusError Unsubscribe(FeatureInfoClient* self);

/**
 * @brief check if there is a binding to the remote feature
 */
bool HasBinding(FeatureInfoClient* self);

/**
 * @brief Bind to the feature of the entity
 */
EebusError Bind(FeatureInfoClient* self);

/**
 * @brief Remove a binding to the feature of the entity
 */
EebusError Unbind(FeatureInfoClient* self);

/**
 * @brief Add a callback function to be invoked once a result or reply message for a msgCounter came in
 */
EebusError
AddResponseCallback(FeatureInfoClient* self, MsgCounterType msg_counter_ref, ResponseMessageCallback cb, void* ctx);

/**
 * @brief Add a callback function to be invoked once a result came in
 */
void AddResultCallback(FeatureInfoClient* self, ResponseMessageCallback cb, void* ctx);

/**
 * @brief Helper method which adds checking if the feature is available and the operation is allowed
 * selectors and elements are used if specific data should be requested by using
 * model.FilterType DataSelectors (selectors) and/or DataElements (elements)
 * both should use the proper data types for the used function
 */
EebusError
RequestData(FeatureInfoClient* self, FunctionType function_type, const void* selectors, const void* elements);

/**
 * @brief Helper method which writes data to remote
 * @param self Pointer to the Feature Info Client instance
 * @param cmd Pointer to the command to be written
 * @return EebusError Error code indicating the success or failure of the operation
 */
EebusError WriteCmd(FeatureInfoClient* self, const CmdType* cmd);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_FEATURE_INFO_FEATURE_INFO_CLIENT_H_

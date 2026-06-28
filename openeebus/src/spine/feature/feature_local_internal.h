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
 * @brief Feature Local internal declarations (to be used only by unit tests or FeatureLocal subclasses)
 */

#ifndef SRC_SPINE_FEATURE_FEATURE_LOCAL_INTERNAL_H_
#define SRC_SPINE_FEATURE_FEATURE_LOCAL_INTERNAL_H_

#include "src/common/uint64_lut.h"
#include "src/common/vector.h"
#include "src/spine/api/entity_local_interface.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/feature/feature_address_container.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct FeatureLocal FeatureLocal;

struct FeatureLocal {
  /** Inherits the Feature */
  Feature obj;

  EntityLocalObject* entity;
  Uint64Lut resp_msg_cbs;
  Vector result_cbs;
  Vector wr_approval_cbs;
  Vector pending_write_requests;

  FeatureAddressContainer bindings;
  FeatureAddressContainer subscriptions;
};

#define FEATURE_LOCAL(obj) ((FeatureLocal*)(obj))

void FeatureLocalConstruct(
    FeatureLocal* self,
    uint32_t id,
    EntityLocalObject* entity,
    FeatureTypeType type,
    RoleType role
);

EebusError FeatureLocalProcessResult(FeatureLocal* self, const Message* msg);

void FeatureLocalDestruct(FeatureObject* self);
DeviceLocalObject* FeatureLocalGetDevice(const FeatureLocalObject* self);
EntityLocalObject* FeatureLocalGetEntity(const FeatureLocalObject* self);
const void* FeatureLocalGetData(const FeatureLocalObject* self, FunctionType function_type);
void FeatureLocalSetFunctionOperations(FeatureLocalObject* self, FunctionType type, bool read, bool write);
EebusError FeatureLocalAddResponseCallback(
    FeatureLocalObject* self,
    MsgCounterType msg_counter_ref,
    ResponseMessageCallback cb,
    void* ctx
);
void FeatureLocalAddResultCallback(FeatureLocalObject* self, ResponseMessageCallback cb, void* ctx);
EebusError FeatureLocalAddWriteApprovalCallback(FeatureLocalObject* self, WriteApprovalCallback cb, void* ctx);
EebusError FeatureLocalTryApproveWrite(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt);
EebusError
FeatureLocalDenyWrite(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt, const ErrorType* err);
void FeatureLocalCleanRemoteDeviceCaches(FeatureLocalObject* self, const DeviceAddressType* remote_addr);
void* FeatureLocalDataCopy(const FeatureLocalObject* self, FunctionType function_type);
EebusError FeatureLocalUpdateData(
    FeatureLocalObject* self,
    FunctionType fcn_type,
    const void* data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
);
void FeatureLocalSetData(FeatureLocalObject* self, FunctionType function_type, void* data);
EebusError FeatureLocalRequestRemoteData(
    FeatureLocalObject* self,
    FunctionType function_type,
    const FilterType* filter_partial,
    FeatureRemoteObject* dest_feature
);
EebusError FeatureLocalRequestRemoteDataBySenderAddress(
    FeatureLocalObject* self,
    const CmdType* cmd,
    SenderObject* sender,
    const char* dest_ski,
    const FeatureAddressType* dest_addr,
    uint32_t max_delay
);
bool FeatureLocalHasSubscriptionToRemote(const FeatureLocalObject* self, const FeatureAddressType* remote_addr);
EebusError FeatureLocalSubscribeToRemote(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
EebusError FeatureLocalRemoveRemoteSubscription(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
void FeatureLocalRemoveAllRemoteSubscriptions(FeatureLocalObject* self);
bool FeatureLocalHasBindingToRemote(const FeatureLocalObject* self, const FeatureAddressType* remote_addr);
EebusError FeatureLocalBindToRemote(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
EebusError FeatureLocalRemoveRemoteBinding(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
void FeatureLocalRemoveAllRemoteBindings(FeatureLocalObject* self);
NodeManagementDetailedDiscoveryFeatureInformationType* FeatureLocalCreateInformation(const FeatureLocalObject* self);
void FeatureLocalTick(FeatureLocalObject* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_FEATURE_FEATURE_LOCAL_INTERNAL_H_

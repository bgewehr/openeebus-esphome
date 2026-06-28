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
 * @brief Feature Local interface declarations
 */

#ifndef SRC_SPINE_API_FEATURE_LOCAL_INTERFACE_H_
#define SRC_SPINE_API_FEATURE_LOCAL_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/spine/api/feature_interface.h"
#include "src/spine/api/feature_remote_interface.h"
#include "src/spine/api/message.h"
#include "src/spine/api/sender_interface.h"
#include "src/spine/model/feature_types.h"
#include "src/spine/model/function_types.h"
#include "src/spine/model/node_management_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Message Message;

typedef struct ResponseMessage ResponseMessage;

typedef void (*ResponseMessageCallback)(const ResponseMessage* response_msg, void* ctx);

typedef void (*WriteApprovalCallback)(const Message* msg, void* ctx);

typedef struct DeviceLocalObject DeviceLocalObject;

typedef struct EntityLocalObject EntityLocalObject;

/**
 * @brief Feature Local Interface
 * (Feature Local "virtual functions table" declaration)
 */
typedef struct FeatureLocalInterface FeatureLocalInterface;

/**
 * @brief Feature Local Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct FeatureLocalObject FeatureLocalObject;

/**
 * @brief FeatureLocal Interface Structure
 */
struct FeatureLocalInterface {
  /** Extends FeatureInterface */
  FeatureInterface feature_interface;

  DeviceLocalObject* (*get_device)(const FeatureLocalObject* self);
  EntityLocalObject* (*get_entity)(const FeatureLocalObject* self);
  const void* (*get_data)(const FeatureLocalObject* self, FunctionType function_type);
  void (*set_function_operations)(FeatureLocalObject* self, FunctionType type, bool read, bool write);
  EebusError (*add_response_callback)(
      FeatureLocalObject* self,
      MsgCounterType msg_counter_ref,
      ResponseMessageCallback cb,
      void* ctx
  );
  void (*add_result_callback)(FeatureLocalObject* self, ResponseMessageCallback cb, void* ctx);
  EebusError (*add_write_approval_callback)(FeatureLocalObject* self, WriteApprovalCallback cb, void* ctx);
  EebusError (*try_approve_write)(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt);
  EebusError (*deny_write)(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt, const ErrorType* err);
  void (*clean_remote_device_caches)(FeatureLocalObject* self, const DeviceAddressType* remote_addr);
  void* (*data_copy)(const FeatureLocalObject* self, FunctionType function_type);
  EebusError (*update_data)(
      FeatureLocalObject* self,
      FunctionType fcn_type,
      const void* data,
      const FilterType* filter_partial,
      const FilterType* filter_delete
  );
  void (*set_data)(FeatureLocalObject* self, FunctionType function_type, void* data);
  EebusError (*request_remote_data)(
      FeatureLocalObject* self,
      FunctionType function_type,
      const FilterType* filter_partial,
      FeatureRemoteObject* dest_feature
  );
  EebusError (*request_remote_data_by_sender_address)(
      FeatureLocalObject* self,
      const CmdType* cmd,
      SenderObject* sender,
      const char* dest_ski,
      const FeatureAddressType* dest_addr,
      uint32_t max_delay
  );
  bool (*has_subscription_to_remote)(const FeatureLocalObject* self, const FeatureAddressType* remote_addr);
  EebusError (*subscribe_to_remote)(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
  EebusError (*remove_remote_subscription)(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
  void (*remove_all_remote_subscriptions)(FeatureLocalObject* self);
  bool (*has_binding_to_remote)(const FeatureLocalObject* self, const FeatureAddressType* remote_addr);
  EebusError (*bind_to_remote)(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
  EebusError (*remove_remote_binding)(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
  void (*remove_all_remote_bindings)(FeatureLocalObject* self);
  EebusError (*handle_message)(FeatureLocalObject* self, const Message* msg);
  NodeManagementDetailedDiscoveryFeatureInformationType* (*create_information)(const FeatureLocalObject* self);
  void (*tick)(FeatureLocalObject* self);
};

/**
 * @brief Feature Local Object Structure
 */
struct FeatureLocalObject {
  const FeatureLocalInterface* interface_;
};

/**
 * @brief Feature Local pointer typecast
 */
#define FEATURE_LOCAL_OBJECT(obj) ((FeatureLocalObject*)(obj))

/**
 * @brief Feature Local Interface class pointer typecast
 */
#define FEATURE_LOCAL_INTERFACE(obj) (FEATURE_LOCAL_OBJECT(obj)->interface_)

/**
 * @brief Feature Local Get Device caller definition
 */
#define FEATURE_LOCAL_GET_DEVICE(obj) (FEATURE_LOCAL_INTERFACE(obj)->get_device(obj))

/**
 * @brief Feature Local Get Entity caller definition
 */
#define FEATURE_LOCAL_GET_ENTITY(obj) (FEATURE_LOCAL_INTERFACE(obj)->get_entity(obj))

/**
 * @brief Feature Local Get Data caller definition
 */
#define FEATURE_LOCAL_GET_DATA(obj, function_type) (FEATURE_LOCAL_INTERFACE(obj)->get_data(obj, function_type))

/**
 * @brief Feature Local Set Function Operations caller definition
 */
#define FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(obj, type, read, write) \
  (FEATURE_LOCAL_INTERFACE(obj)->set_function_operations(obj, type, read, write))

/**
 * @brief Feature Local Add Response Callback caller definition
 */
#define FEATURE_LOCAL_ADD_RESPONSE_CALLBACK(obj, msg_counter_ref, cb, ctx) \
  (FEATURE_LOCAL_INTERFACE(obj)->add_response_callback(obj, msg_counter_ref, cb, ctx))

/**
 * @brief Feature Local Add Result Callback caller definition
 */
#define FEATURE_LOCAL_ADD_RESULT_CALLBACK(obj, cb, ctx) \
  (FEATURE_LOCAL_INTERFACE(obj)->add_result_callback(obj, cb, ctx))

/**
 * @brief Feature Local Add Write Approval Callback caller definition
 */
#define FEATURE_LOCAL_ADD_WRITE_APPROVAL_CALLBACK(obj, cb, ctx) \
  (FEATURE_LOCAL_INTERFACE(obj)->add_write_approval_callback(obj, cb, ctx))

/**
 * @brief Feature Local Try Approve Write caller definition
 */
#define FEATURE_LOCAL_TRY_APPROVE_WRITE(obj, ski, msg_cnt) \
  (FEATURE_LOCAL_INTERFACE(obj)->try_approve_write(obj, ski, msg_cnt))

/**
 * @brief Feature Local Deny Write caller definition
 */
#define FEATURE_LOCAL_DENY_WRITE(obj, ski, msg_cnt, err) \
  (FEATURE_LOCAL_INTERFACE(obj)->deny_write(obj, ski, msg_cnt, err))

/**
 * @brief Feature Local Clean Remote Device Caches caller definition
 */
#define FEATURE_LOCAL_CLEAN_REMOTE_DEVICE_CACHES(obj, remote_addr) \
  (FEATURE_LOCAL_INTERFACE(obj)->clean_remote_device_caches(obj, remote_addr))

/**
 * @brief Feature Local Data Copy caller definition
 */
#define FEATURE_LOCAL_DATA_COPY(obj, function_type) (FEATURE_LOCAL_INTERFACE(obj)->data_copy(obj, function_type))

/**
 * @brief Feature Local Update Data caller definition
 */
#define FEATURE_LOCAL_UPDATE_DATA(obj, fcn_type, data, filter_partial, filter_delete) \
  (FEATURE_LOCAL_INTERFACE(obj)->update_data(obj, fcn_type, data, filter_partial, filter_delete))

/**
 * @brief Feature Local Set Data caller definition
 */
#define FEATURE_LOCAL_SET_DATA(obj, function_type, data) \
  (FEATURE_LOCAL_INTERFACE(obj)->set_data(obj, function_type, data))

/**
 * @brief Feature Local Request Remote Data caller definition
 */
#define FEATURE_LOCAL_REQUEST_REMOTE_DATA(obj, function_type, filter_partial, dest_feature) \
  (FEATURE_LOCAL_INTERFACE(obj)->request_remote_data(obj, function_type, filter_partial, dest_feature))

/**
 * @brief Feature Local Request Remote Data By Sender Address caller definition
 */
#define FEATURE_LOCAL_REQUEST_REMOTE_DATA_BY_SENDER_ADDRESS(obj, cmd, sender, dest_ski, dest_addr, max_delay) \
  (FEATURE_LOCAL_INTERFACE(obj)                                                                               \
       ->request_remote_data_by_sender_address(obj, cmd, sender, dest_ski, dest_addr, max_delay))

/**
 * @brief Feature Local Has Subscription To Remote caller definition
 */
#define FEATURE_LOCAL_HAS_SUBSCRIPTION_TO_REMOTE(obj, remote_addr) \
  (FEATURE_LOCAL_INTERFACE(obj)->has_subscription_to_remote(obj, remote_addr))

/**
 * @brief Feature Local Subscribe To Remote caller definition
 */
#define FEATURE_LOCAL_SUBSCRIBE_TO_REMOTE(obj, remote_addr) \
  (FEATURE_LOCAL_INTERFACE(obj)->subscribe_to_remote(obj, remote_addr))

/**
 * @brief Feature Local Remove Remote Subscription caller definition
 */
#define FEATURE_LOCAL_REMOVE_REMOTE_SUBSCRIPTION(obj, remote_addr) \
  (FEATURE_LOCAL_INTERFACE(obj)->remove_remote_subscription(obj, remote_addr))

/**
 * @brief Feature Local Remove All Remote Subscriptions caller definition
 */
#define FEATURE_LOCAL_REMOVE_ALL_REMOTE_SUBSCRIPTIONS(obj) \
  (FEATURE_LOCAL_INTERFACE(obj)->remove_all_remote_subscriptions(obj))

/**
 * @brief Feature Local Has Binding To Remote caller definition
 */
#define FEATURE_LOCAL_HAS_BINDING_TO_REMOTE(obj, remote_addr) \
  (FEATURE_LOCAL_INTERFACE(obj)->has_binding_to_remote(obj, remote_addr))

/**
 * @brief Feature Local Bind To Remote caller definition
 */
#define FEATURE_LOCAL_BIND_TO_REMOTE(obj, remote_addr) (FEATURE_LOCAL_INTERFACE(obj)->bind_to_remote(obj, remote_addr))

/**
 * @brief Feature Local Remove Remote Binding caller definition
 */
#define FEATURE_LOCAL_REMOVE_REMOTE_BINDING(obj, remote_addr) \
  (FEATURE_LOCAL_INTERFACE(obj)->remove_remote_binding(obj, remote_addr))

/**
 * @brief Feature Local Remove All Remote Bindings caller definition
 */
#define FEATURE_LOCAL_REMOVE_ALL_REMOTE_BINDINGS(obj) (FEATURE_LOCAL_INTERFACE(obj)->remove_all_remote_bindings(obj))

/**
 * @brief Feature Local Handle Message caller definition
 */
#define FEATURE_LOCAL_HANDLE_MESSAGE(obj, msg) (FEATURE_LOCAL_INTERFACE(obj)->handle_message(obj, msg))

/**
 * @brief Feature Local Create Information caller definition
 */
#define FEATURE_LOCAL_CREATE_INFORMATION(obj) (FEATURE_LOCAL_INTERFACE(obj)->create_information(obj))

/**
 * @brief Feature Local Tick caller definition
 */
#define FEATURE_LOCAL_TICK(obj) (FEATURE_LOCAL_INTERFACE(obj)->tick(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_FEATURE_LOCAL_INTERFACE_H_

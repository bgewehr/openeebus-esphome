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
 * @brief Device Configuration Client functionality implementation
 */

#include "src/use_case/specialization/device_configuration/device_configuration_client.h"

#include "src/spine/model/device_configuration_types.h"
#include "src/spine/model/filter.h"

static const FunctionType key_value_fcn = kFunctionTypeDeviceConfigurationKeyValueListData;

EebusError DeviceConfigurationClientConstruct(
    DeviceConfigurationClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
) {
  const EebusError err = FeatureInfoClientConstruct(
      &self->feature_info_client,
      kFeatureTypeTypeDeviceConfiguration,
      local_entity,
      remote_entity
  );

  if (err != kEebusErrorOk) {
    return err;
  }

  DeviceConfigurationCommonConstruct(&self->device_cfg_common, NULL, self->feature_info_client.remote_feature);
  return kEebusErrorOk;
}

EebusError DeviceConfigurationClientRequestKeyValueDescription(
    DeviceConfigurationClient* self,
    const DeviceConfigurationKeyValueDescriptionListDataSelectorsType* selectors,
    const DeviceConfigurationKeyValueDescriptionDataElementsType* elements
) {
  return RequestData(
      &self->feature_info_client,
      kFunctionTypeDeviceConfigurationKeyValueDescriptionListData,
      selectors,
      elements
  );
}

EebusError DeviceConfigurationClientRequestKeyValue(
    DeviceConfigurationClient* self,
    const DeviceConfigurationKeyValueListDataSelectorsType* selectors,
    const DeviceConfigurationKeyValueDataElementsType* elements
) {
  return RequestData(&self->feature_info_client, key_value_fcn, selectors, elements);
}

EebusError DeviceConfigurationClientWriteKeyValueList(
    DeviceConfigurationClient* self,
    const DeviceConfigurationKeyValueListDataType* key_value_list
) {
  if (key_value_list == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  if ((key_value_list->device_configuration_key_value_data == NULL)
      || (key_value_list->device_configuration_key_value_data_size == 0)) {
    return kEebusErrorInputArgument;
  }

  FeatureRemoteObject* const fr = self->feature_info_client.remote_feature;

  const OperationsObject* const operations = FEATURE_GET_FUNCTION_OPERATIONS(FEATURE_OBJECT(fr), key_value_fcn);

  if ((operations == NULL) || !OPERATIONS_GET_WRITE_PARTIAL(operations)) {
    const EebusError err = FEATURE_REMOTE_UPDATE_DATA(fr, key_value_fcn, key_value_list, NULL, NULL, false);
    if (err != kEebusErrorOk) {
      return err;
    }

    const CmdType cmd = {
        .data_choice         = FEATURE_REMOTE_GET_DATA(fr, key_value_fcn),
        .data_choice_type_id = key_value_fcn,
    };

    return WriteCmd(&self->feature_info_client, &cmd);
  } else {
    const CmdType cmd = {
        .data_choice         = key_value_list,
        .data_choice_type_id = key_value_fcn,
        .filter              = &(const FilterType*){&FILTER_PARTIAL(key_value_fcn, NULL, NULL, NULL)},
        .filter_size         = 1,
        .function            = &(FunctionType){key_value_fcn},
    };

    return WriteCmd(&self->feature_info_client, &cmd);
  }
}

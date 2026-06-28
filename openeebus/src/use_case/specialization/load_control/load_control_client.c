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
 * @brief Load Control Client functionality implementation
 */

#include "src/use_case/specialization/load_control/load_control_client.h"

#include "src/spine/model/filter.h"
#include "src/spine/model/loadcontrol_types.h"

static const FunctionType limit_fcn = kFunctionTypeLoadControlLimitListData;

EebusError LoadControlClientConstruct(
    LoadControlClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
) {
  const EebusError err = FeatureInfoClientConstruct(
      &self->feature_info_client,
      kFeatureTypeTypeLoadControl,
      local_entity,
      remote_entity
  );

  if (err != kEebusErrorOk) {
    return err;
  }

  LocalLoadControlCommonConstruct(&self->load_control_common, NULL, self->feature_info_client.remote_feature);
  return kEebusErrorOk;
}

EebusError LoadControlClientRequestLimitDescriptions(
    LoadControlClient* self,
    const LoadControlLimitDescriptionListDataSelectorsType* selectors,
    const LoadControlLimitDescriptionDataElementsType* elements
) {
  return RequestData(&self->feature_info_client, kFunctionTypeLoadControlLimitDescriptionListData, selectors, elements);
}

EebusError LoadControlClientRequestLimitConstraints(
    LoadControlClient* self,
    const LoadControlLimitConstraintsListDataSelectorsType* selectors,
    const LoadControlLimitConstraintsDataElementsType* elements
) {
  return RequestData(&self->feature_info_client, kFunctionTypeLoadControlLimitConstraintsListData, selectors, elements);
}

EebusError LoadControlClientRequestLimitData(
    LoadControlClient* self,
    const LoadControlLimitListDataSelectorsType* selectors,
    const LoadControlLimitDataElementsType* elements
) {
  return RequestData(&self->feature_info_client, limit_fcn, selectors, elements);
}

EebusError LoadControlClientWriteLimitList(
    LoadControlClient* self,
    const LoadControlLimitListDataType* limit_list,
    const LoadControlLimitListDataSelectorsType* delete_selectors,
    const LoadControlLimitDataElementsType* delete_elements
) {
  if (limit_list == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  if (limit_list->load_control_limit_data == NULL || limit_list->load_control_limit_data_size == 0) {
    return kEebusErrorInputArgument;
  }

  FeatureRemoteObject* const fr = self->feature_info_client.remote_feature;

  const OperationsObject* const operations = FEATURE_GET_FUNCTION_OPERATIONS(FEATURE_OBJECT(fr), limit_fcn);

  if ((operations == NULL) || !OPERATIONS_GET_WRITE_PARTIAL(operations)) {
    // All limit_list data shall be sent
    const EebusError err = FEATURE_REMOTE_UPDATE_DATA(fr, limit_fcn, limit_list, NULL, NULL, false);
    if (err != kEebusErrorOk) {
      return err;
    }

    const CmdType cmd = {
        .data_choice         = FEATURE_REMOTE_GET_DATA(fr, limit_fcn),
        .data_choice_type_id = limit_fcn,
    };

    return WriteCmd(&self->feature_info_client, &cmd);
  } else {
    const FilterType filter_partial = FILTER_PARTIAL(limit_fcn, NULL, NULL, NULL);
    const FilterType filter_delete  = FILTER_DELETE(limit_fcn, NULL, delete_selectors, delete_elements);

    const FilterType* filters[2] = {&filter_partial, NULL};
    size_t filters_size          = 1;

    if ((delete_elements != NULL) && (delete_selectors != NULL)) {
      filters[filters_size++] = &filter_delete;
    }

    const CmdType cmd = {
        .data_choice         = limit_list,
        .data_choice_type_id = limit_fcn,
        .filter              = filters,
        .filter_size         = filters_size,
        .function            = &(FunctionType){limit_fcn},
    };

    return WriteCmd(&self->feature_info_client, &cmd);
  }
}

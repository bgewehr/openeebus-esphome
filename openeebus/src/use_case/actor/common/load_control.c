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
 * @brief Load Control helper functions implementation
 */

#include "src/common/eebus_errors.h"
#include "src/spine/model/absolute_or_relative_time.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/specialization/load_control/load_control_client.h"

EebusError LoadControlWriteLimit(
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity,
    const LoadControlLimitDescriptionDataType* filter,
    const LoadLimit* limit
) {
  LoadControlClient lcc;
  const EebusError err = LoadControlClientConstruct(&lcc, local_entity, remote_entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  const LoadControlLimitDataType* limit_data = LoadControlCommonGetLimitWithFilter(&lcc.load_control_common, filter);

  if (limit_data == NULL || limit_data->limit_id == NULL) {
    return kEebusErrorNoChange;
  }

  // EEBus_UC_TS_LimitationOfPowerConsumption V1.0.0 3.2.2.2.2.2
  // If set to "true", the timePeriod, value and isLimitActive Elements SHALL be writeable by a client.
  if ((limit_data->is_limit_changeable != NULL) && (!*limit_data->is_limit_changeable)) {
    return kEebusErrorNotSupported;
  }

  const TimePeriodType time_period = {
      .end_time = &ABSOLUTE_OR_RELATIVE_TIME_WITH_DURATION(limit->duration),
  };

  const LoadControlLimitDataType new_limit = {
      .limit_id        = limit_data->limit_id,
      .is_limit_active = &limit->is_active,
      .value           = &(ScaledNumberType){&limit->value.value, &limit->value.scale},
      .time_period     = (EebusDurationToSeconds(&limit->duration) > 0) ? &time_period : NULL,
  };

  const LoadControlLimitListDataType new_limit_list = (LoadControlLimitListDataType){
      .load_control_limit_data      = &(const LoadControlLimitDataType*){&new_limit},
      .load_control_limit_data_size = 1,
  };

  const LoadControlLimitListDataSelectorsType* delete_selectors = &(LoadControlLimitListDataSelectorsType){
      .limit_id = limit_data->limit_id,
  };

  // clang-format off
  const LoadControlLimitDataElementsType* delete_elements = &(LoadControlLimitDataElementsType){
      .time_period = &(TimePeriodElementsType){
          .start_time = EEBUS_TAG_RESET,
          .end_time   = EEBUS_TAG_RESET,
      },
  };
  // clang-format on

  // If timer period should not be deleted, reset delete_selectors and delete_elements
  if (!limit->delete_duration) {
    delete_selectors = NULL;
    delete_elements  = NULL;
  }

  return LoadControlClientWriteLimitList(&lcc, &new_limit_list, delete_selectors, delete_elements);
}

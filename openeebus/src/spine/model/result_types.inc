/**
 * @file
 * @brief SPINE Datagram Result payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_RESULT_TYPES_INC_
#define SRC_SPINE_MODEL_RESULT_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/result_types.h"

static const EebusDataCfg result_data_cfg[] = {
    EEBUS_DATA_UINT32("errorNumber", ResultDataType, error_number),
    EEBUS_DATA_STRING("description", ResultDataType, description),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_RESULT_TYPES_INC_

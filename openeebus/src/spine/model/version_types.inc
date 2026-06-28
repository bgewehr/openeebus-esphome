/**
 * @file
 * @brief SPINE Datagram Version payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_VERSION_TYPES_INC_
#define SRC_SPINE_MODEL_VERSION_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/version_types.h"

static const EebusDataCfg specification_version_data_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(string, 0, NULL);

static const EebusDataCfg specification_version_list_data_cfg[] = {
    EEBUS_DATA_LIST("specificationVersionData", SpecificationVersionListDataType, specification_version_data,
        &specification_version_data_element_data_cfg),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_VERSION_TYPES_INC_

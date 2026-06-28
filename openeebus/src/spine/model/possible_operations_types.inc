/**
 * @file
 * @brief SPINE Datagram Possible Operations payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_POSSIBLE_OPERATIONS_TYPES_INC_
#define SRC_SPINE_MODEL_POSSIBLE_OPERATIONS_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/possible_operations_types.h"

static const EebusDataCfg possible_operations_classifier_cfg[] = {
    EEBUS_DATA_TAG("partial", PossibleOperationsClassifierType, partial),
    EEBUS_DATA_END,
};

static const EebusDataCfg possible_operations_read_cfg[] = {
    EEBUS_DATA_TAG("partial", PossibleOperationsReadType, partial),
    EEBUS_DATA_END,
};

static const EebusDataCfg possible_operations_write_cfg[] = {
    EEBUS_DATA_TAG("partial", PossibleOperationsWriteType, partial),
    EEBUS_DATA_END,
};

static const EebusDataCfg possible_operations_cfg[] = {
    EEBUS_DATA_SEQUENCE("read", PossibleOperationsType, read, possible_operations_read_cfg),
    EEBUS_DATA_SEQUENCE("write", PossibleOperationsType, write, possible_operations_write_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg possible_operations_elements_cfg[] = {
    EEBUS_DATA_TAG("read", PossibleOperationsElementsType, read_enabled),
    EEBUS_DATA_TAG("write", PossibleOperationsElementsType, write_enabled),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_POSSIBLE_OPERATIONS_TYPES_INC_

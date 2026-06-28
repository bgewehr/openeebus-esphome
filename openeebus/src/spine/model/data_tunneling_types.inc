/**
 * @file
 * @brief SPINE Datagram Data Tunneling payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_DATA_TUNNELING_TYPES_INC_
#define SRC_SPINE_MODEL_DATA_TUNNELING_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/data_tunneling_types.h"

static const EebusDataCfg data_tunneling_header_cfg[] = {
    EEBUS_DATA_STRING("purposeId", DataTunnelingHeaderType, purpose_id),
    EEBUS_DATA_UINT32("channelId", DataTunnelingHeaderType, channel_id),
    EEBUS_DATA_UINT32("sequenceId", DataTunnelingHeaderType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg data_tunneling_header_elements_cfg[] = {
    EEBUS_DATA_TAG("purposeId", DataTunnelingHeaderElementsType, purpose_id),
    EEBUS_DATA_TAG("channelId", DataTunnelingHeaderElementsType, channel_id),
    EEBUS_DATA_TAG("sequenceId", DataTunnelingHeaderElementsType, sequence_id),
    EEBUS_DATA_END,
};

static const EebusDataCfg data_tunneling_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("header", DataTunnelingCallType, header, data_tunneling_header_cfg),
    EEBUS_DATA_STRING("payload", DataTunnelingCallType, payload),
    EEBUS_DATA_END,
};

static const EebusDataCfg data_tunneling_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("header", DataTunnelingCallElementsType, header, data_tunneling_header_elements_cfg),
    EEBUS_DATA_TAG("payload", DataTunnelingCallElementsType, payload),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_DATA_TUNNELING_TYPES_INC_

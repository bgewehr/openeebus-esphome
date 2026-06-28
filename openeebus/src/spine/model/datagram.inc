/**
 * @file
 * @brief Datagram Data Configuration
 */

#ifndef SRC_EEBUS_SRC_SPINE_MODEL_DATAGRAM_INC_
#define SRC_EEBUS_SRC_SPINE_MODEL_DATAGRAM_INC_

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/command_frame_types.inc"
#include "src/spine/model/datagram.h"

static const EnumMapping cmd_classifiers[] = {
    {"read", kCommandClassifierTypeRead},
    {"reply", kCommandClassifierTypeReply},
    {"notify", kCommandClassifierTypeNotify},
    {"write", kCommandClassifierTypeWrite},
    {"call", kCommandClassifierTypeCall},
    {"result", kCommandClassifierTypeResult},
    EEBUS_ENUM_MAPPING_END,
};

static const EebusDataCfg datagram_header_data_cfg[] = {
    EEBUS_DATA_STRING("specificationVersion", HeaderType, spec_version),
    EEBUS_DATA_SEQUENCE("addressSource", HeaderType, src_addr, feature_address_cfg),
    EEBUS_DATA_SEQUENCE("addressDestination", HeaderType, dest_addr, feature_address_cfg),
    EEBUS_DATA_SEQUENCE("addressOriginator", HeaderType, originator_addr, feature_address_cfg),
    EEBUS_DATA_UINT64("msgCounter", HeaderType, msg_cnt),
    EEBUS_DATA_UINT64("msgCounterReference", HeaderType, msg_cnt_ref),
    EEBUS_DATA_ENUM("cmdClassifier", HeaderType, cmd_classifier, cmd_classifiers),
    EEBUS_DATA_BOOL("ackRequest", HeaderType, ack_request),
    EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME("timestamp", HeaderType, timestamp),
    EEBUS_DATA_END,
};

static const EebusDataCfg cmd_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(sequence, sizeof(CmdType), cmd_cfg);

static const EebusDataCfg datagram_payload_data_cfg[] = {
    EEBUS_DATA_LIST("cmd", PayloadType, cmd, &cmd_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg datagram_sequence_data_cfg[] = {
    EEBUS_DATA_SEQUENCE("header", DatagramType, header, datagram_header_data_cfg),
    EEBUS_DATA_SEQUENCE("payload", DatagramType, payload, datagram_payload_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg datagram_data_cfg
    = EEBUS_DATA_SEQUENCE_ROOT("datagram", DatagramType, datagram_sequence_data_cfg);

#endif  // SRC_EEBUS_SRC_SPINE_MODEL_DATAGRAM_INC_

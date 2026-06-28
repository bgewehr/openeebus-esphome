/**
 * @file
 * @brief SPINE Datagram Time Information payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_TIME_INFORMATION_TYPES_INC_
#define SRC_SPINE_MODEL_TIME_INFORMATION_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/time_information_types.h"

static const EebusDataCfg time_information_data_cfg[] = {
    EEBUS_DATA_DATE_TIME("utc", TimeInformationDataType, utc),
    EEBUS_DATA_DURATION("utcOffset", TimeInformationDataType, utc_offset),
    EEBUS_DATA_ENUM("dayOfWeek", TimeInformationDataType, day_of_week, day_of_week_lut),
    EEBUS_DATA_UINT8("calendarWeek", TimeInformationDataType, calendar_week),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_information_data_elements_cfg[] = {
    EEBUS_DATA_TAG("utc", TimeInformationDataElementsType, utc),
    EEBUS_DATA_TAG("utcOffset", TimeInformationDataElementsType, utc_offset),
    EEBUS_DATA_TAG("dayOfWeek", TimeInformationDataElementsType, day_of_week),
    EEBUS_DATA_TAG("calendarWeek", TimeInformationDataElementsType, calendar_week),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_distributor_data_cfg[] = {
    EEBUS_DATA_BOOL("isTimeDistributor", TimeDistributorDataType, is_time_distributor),
    EEBUS_DATA_UINT32("distributorPriority", TimeDistributorDataType, distributor_priority),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_distributor_data_elements_cfg[] = {
    EEBUS_DATA_TAG("isTimeDistributor", TimeDistributorDataElementsType, is_time_distributor),
    EEBUS_DATA_TAG("distributorPriority", TimeDistributorDataElementsType, distributor_priority),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_precision_data_cfg[] = {
    EEBUS_DATA_BOOL("isSynchronised", TimePrecisionDataType, is_synchronised),
    EEBUS_DATA_DATE_TIME("lastSyncAt", TimePrecisionDataType, last_sync_at),
    EEBUS_DATA_INT32("clockDrift", TimePrecisionDataType, clock_drift),
    EEBUS_DATA_END,
};

static const EebusDataCfg time_precision_data_elements_cfg[] = {
    EEBUS_DATA_TAG("isSynchronised", TimePrecisionDataElementsType, is_synchronised),
    EEBUS_DATA_TAG("lastSyncAt", TimePrecisionDataElementsType, last_sync_at),
    EEBUS_DATA_TAG("clockDrift", TimePrecisionDataElementsType, clock_drift),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_TIME_INFORMATION_TYPES_INC_

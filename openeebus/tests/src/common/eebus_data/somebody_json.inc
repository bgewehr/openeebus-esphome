#ifndef TESTS_SRC_COMMON_DATA_SOMEBODY_DATA_INC
#define TESTS_SRC_COMMON_DATA_SOMEBODY_DATA_INC

#include "src/common/eebus_data/eebus_data.h"
#include "tests/src/common/eebus_data/employee.h"
#include "tests/src/common/eebus_data/employee_json.inc"
#include "tests/src/common/eebus_data/person.h"
#include "tests/src/common/eebus_data/person_json.inc"
#include "tests/src/common/eebus_data/somebody.h"

static const EebusDataCfg somebody_choice_data_cfg[] = {
    EEBUS_DATA_CHOICE_ELEMENT(sequence, "employee", Employee, employee_sequence_data_cfg),
    EEBUS_DATA_CHOICE_ELEMENT(sequence, "person", Person, person_sequence_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg somebody_choice_root_cfg = EEBUS_DATA_CHOICE(Somebody, data, somebody_choice_data_cfg);

static const EebusDataCfg somebody_data_cfg = EEBUS_DATA_CHOICE_ROOT(Somebody, &somebody_choice_root_cfg);

#endif  // TESTS_SRC_COMMON_DATA_SOMEBODY_DATA_INC

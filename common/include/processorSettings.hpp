#ifndef COMMON_INCLUDE_PROCESSOR_SETTINGS_HPP
#define COMMON_INCLUDE_PROCESSOR_SETTINGS_HPP

#include <inttypes.h>
#include <cstddef>

typedef long double processor_data_type;

const int NUM_OF_REGISTERS         = 4;
const size_t SIZE_OF_REGISTER      = 1;

// FIXME: seems that it's not used
const size_t SIZE_OF_NORMAL_VAR    = sizeof(processor_data_type);
const int PROCESSOR_DATA_TYPE_SIZE = sizeof(processor_data_type);

enum PushCmdArgs {
    HAS_NUM_ARG = 1,
    HAS_REG_ARG   = 2,
    HAS_RAM_ARG   = 4,
};

enum RegisterNames {
    AX = 1,
    BX = 2,
    CX = 3,
    DX = 4,
};

#endif

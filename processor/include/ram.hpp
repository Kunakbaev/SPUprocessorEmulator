#ifndef PROCESSOR_INCLUDE_RAM_HPP
#define PROCESSOR_INCLUDE_RAM_HPP

#include "../../common/include/processorSettings.hpp"

enum RamStructErrors {
    RAM_STATUS_OK                                   = 0,                  // no error, everything is valid
    RAM_ERROR_INVALID_ARGUMENT                      = 1,                  // usually when argument is set to NULL
    RAM_ERROR_MEMORY_ALLOCATION_ERROR               = 2,                  // usually when memory allocation by calloc fails
    RAM_ERROR_BAD_VAR_INDEX                         = 3,
};

struct RamStruct {
    size_t               memorySize;
    processor_data_type* memory;
};

RamStructErrors pleaseGiveMeRAM(RamStruct* ram);
RamStructErrors getRamVarByIndex(const RamStruct* ram, size_t index, processor_data_type** result);
RamStructErrors setRamVarByIndex(const RamStruct* ram, size_t index, processor_data_type value);
RamStructErrors drawRamMemory(const RamStruct* ram);
const char* getRamErrorMessage(RamStructErrors error);

#endif

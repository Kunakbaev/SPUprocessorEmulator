#ifndef PROCESSOR_HEADER
#define PROCESSOR_HEADER

#include "processorErrorsHandler.hpp"
#include "../../StackStruct/include/stackLib.hpp"

const   int PROCESSOR_DATA_TYPE_SIZE = 4; // int
typedef int processor_data_type;

static_assert(sizeof(processor_data_type) == PROCESSOR_DATA_TYPE_SIZE);

struct Processor {
    size_t   numberOfInstructions;
    size_t   instructionPointer;
    uint8_t* programCode; // WARNING: be careful with small data type
    Stack    stack;
};

Errors ProcessorConstructor(Processor* processor);
Errors readProgramBinary(Processor* processor, const char* binFileName);

Errors pushElementToCalculator(Processor* processor);
Errors runProgramBinary(Processor* processor);
Errors ProcessorDestructor(Processor* processor);

#endif

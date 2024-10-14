#ifndef PROCESSOR_HEADER
#define PROCESSOR_HEADER

#include "../../external/StackStruct/include/stackLib.hpp"
#include "processorErrorsHandler.hpp"

const   int PROCESSOR_DATA_TYPE_SIZE = 4; // int
typedef int processor_data_type;

static_assert(sizeof(processor_data_type) == PROCESSOR_DATA_TYPE_SIZE);

struct Processor {
    size_t   numberOfInstructions;
    size_t   instructionPointer;
    uint8_t* programCode; // WARNING: be careful with small data type
    Stack    stack;
};

ProcessorErrors ProcessorConstructor(Processor* processor);
ProcessorErrors readProgramBinary(Processor* processor, const char* binFileName);

ProcessorErrors pushElementToCalculator(Processor* processor);
ProcessorErrors runProgramBinary(Processor* processor);
ProcessorErrors ProcessorDestructor(Processor* processor);

#endif

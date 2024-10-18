#ifndef PROCESSOR_INCLUDE_PROCESSOR_HPP
#define PROCESSOR_INCLUDE_PROCESSOR_HPP

#include "../../external/StackStruct/include/stackLib.hpp"
#include "processorErrorsHandler.hpp"

const int NUM_OF_REGISTERS         = 4;
const size_t SIZE_OF_REGISTER      = 1;
typedef int processor_data_type;

// FIXME:
const size_t SIZE_OF_NORMAL_VAR    = sizeof(processor_data_type);
const int PROCESSOR_DATA_TYPE_SIZE = sizeof(processor_data_type);

struct Processor {
    size_t   numberOfInstructions;
    size_t   instructionPointer;
    // ASK: does it need to be const? And if so, how to implement it?
    uint8_t* programCode; // WARNING: be careful with small data type
    Stack    stackOfVars;
    processor_data_type* registers;
};

ProcessorErrors ProcessorConstructor(Processor* processor);
ProcessorErrors readProgramBinary(Processor* processor, const char* binFileName);

ProcessorErrors runProgramBinary(Processor* processor);
ProcessorErrors ProcessorDestructor(Processor* processor);

#endif

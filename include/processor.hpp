#ifndef PROCESSOR_HEADER
#define PROCESSOR_HEADER

#include "../StackStruct/include/stackLib.hpp"
#include "errorsHandler.hpp"

const int PROCESSOR_DATA_TYPE_SIZE = 4; // int

struct Processor {
    size_t   numberOfInstructions;
    int      instructionPointer;
    uint8_t* programCode; // WARNING: be careful with small data type
    Stack    stack;
};

Errors ProcessorConstructor(Processor* processor);
Errors readProgramBinary(Processor* processor, const char* binFileName);
Errors runProgramBinary(Processor* processor);
Errors ProcessorDestructor(Processor* processor);

#endif

#ifndef PROCESSOR_INCLUDE_PROCESSOR_HPP
#define PROCESSOR_INCLUDE_PROCESSOR_HPP

#include "../../external/StackStruct/include/stackLib.hpp"
#include "processorErrorsHandler.hpp"
#include "../../common/include/processorSettings.hpp"
#include "ram.hpp"

struct Processor {
    size_t               numberOfInstructions;
    size_t               instructionPointer;
    // ASK: does it need to be const? And if so, how to implement it?
    // FIXME: rewrite to const
    uint8_t*             programCode; // WARNING: be careful with small data type
    processor_data_type* registers;

    struct Stack         stackOfVars;
    struct Stack         stackOfCalls; // saves size_t, instruction pointers to which we should return after call
    struct RamStruct     ram;
};

ProcessorErrors ProcessorConstructor(Processor* processor);
ProcessorErrors readProgramBinary(Processor* processor, const char* binFileName);

ProcessorErrors runProgramBinary(Processor* processor);
ProcessorErrors ProcessorDestructor(Processor* processor);

#endif

#ifndef PROCESSOR_INCLUDE_PROCESSOR_COMMANDS_REALIZATION_HPP
#define PROCESSOR_INCLUDE_PROCESSOR_COMMANDS_REALIZATION_HPP

#include "processor.hpp"

ProcessorErrors haltCommandFunc(Processor* processor);
ProcessorErrors lookLastVarInVarStackFunc(Processor* processor);
ProcessorErrors popAndPrintLastVarInStackFunc(Processor* processor);
ProcessorErrors popFromProcessorStackFunc(Processor* processor);
ProcessorErrors pushToProcessorStackFunc(Processor* processor);
ProcessorErrors procCommandCallFunc(Processor* processor);
ProcessorErrors procCommandReturnFromFunc(Processor* processor);
ProcessorErrors procCommandDrawFunc(Processor* processor);
ProcessorErrors procCommandInFromTerminal(Processor* processor);
ProcessorErrors meowFunc(Processor* processor);

#define PROCESSOR_COMMAND_METHOD_DECL(funcName) \
    ProcessorErrors funcName(Processor* processor)

PROCESSOR_COMMAND_METHOD_DECL(add2numsFunc);
PROCESSOR_COMMAND_METHOD_DECL(sub2numsFunc);
PROCESSOR_COMMAND_METHOD_DECL(mul2numsFunc);
PROCESSOR_COMMAND_METHOD_DECL(div2numsFunc);
PROCESSOR_COMMAND_METHOD_DECL(mod2numsFunc);
PROCESSOR_COMMAND_METHOD_DECL(sqrt1numFunc);
PROCESSOR_COMMAND_METHOD_DECL(lessCmpFunc);
PROCESSOR_COMMAND_METHOD_DECL(lessOrEqCmpFunc);
PROCESSOR_COMMAND_METHOD_DECL(greaterCmpFunc);
PROCESSOR_COMMAND_METHOD_DECL(greaterOrEqCmpFunc);
PROCESSOR_COMMAND_METHOD_DECL(isEqCmpFunc);

PROCESSOR_COMMAND_METHOD_DECL(procCommandJumpIfEqual);
PROCESSOR_COMMAND_METHOD_DECL(procCommandJumpIfBelow);
PROCESSOR_COMMAND_METHOD_DECL(procCommandJumpIfMore);
PROCESSOR_COMMAND_METHOD_DECL(procCommandJumpAnyway);

#endif

#ifndef PROCESSOR_INCLUDE_PROCESSOR_COMMANDS_REALIZATION_HPP
#define PROCESSOR_INCLUDE_PROCESSOR_COMMANDS_REALIZATION_HPP

#include "processor.hpp"

ProcessorErrors haltCommandFunc(Processor* processor);
ProcessorErrors lookLastVarInVarStackFunc(Processor* processor);
ProcessorErrors popAndPrintLastVarInStackFunc(Processor* processor);
ProcessorErrors popFromProcessorStackFunc(Processor* processor);
ProcessorErrors pushToProcessorStackFunc(Processor* processor);

#define PROCESSOR_COMMAND_FUNC_DECL_WITH_2_ARGS(funcName) \
    ProcessorErrors funcName(Processor* processor)

PROCESSOR_COMMAND_FUNC_DECL_WITH_2_ARGS(add2numsFunc);
PROCESSOR_COMMAND_FUNC_DECL_WITH_2_ARGS(sub2numsFunc);
PROCESSOR_COMMAND_FUNC_DECL_WITH_2_ARGS(mul2numsFunc);
PROCESSOR_COMMAND_FUNC_DECL_WITH_2_ARGS(div2numsFunc);

#endif

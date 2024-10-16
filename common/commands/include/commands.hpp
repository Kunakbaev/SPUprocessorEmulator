#ifndef COMMON_COMMANDS_INCLUDE_COMMANDS_HPP
#define COMMON_COMMANDS_INCLUDE_COMMANDS_HPP

// TODO: manage func ptr typedef more carefull
#include "commandsErrorsHandler.hpp"
#include "../../../processor/include/processor.hpp"
#include "../include/operations.hpp"

// ASK: this define gets redefined in each header, how to solve it?
// maybe name define with additional name of module to which it belongs

#include <cstddef>

struct CommandStruct {
    size_t commandIndex; // ASK: cringe?
    const char* commandName;
};

// enum CommandsEnum {
//     HALT_COMMAND        = 0,
//     PUSH_COMMAND        = 1,
//     POP_COMMAND         = 2,
//     OUT_COMMAND         = 3,
//     ADD_COMMAND         = 4,
//     SUB_COMMAND         = 5,
//     MUL_COMMAND         = 6,
//     DIV_COMMAND         = 7,
//     DUMP_COMMAND        = 9
// };

CommandErrors popAndPrintLastInStack(Stack* stack, size_t* instructionPointer,
                                     size_t numberOfInstructions);
CommandErrors executeOperationWith2Args(const uint8_t* programCode, size_t* instructionPointer,
                                        size_t numberOfInstructions,
                                        Stack* stack,
                                        twoArgsOperFuncPtr operation);
CommandErrors pushToProcessorStack(const uint8_t* programCode, size_t* instructionPointer,
                                   size_t numberOfInstructions,
                                   Stack* stack);

CommandErrors getCommandByName(const char* commandName, CommandStruct* result);
CommandErrors getCommandByIndex(size_t index, CommandStruct* result);

#endif

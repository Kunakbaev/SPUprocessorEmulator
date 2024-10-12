#ifndef INCLUDE_COMMANDS_HPP
#define INCLUDE_COMMANDS_HPP

// TODO: manage func ptr typedef more carefull
#include "cringeTypedefFuncPtr.hpp"

struct CommandStruct {
    size_t commandIndex; // ASK: cringe?
    const char* commandName;
    processorCommandExecFuncPtr actionFunc;
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

//CommandsEnum getCommandIndexByName(const char* command);
Errors getCommandByName(const char* commandName, CommandStruct* result);
Errors getCommandByIndex(size_t index, CommandStruct* result);

#endif

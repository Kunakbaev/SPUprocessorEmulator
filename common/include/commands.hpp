#ifndef INCLUDE_COMMANDS_HPP
#define INCLUDE_COMMANDS_HPP

// TODO: manage func ptr typedef more carefull
#include "cringeTypedefFuncPtr.hpp"
#include "errorsHandlerDefines.hpp"

// ASK: this define gets redefined in each header, how to solve it?
// maybe name define with additional name of module to which it belongs

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

enum CommandErrors {
    COMMANDS_STATUS_OK                                   = 0,                  // no error, everything is valid
    COMMANDS_ERROR_INVALID_ARGUMENT                      = 1,                  // usually when argument is set to NULL
    COMMANDS_ERROR_MEMORY_ALLOCATION_ERROR               = 2,                  // usually when memory allocation by calloc fails
    COMMANDS_ERROR_MEMORY_REALLOCATION_ERROR             = 3,                  // couldn't reallocate memory
    COMMANDS_ERROR_COULDNT_OPEN_FILE                     = 4,
    COMMANDS_ERROR_BAD_INDEX                             = 5,                  // command index index is bigger or equal (0 indexation) than total number of comands
};

CommandErrors getCommandByName(const char* commandName, CommandStruct* result);
CommandErrors getCommandByIndex(size_t index, CommandStruct* result);

const char* getCommandsErrorMessage(CommandErrors error);

#endif

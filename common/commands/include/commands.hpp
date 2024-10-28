#ifndef COMMON_COMMANDS_INCLUDE_COMMANDS_HPP
#define COMMON_COMMANDS_INCLUDE_COMMANDS_HPP

// TODO: manage func ptr typedef more carefull
#include "commandsErrorsHandler.hpp"
#include "../../../processor/include/processor.hpp"

#include <cstddef>

struct CommandStruct {
    size_t      commandIndex; // cringe? норм
    const char* commandName;
    int         possibleArgsMask;
};

CommandErrors validateCommands();
CommandErrors findRegName(const char* name, int* ind);
CommandErrors getCommandByName(const char* commandName, CommandStruct* result);
CommandErrors getCommandByIndex(size_t index, CommandStruct* result);
CommandErrors checkIfGoodArgMaskForCommand(size_t commandIndex, int mask, bool* is);
CommandErrors isJumpCommand(const char* commandName, bool* is);

#endif

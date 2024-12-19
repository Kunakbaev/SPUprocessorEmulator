#include <inttypes.h>

#include "../include/commands.hpp"
#include "../../include/errorsHandlerDefines.hpp"
#include "../../../external/StackStruct/include/stackLib.hpp"
#include "../../../processor/include/ram.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, COMMANDS_ERROR_INVALID_ARGUMENT, getCommandsErrorMessage)


#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getCommandsErrorMessage, COMMANDS_STATUS_OK)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getCommandsErrorMessage)
//
// #define GET_COMMAND_HASH(name) \
//     do {                        \
//         uint64_t heh = #name;                                \
//
//     } while (0)

/*

ASK:
will hash work properly? For same strings as an argument produce same hashes

*/

#define GET_COMMAND_HASH(name) ((uint64_t)(&(#name)))

#define CMD(commandIndex, commandName, possibleArgsMask, funcPtr) \
    {commandIndex, commandName, possibleArgsMask},

constexpr CommandStruct COMMANDS[] = {
    #include "../include/commandsRealization.in"
};

#undef CMD

// WARNING: be carefull that jump commands in this array and in COMMANDS array are same
constexpr const char* JUMP_COMMANDS[] = {
    "jmp", "jb", "ja", "je", "call"
};

const size_t NUM_OF_COMMANDS      = sizeof(COMMANDS)      / sizeof(*COMMANDS);
const size_t NUM_OF_JUMP_COMMANDS = sizeof(JUMP_COMMANDS) / sizeof(*JUMP_COMMANDS);

// registers are numberated in one indexation, so first register name is undefined
const char* registerNames[] = { "?", "AX", "BX", "CX", "DX" };

const size_t NUM_OF_REGS = sizeof(registerNames) / sizeof(*registerNames) - 1;

// checks that command index - 1 (one indexation to reduce errors) is equal to array index
CommandErrors validateCommands() {
    for (size_t commandInd = 0; commandInd < NUM_OF_COMMANDS; ++commandInd) {
        if (COMMANDS[commandInd].commandIndex - 1 != commandInd) {
            return COMMANDS_ERROR_BAD_COMMAND_INDEX_FORMAT;
        }
    }

    return COMMANDS_STATUS_OK;
}

CommandErrors findRegName(const char* name, int* ind) {
    IF_ARG_NULL_RETURN(name);
    IF_ARG_NULL_RETURN(ind);

    *ind = -1;
    for (size_t regInd = 1; regInd <= NUM_OF_REGS; ++regInd) {
        if (strcmp(name, registerNames[regInd]) == 0) {
            *ind = regInd;
            return COMMANDS_STATUS_OK;
        }
    }

    return COMMANDS_STATUS_OK;
}

CommandErrors isJumpCommand(const char* commandName, bool* is) {
    IF_ARG_NULL_RETURN(commandName);
    IF_ARG_NULL_RETURN(is);

    *is = false;
    for (size_t commandIndex = 0; commandIndex < NUM_OF_JUMP_COMMANDS; ++commandIndex) {
        // FIXME: too slow
        // can write hash function check for this
        if (strcmp(commandName, JUMP_COMMANDS[commandIndex]) == 0) {
            *is = true;
            return COMMANDS_STATUS_OK;
        }
    }

    return COMMANDS_STATUS_OK;
}

CommandErrors checkIfGoodArgMaskForCommand(size_t commandIndex, int mask, bool* is) {
    IF_NOT_COND_RETURN(commandIndex - 1 < NUM_OF_COMMANDS,
                       COMMANDS_ERROR_INVALID_ARGUMENT);
    IF_NOT_COND_RETURN(mask < 8, COMMANDS_ERROR_INVALID_ARGUMENT);
    IF_ARG_NULL_RETURN(is);

    LOG_DEBUG_VARS(COMMANDS[commandIndex - 1].possibleArgsMask);
    *is = (COMMANDS[commandIndex - 1].possibleArgsMask >> mask) & 1;
    return COMMANDS_STATUS_OK;
}

CommandErrors getCommandByName(const char* commandName, CommandStruct* result) {
    IF_ARG_NULL_RETURN(result);
    IF_ARG_NULL_RETURN(commandName);

    //result = NULL; // in case if not found
    result->commandName = "?"; // in case if not found
    for (size_t commandIndex = 0; commandIndex < NUM_OF_COMMANDS; ++commandIndex) {
        // FIXME: too slow
        // TODO: read about strecmp
        // can write hash function check for this
        // LOG_DEBUG_VARS(commandIndex, commandName, COMMANDS[commandIndex].commandName);
        if (strcmp(commandName, COMMANDS[commandIndex].commandName) == 0) {
            *result = COMMANDS[commandIndex];
            return COMMANDS_STATUS_OK;
        }
    }

    return COMMANDS_ERROR_COMMAND_NOT_FOUND;
}

CommandErrors getCommandByIndex(size_t index, CommandStruct* result) {
    IF_ARG_NULL_RETURN(result);
    IF_NOT_COND_RETURN(index - 1 < NUM_OF_COMMANDS,
                       COMMANDS_ERROR_INVALID_ARGUMENT); // TODO: add error

    *result = COMMANDS[index - 1];
    return COMMANDS_STATUS_OK;
}

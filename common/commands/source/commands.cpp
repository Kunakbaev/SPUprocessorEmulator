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

constexpr CommandStruct COMMANDS[] = {
    {1,  "push"},
    {2,  "add"},
    {3,  "sub"},
    {4,  "mul"},
    {5,  "div"},
    {6,  "out"},    // pop last num from stack and than outputs it
    {7,  "halt"},   // ends program
    {9,  "pop"},    // pop last value from stack to given register
    {10, "pick"},
};

const size_t NUM_OF_COMMANDS = sizeof(COMMANDS) / sizeof(*COMMANDS); // ?????????????

// registers are numberated in one indexation, so first register name is undefined
const char* registerNames[] = { "?", "AX", "BX", "CX", "DX" };

const size_t NUM_OF_REGS = sizeof(registerNames) / sizeof(*registerNames) - 1;

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

CommandErrors getCommandByName(const char* commandName, CommandStruct* result) {
    IF_ARG_NULL_RETURN(result);
    IF_ARG_NULL_RETURN(commandName);

    //result = NULL; // in case if not found
    result->commandName = "?"; // in case if not found
    for (size_t commandIndex = 0; commandIndex < NUM_OF_COMMANDS; ++commandIndex) {
        // FIXME: too slow
        // can write hash function check for this
        if (strcmp(commandName, COMMANDS[commandIndex].commandName) == 0) {
            *result = COMMANDS[commandIndex];
            return COMMANDS_STATUS_OK;
        }
    }

    return COMMANDS_ERROR_COMMAND_NOT_FOUND;
}

CommandErrors getCommandByIndex(size_t index, CommandStruct* result) {
    IF_ARG_NULL_RETURN(result);

    result->commandName = "?"; // in case if not found
    for (size_t commandIndex = 0; commandIndex < NUM_OF_COMMANDS; ++commandIndex) {
        // FIXME: too slow?
        // can write hash function check for this
        if (COMMANDS[commandIndex].commandIndex == index) {
            *result = COMMANDS[commandIndex];
            return COMMANDS_STATUS_OK;
        }
    }

    return COMMANDS_ERROR_COMMAND_NOT_FOUND;
}

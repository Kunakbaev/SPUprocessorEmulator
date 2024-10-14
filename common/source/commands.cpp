#include <inttypes.h>

#include "../include/commands.hpp"

// ASK:
// CommandStruct commands[NUM_OF_COMMANDS] = {
//     {"push", pushElementToCalculator},
// };

constexpr CommandStruct COMMANDS[] = {
    {1, "push", pushElementToCalculator},
    {1, "push", pushElementToCalculator},
};

const size_t NUM_OF_COMMANDS = sizeof(COMMANDS) / sizeof(*COMMANDS); // ?????????????


// uint64_t getCommandHash();
//
// CommandsEnum getCommandIndexByName(const char* command) {
//
// }

const char* getErrorMessage(CommandErrors error) {
    switch (error) {
        //  -------------------   GENERAL ERRORS    ---------------------------
        case COMMANDS_STATUS_OK:
            return "No erros in processor, everything is valid.\n";
        case COMMANDS_ERROR_INVALID_ARGUMENT:
            return "Commands error: invalid argument (possibly set to NULL).\n";
        case COMMANDS_ERROR_MEMORY_ALLOCATION_ERROR:
            return "Commands error: couldn't allocate memory.\n";
        case COMMANDS_ERROR_MEMORY_REALLOCATION_ERROR:
            return "Commands error: couldn't reallocate memory.\n";
        case COMMANDS_ERROR_COULDNT_OPEN_FILE:
            return "Commands error: couldn't open file.\n";

        default:
            return "Unknown error.\n";
    }
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

    return COMMANDS_STATUS_OK;
}

CommandErrors getCommandByIndex(size_t index, CommandStruct* result) {
    IF_ARG_NULL_RETURN(result);
    IF_NOT_COND_RETURN(index < NUM_OF_COMMANDS, COMMANDS_ERROR_BAD_INDEX);

    *result = COMMANDS[index];
    return COMMANDS_STATUS_OK;

    // switch (index) {
    //     case PUSH_COMMAND:          return "push";
    //     case ADD_COMMAND:           return "add";
    //     case SUB_COMMAND:           return "sub";
    //     case MUL_COMMAND:           return "mul";
    //     case DIV_COMMAND:           return "div";
    //     case HALT_COMMAND:          return "halt";
    // }
}

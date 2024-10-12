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

Errors getCommandByName(const char* commandName, CommandStruct* result) {
    IF_ARG_NULL_RETURN(result);
    IF_ARG_NULL_RETURN(commandName);

    //result = NULL; // in case if not found
    result->commandName = "?"; // in case if not found
    for (size_t commandIndex = 0; commandIndex < NUM_OF_COMMANDS; ++commandIndex) {
        // FIXME: too slow
        // can write hash function check for this
        if (strcmp(commandName, commands[commandIndex].commandName) == 0) {
            *result = commands[commandIndex];
            return STATUS_OK;
        }
    }

    return STATUS_OK;
}

Errors getCommandByIndex(size_t index, CommandStruct* result) {
    IF_ARG_NULL_RETURN(result);
    IF_NOT_COND_RETURN(index < NUM_OF_COMMANDS, ERROR_ARRAY_BAD_INDEX);

    *result = commands[index];
    return STATUS_OK;

    // switch (index) {
    //     case PUSH_COMMAND:          return "push";
    //     case ADD_COMMAND:           return "add";
    //     case SUB_COMMAND:           return "sub";
    //     case MUL_COMMAND:           return "mul";
    //     case DIV_COMMAND:           return "div";
    //     case HALT_COMMAND:          return "halt";
    // }
}

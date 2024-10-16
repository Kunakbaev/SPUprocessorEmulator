#include <inttypes.h>

#include "../include/commands.hpp"
#include "../../include/errorsHandlerDefines.hpp"
#include "../../../external/StackStruct/include/stackLib.hpp"

// ASK:
// CommandStruct commands[NUM_OF_COMMANDS] = {
//     {"push", pushElementToCalculator},
// };

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, COMMANDS_ERROR_INVALID_ARGUMENT, getCommandsErrorMessage)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getCommandsErrorMessage)

constexpr CommandStruct COMMANDS[] = {
    {1, "push"},
    {2, "add"},
    {3, "sub"},
    {4, "mul"},
    {5, "div"},
    {6, "out"},
    {7, "halt"},
};

const size_t NUM_OF_COMMANDS = sizeof(COMMANDS) / sizeof(*COMMANDS); // ?????????????

// ASK: it's bad to pass whole Processor struct, because it has to many fields that this function
// doesn't need (stack of func calls, registers)

CommandErrors popAndPrintLastInStack(Stack* stack, size_t* instructionPointer,
                                     size_t numberOfInstructions) {
    IF_ARG_NULL_RETURN(stack);
    IF_ARG_NULL_RETURN(instructionPointer);
    IF_NOT_COND_RETURN(*instructionPointer < numberOfInstructions,
                       COMMANDS_ERROR_BAD_INSTR_PTR);

    processor_data_type num = 0;
    // dumpStackLog(stack);
    LOG_DEBUG_VARS(stack->numberOfElements);
    Errors error = popElementToStack(stack, &num);
    if (error != NULL) {
        LOG_ERROR(getErrorMessage(error));
        return COMMANDS_ERROR_STACK_ERROR;
    }

    // FIXME: do something with different types
    printf("last in stack : %d\n", num);
    LOG_DEBUG_VARS("last in stack : ", num);

    *instructionPointer += 1;

    return COMMANDS_STATUS_OK;
}

CommandErrors executeOperationWith2Args(const uint8_t* programCode, size_t* instructionPointer,
                                        size_t numberOfInstructions,
                                        Stack* stack,
                                        twoArgsOperFuncPtr operation) {
    IF_ARG_NULL_RETURN(programCode);
    IF_ARG_NULL_RETURN(stack);
    IF_ARG_NULL_RETURN(instructionPointer);
    IF_ARG_NULL_RETURN(operation);
    IF_NOT_COND_RETURN(*instructionPointer < numberOfInstructions,
                       COMMANDS_ERROR_BAD_INSTR_PTR);

    processor_data_type number_1 = 0;
    processor_data_type number_2 = 0;
    // TODO: define for checking error from separate lib or function
    LOG_DEBUG_VARS(stack->numberOfElements);
    Errors error = popElementToStack(stack, &number_2);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return COMMANDS_ERROR_STACK_ERROR;
    }

    error = popElementToStack(stack, &number_1);
    LOG_DEBUG_VARS(stack->numberOfElements);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return COMMANDS_ERROR_STACK_ERROR;
    }
    processor_data_type operationResult = (*operation)(number_1, number_2);
    LOG_DEBUG_VARS(number_1, number_2, operationResult);

    error = pushElementToStack(stack, &operationResult);
    LOG_DEBUG_VARS(stack->numberOfElements);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return COMMANDS_ERROR_STACK_ERROR;
    }

    *instructionPointer += 1;
    return COMMANDS_STATUS_OK;
}

CommandErrors pushToProcessorStack(const uint8_t* programCode, size_t* instructionPointer,
                                   size_t numberOfInstructions,
                                   Stack* stack) {
    IF_ARG_NULL_RETURN(programCode);
    IF_ARG_NULL_RETURN(instructionPointer);
    IF_ARG_NULL_RETURN(stack);
    IF_NOT_COND_RETURN(*instructionPointer + 2 < numberOfInstructions,
                       COMMANDS_ERROR_BAD_INSTR_PTR);

    processor_data_type number = programCode[*instructionPointer + 1];
    Errors error = pushElementToStack(stack, &number);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return COMMANDS_ERROR_STACK_ERROR;
    }
    LOG_DEBUG_VARS(stack->numberOfElements);

    *instructionPointer += 2;
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
    //IF_NOT_COND_RETURN(index < NUM_OF_COMMANDS, COMMANDS_ERROR_BAD_INDEX);

    // *result = COMMANDS[index];

    result->commandName = "?"; // in case if not found
    for (size_t commandIndex = 0; commandIndex < NUM_OF_COMMANDS; ++commandIndex) {
        // FIXME: too slow
        // can write hash function check for this
        if (COMMANDS[commandIndex].commandIndex == index) {
            *result = COMMANDS[commandIndex];
            return COMMANDS_STATUS_OK;
        }
    }

    return COMMANDS_ERROR_COMMAND_NOT_FOUND;

    // switch (index) {
    //     case PUSH_COMMAND:          return "push";
    //     case ADD_COMMAND:           return "add";
    //     case SUB_COMMAND:           return "sub";
    //     case MUL_COMMAND:           return "mul";
    //     case DIV_COMMAND:           return "div";
    //     case HALT_COMMAND:          return "halt";
    // }
}

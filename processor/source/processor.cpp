#include <iostream>

#include "../../common/commands/include/commands.hpp"
#include "../../common/include/errorsHandlerDefines.hpp"
#include "../include/processor.hpp"
#include "../../common/commands/include/operations.hpp"


// ASK: is this good solution?

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, PROCESSOR_ERROR_INVALID_ARGUMENT, getProcessorErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getProcessorErrorMessage, PROCESSOR_STATUS_OK)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getProcessorErrorMessage)




const size_t MAX_PROGRAM_CODE_SIZE = 1 << 10;
const size_t FILE_LINE_BUFFER_SIZE = 1 << 10;

char* fileLineBuffer;

const char* registerNames[] = { "AX", "BX", "CX", "DX" };

const size_t NUM_OF_REGS = sizeof(registerNames) / sizeof(*registerNames);

ProcessorErrors findRegName(const char* name, int* ind) {
    IF_ARG_NULL_RETURN(name);
    IF_ARG_NULL_RETURN(ind);

    *ind = -1;
    for (size_t regInd = 0; regInd < NUM_OF_REGS; ++regInd) {
        if (strcmp(name, registerNames[regInd]) == 0) {
            *ind = regInd;
            return PROCESSOR_STATUS_OK;
        }
    }

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors ProcessorConstructor(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    *processor = {};
    constructStack(&processor->stackOfVars, 8, PROCESSOR_DATA_TYPE_SIZE);
    processor->instructionPointer   = 0;
    processor->numberOfInstructions = 0;
    processor->programCode          = NULL;

    processor->registers = (processor_data_type*)calloc(NUM_OF_REGISTERS, sizeof(processor_data_type));
    IF_NOT_COND_RETURN(processor->registers != NULL,
                       PROCESSOR_ERROR_MEMORY_ALLOCATION_ERROR);

    fileLineBuffer = (char*)calloc(FILE_LINE_BUFFER_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(fileLineBuffer != NULL,
                       PROCESSOR_ERROR_MEMORY_ALLOCATION_ERROR);

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors getNumberOfLines(FILE* file, int* numOfLines) {
    IF_ARG_NULL_RETURN(file);
    IF_ARG_NULL_RETURN(numOfLines);

    *numOfLines = 0;
    const char CH_TO_COUNT = '\n';
    int ch = '?';
    while ((ch = fgetc(file)) != EOF) {
        *numOfLines += ch == CH_TO_COUNT;
    }
    // while (fgets(fileLineBuffer, FILE_LINE_BUFFER_SIZE, file)) {
    //
    // }

    LOG_DEBUG_VARS(*numOfLines);

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors readCommandsFromFileToArray(uint8_t* array, FILE* file, int numOfLines) {
    IF_ARG_NULL_RETURN(array);
    IF_ARG_NULL_RETURN(file);

    int lineIndex = 0;
    rewind(file);
    while (fgets(fileLineBuffer, FILE_LINE_BUFFER_SIZE, file) && lineIndex < numOfLines) {
        int number = atoi(fileLineBuffer); // FIXME: no error check
        array[lineIndex] = number;
        LOG_DEBUG_VARS(lineIndex, number);
        ++lineIndex;
    }

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors readProgramBinary(Processor* processor, const char* binFileName) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(binFileName);

    LOG_DEBUG("read binary");
    FILE* binaryFile = fopen(binFileName, "r");
    IF_NOT_COND_RETURN(binaryFile != NULL,
                       PROCESSOR_ERROR_COULDNT_OPEN_FILE);

    int numOfLines = 0;
    LOG_DEBUG("ok");
    ProcessorErrors error = getNumberOfLines(binaryFile, &numOfLines);
    IF_ERR_RETURN(error);

    processor->numberOfInstructions = numOfLines;
    processor->programCode = (uint8_t*)calloc(numOfLines, sizeof(uint8_t));
    IF_NOT_COND_RETURN(processor->programCode,
                       PROCESSOR_ERROR_MEMORY_ALLOCATION_ERROR);
    error = readCommandsFromFileToArray(processor->programCode, binaryFile, numOfLines);
    IF_ERR_RETURN(error);

    return PROCESSOR_STATUS_OK;
}

// ends (finishes) 'program'
static ProcessorErrors haltCommandFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    LOG_DEBUG("halt");
    // for loop will end, because conditions is not satisfied any longer
    processor->instructionPointer = processor->numberOfInstructions;

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors pushCommandFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    LOG_DEBUG("push");
    CommandErrors err = pushToProcessorStack(processor->programCode, &processor->instructionPointer,
                                             processor->numberOfInstructions, &processor->stackOfVars);
    if (err != COMMANDS_STATUS_OK) {
        LOG_ERROR(getCommandsErrorMessage(err));
        return PROCESSOR_ERROR_COMMANDS_ERROR;
    }

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors outCommandFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    LOG_DEBUG("out");
    CommandErrors err = popAndPrintLastInStack(&(processor->stackOfVars), &processor->instructionPointer,
                                               processor->numberOfInstructions);
    if (err != COMMANDS_STATUS_OK) {
        LOG_ERROR(getCommandsErrorMessage(err));
        return PROCESSOR_ERROR_COMMANDS_ERROR;
    }

    return PROCESSOR_STATUS_OK;
}

typedef ProcessorErrors (normalCommandFuncPtr)(Processor* processor);

struct NormalCommandsStruct {
    const char* commandName;
    normalCommandFuncPtr* funcPtr;
};

// takes 2 args from stack, pops them, calculates result, and then pushes it to stack
// if args are registers, than they need to be firstly put into stack
struct TwoArgsOperationsCommandsStruct {
    const char* commandName;
    twoArgsOperFuncPtr* funcPtr;
};

struct OneArgOperationsCommandsStruct {
    const char* commandName;
    oneArgOperFuncPtr* funcPtr;
};

NormalCommandsStruct normalCommandsArr[] = {
    {"halt", haltCommandFunc},
    {"push", pushCommandFunc},
    {"out",  outCommandFunc},
};

TwoArgsOperationsCommandsStruct twoArgsCommandsArr[] = {
    {"add", add2nums},
    {"sub", sub2nums},
    {"mul", mul2nums},
    {"div", div2nums}
};

OneArgOperationsCommandsStruct oneArgCommandsArr[] = {
    {"abs", absOperation},
};

const size_t NORMAL_COMMANDS_ARR_SIZE   = sizeof(normalCommandsArr)  / sizeof(*normalCommandsArr);
const size_t TWO_ARGS_COMMANDS_ARR_SIZE = sizeof(twoArgsCommandsArr) / sizeof(*twoArgsCommandsArr);
const size_t ONE_ARG_COMMANDS_ARR_SIZE  = sizeof(oneArgCommandsArr)  / sizeof(*oneArgCommandsArr);

static ProcessorErrors checkNormalCommands(Processor* processor, const char* commandName) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(commandName);

    for (size_t commandIndex = 0; commandIndex < NORMAL_COMMANDS_ARR_SIZE; ++commandIndex) {
        if (strcmp(normalCommandsArr[commandIndex].commandName, commandName) != 0)
            continue;

        ProcessorErrors error = (*normalCommandsArr[commandIndex].funcPtr)(processor);
        IF_ERR_RETURN(error);
    }

    return PROCESSOR_STATUS_OK;
}

// ASK: is this copypaste

ProcessorErrors checkCommandsWithTwoArgs(Processor* processor, const char* commandName) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(commandName);

    for (size_t commandIndex = 0; commandIndex < TWO_ARGS_COMMANDS_ARR_SIZE; ++commandIndex) {
        if (strcmp(twoArgsCommandsArr[commandIndex].commandName, commandName) != 0)
            continue;

        CommandErrors error = executeOperationWith2Args(processor->programCode, &processor->instructionPointer,
                                                        processor->numberOfInstructions, &processor->stackOfVars,
                                                        twoArgsCommandsArr[commandIndex].funcPtr);
        if (error != NULL) {
            LOG_ERROR(getCommandsErrorMessage(error));
            return PROCESSOR_ERROR_COMMANDS_ERROR;
        }
    }

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors checkCommandsWithTwoArgs(Processor* processor, const char* commandName) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(commandName);

    for (size_t commandIndex = 0; commandIndex < TWO_ARGS_COMMANDS_ARR_SIZE; ++commandIndex) {
        if (strcmp(twoArgsCommandsArr[commandIndex].commandName, commandName) != 0)
            continue;

        CommandErrors error = executeOperationWith1Arg(processor->programCode, &processor->instructionPointer,
                                                       processor->numberOfInstructions, &processor->stackOfVars,
                                                       twoArgsCommandsArr[commandIndex].funcPtr);
        if (error != NULL) {
            LOG_ERROR(getCommandsErrorMessage(error));
            return PROCESSOR_ERROR_COMMANDS_ERROR;
        }
    }

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors runProgramBinary(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    processor->instructionPointer = 0;
    for (; processor->instructionPointer < processor->numberOfInstructions;) {
        int commandIndex = processor->programCode[processor->instructionPointer];
        LOG_DEBUG_VARS(processor->instructionPointer, commandIndex);

        CommandStruct command = {};
        CommandErrors error = getCommandByIndex(commandIndex, &command);
        LOG_DEBUG_VARS(command.commandIndex, command.commandName);
        if (error != COMMANDS_STATUS_OK) {
            // FIXME: probably overload happens and that's not function that we are looking for
            LOG_ERROR(getCommandsErrorMessage(error));
            return PROCESSOR_ERROR_COMMANDS_ERROR;
        }

        // ASK: we don't have switch, so what to do? Use hashing or just ifs?
        CommandErrors err = COMMANDS_STATUS_OK;
        // end of 'program'
        if (strcmp(command.commandName, "halt") == 0) {
            break;
        }

        if (strcmp(command.commandName, "push") == 0) {
            LOG_DEBUG("push");
            err = pushToProcessorStack(processor->programCode, &processor->instructionPointer,
                                       processor->numberOfInstructions, &processor->stackOfVars);
            if (err != COMMANDS_STATUS_OK) {
                LOG_ERROR(getCommandsErrorMessage(err));
                return PROCESSOR_ERROR_COMMANDS_ERROR;
            }
            continue;
        }

        if (strcmp(command.commandName, "out") == 0) {
            LOG_DEBUG("out");
            err = popAndPrintLastInStack(&(processor->stackOfVars), &processor->instructionPointer,
                                         processor->numberOfInstructions);
            if (err != COMMANDS_STATUS_OK) {
                LOG_ERROR(getCommandsErrorMessage(err));
                return PROCESSOR_ERROR_COMMANDS_ERROR;
            }
            continue;
        }

        // error = (*command.actionFunc)(processor);
        // IF_ERR_RETURN(error);

        // TODO: add operation ptr to commands

        twoArgsOperFuncPtr* funcPtr2Args = NULL;
        // ASK: smells like shit
        if (strcmp(command.commandName, "add") == 0) funcPtr2Args = add2nums;
        if (strcmp(command.commandName, "sub") == 0) funcPtr2Args = sub2nums;
        if (strcmp(command.commandName, "mul") == 0) funcPtr2Args = mul2nums;
        if (strcmp(command.commandName, "div") == 0) funcPtr2Args = div2nums;

        // FIXME: foreach structs: name of command
        if (funcPtr2Args != NULL) {
            LOG_DEBUG("two args operation");
            err = executeOperationWith2Args(processor->programCode, &processor->instructionPointer,
                                            processor->numberOfInstructions, &processor->stackOfVars,
                                            funcPtr2Args);
            if (err != COMMANDS_STATUS_OK) {
                LOG_ERROR(getCommandsErrorMessage(err));
                return PROCESSOR_ERROR_COMMANDS_ERROR;
            }
        }

        LOG_DEBUG_VARS(processor->instructionPointer, processor->numberOfInstructions);
    }

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors ProcessorDestructor(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    Errors error = destructStack(&processor->stackOfVars);
    if (error != STATUS_OK) {
        // FIXME: probably overload happens and that's not function that we are looking for
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    FREE(processor->programCode);
    processor = {}; // ASK: is this ok?

    FREE(fileLineBuffer);

    return PROCESSOR_STATUS_OK;
}

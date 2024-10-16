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

ProcessorErrors ProcessorConstructor(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    *processor = {};
    constructStack(&processor->stack, 8, PROCESSOR_DATA_TYPE_SIZE);
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
                                       processor->numberOfInstructions, &processor->stack);
            if (err != COMMANDS_STATUS_OK) {
                LOG_ERROR(getCommandsErrorMessage(err));
                return PROCESSOR_ERROR_COMMANDS_ERROR;
            }
            continue;
        }

        if (strcmp(command.commandName, "out") == 0) {
            LOG_DEBUG("out");
            err = popAndPrintLastInStack(&(processor->stack), &processor->instructionPointer,
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

        if (funcPtr2Args != NULL) {
            LOG_DEBUG("two args operation");
            err = executeOperationWith2Args(processor->programCode, &processor->instructionPointer,
                                            processor->numberOfInstructions, &processor->stack,
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

    Errors error = destructStack(&processor->stack);
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

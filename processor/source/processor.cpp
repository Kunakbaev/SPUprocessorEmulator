#include <iostream>

#include "../../common/include/commands.hpp"
#include "../include/processor.hpp"


// ASK: is this good solution?

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, PROCESSOR_ERROR_INVALID_ARGUMENT, getProcessorErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getProcessorErrorMessage)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getProcessorErrorMessage)




const size_t MAX_PROGRAM_CODE_SIZE = 1 << 10;
const size_t FILE_LINE_BUFFER_SIZE = 1 << 10;

char* fileLineBuffer;

ProcessorErrors ProcessorConstructor(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    *processor = {};
    constructStack(&processor->stack, 0, PROCESSOR_DATA_TYPE_SIZE);
    processor->instructionPointer   = 0;
    processor->numberOfInstructions = 0;
    processor->programCode          = NULL;

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

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors readCommandsFromFileToArray(uint8_t* array, FILE* file, int numOfLines) {
    IF_ARG_NULL_RETURN(array);
    IF_ARG_NULL_RETURN(file);

    int lineIndex = 0;
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

    processor->programCode = (uint8_t*)calloc(numOfLines, sizeof(uint8_t));
    IF_NOT_COND_RETURN(processor->programCode,
                       PROCESSOR_ERROR_MEMORY_ALLOCATION_ERROR);
    error = readCommandsFromFileToArray(processor->programCode, binaryFile, numOfLines);
    IF_ERR_RETURN(error);

    return PROCESSOR_STATUS_OK;
}

/*



*/

ProcessorErrors pushElementToCalculator(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);
    IF_NOT_COND_RETURN(processor->instructionPointer + 1 < processor->numberOfInstructions,
                       PROCESSOR_ERROR_BAD_INS_POINTER); // TODO: fix errors

    // WARNING: be carefull with size of data inserted
    processor_data_type number = processor->programCode[processor->instructionPointer + 1];

    // TODO: rewrite stack errors, so they begin with STACK_
    Errors error = pushElementToStack(&processor->stack, &number);
    if (error != STATUS_OK) {
        // FIXME: probably overload happens and that's not function that we are looking for
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    processor->instructionPointer += 2;

    return PROCESSOR_STATUS_OK;
}

processor_data_type add2Nums(processor_data_type a, processor_data_type b) {
    return a + b;
}

processor_data_type sub2Nums(processor_data_type a, processor_data_type b) {
    return a - b;
}

processor_data_type mul2Nums(processor_data_type a, processor_data_type b) {
    return a * b;
}

// WARNING: be carefull, integer division
processor_data_type div2Nums(processor_data_type a, processor_data_type b) {
    if (b == 0) {
        LOG_ERROR("zero division"); // TODO: manage error
        return 0;
    }

    return a / b;
}

typedef processor_data_type (*twoArgsOperationFuncPtr)(processor_data_type a, processor_data_type b);

ProcessorErrors doOperationWith2Args(Processor* processor, twoArgsOperationFuncPtr operation) {
    IF_ARG_NULL_RETURN(processor);
    IF_NOT_COND_RETURN(processor->instructionPointer + 2 < processor->numberOfInstructions,
                       PROCESSOR_ERROR_BAD_INS_POINTER); // TODO: fix errors

    // WARNING: be carefull with size of data inserted
    processor_data_type a = processor->programCode[processor->instructionPointer + 1];
    processor_data_type b = processor->programCode[processor->instructionPointer + 2];
    processor_data_type result = (*operation)(a, b);

    Errors error = pushElementToStack(&processor->stack, &result);
    if (error != STATUS_OK) {
        // FIXME: probably overload happens and that's not function that we are looking for
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    processor->instructionPointer += 3;

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors runProgramBinary(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    processor->instructionPointer = 0;
    for (; processor->instructionPointer < processor->numberOfInstructions; ++processor->instructionPointer) {
        int commandIndex = processor->programCode[processor->instructionPointer];
        LOG_DEBUG_VARS(processor->instructionPointer, commandIndex);

        // ASK: ?????
        CommandStruct command = {};
        CommandErrors error = getCommandByIndex(commandIndex, &command);
        if (error != COMMANDS_STATUS_OK) {
            // FIXME: probably overload happens and that's not function that we are looking for
            LOG_ERROR(getCommandsErrorMessage(error));
            return PROCESSOR_ERROR_COMMANDS_ERROR;
        }

        ProcessorErrors err = PROCESSOR_STATUS_OK;
        if (command.commandName == "push") {
            err = pushElementToCalculator(processor);
            IF_ERR_RETURN(err);
            continue;
        }

        // TODO:
        err = doOperationWith2Args(processor, add2Nums);

        // error = (*command.actionFunc)(processor);
        // IF_ERR_RETURN(error);
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

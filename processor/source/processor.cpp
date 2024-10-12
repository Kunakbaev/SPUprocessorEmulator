#include <iostream>

#include "../include/processor.hpp"
#include "../../common/include/commands.hpp"

const size_t MAX_PROGRAM_CODE_SIZE = 1 << 10;
const size_t FILE_LINE_BUFFER_SIZE = 1 << 10;

char* fileLineBuffer;

Errors ProcessorConstructor(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    *processor = {};
    constructStack(&processor->stack, 0, PROCESSOR_DATA_TYPE_SIZE);
    processor->instructionPointer   = 0;
    processor->numberOfInstructions = 0;
    processor->programCode          = NULL;

    fileLineBuffer = (char*)calloc(FILE_LINE_BUFFER_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(fileLineBuffer != NULL,
                       ERROR_MEMORY_ALLOCATION_ERROR);

    return STATUS_OK;
}

static Errors getNumberOfLines(FILE* file, int* numOfLines) {
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

    return STATUS_OK;
}

static Errors readCommandsFromFileToArray(uint8_t* array, FILE* file, int numOfLines) {
    IF_ARG_NULL_RETURN(array);
    IF_ARG_NULL_RETURN(file);

    int lineIndex = 0;
    while (fgets(fileLineBuffer, FILE_LINE_BUFFER_SIZE, file) && lineIndex < numOfLines) {
        int number = atoi(fileLineBuffer); // FIXME: no error check
        array[lineIndex] = number;
        ++lineIndex;
    }

    return STATUS_OK;
}

Errors readProgramBinary(Processor* processor, const char* binFileName) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(binFileName);

    FILE* binaryFile = fopen(binFileName, "r");
    IF_NOT_COND_RETURN(binaryFile != NULL,
                       ERROR_COULDNT_OPEN_FILE);

    int numOfLines = 0;
    Errors error = getNumberOfLines(binaryFile, &numOfLines);
    IF_ERR_RETURN(error);

    processor->programCode = (uint8_t*)calloc(numOfLines, sizeof(uint8_t));
    IF_NOT_COND_RETURN(processor->programCode,
                       ERROR_MEMORY_ALLOCATION_ERROR);
    error = readCommandsFromFileToArray(processor->programCode, binaryFile, numOfLines);
    IF_ERR_RETURN(error);

    return STATUS_OK;
}

/*



*/

Errors pushElementToCalculator(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);
    IF_NOT_COND_RETURN(processor->instructionPointer + 1 < processor->numberOfInstructions,
                       ERROR_ARRAY_BAD_INDEX); // TODO: fix errors

    // WARNING: be carefull with size of data inserted
    processor_data_type number = processor->programCode[processor->instructionPointer + 1];
    Errors error = pushElementToStack(&processor->stack, &number);
    IF_ERR_RETURN(error);

    processor->instructionPointer += 2;

    return STATUS_OK;
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

Errors doOperationWith2Args(Processor* processor, twoArgsOperationFuncPtr operation) {
    IF_ARG_NULL_RETURN(processor);
    IF_NOT_COND_RETURN(processor->instructionPointer + 2 < processor->numberOfInstructions,
                       ERROR_ARRAY_BAD_INDEX); // TODO: fix errors

    // WARNING: be carefull with size of data inserted
    processor_data_type a = processor->programCode[processor->instructionPointer + 1];
    processor_data_type b = processor->programCode[processor->instructionPointer + 2];
    processor_data_type result = (*operation)(a, b);

    Errors error = pushElementToStack(&processor->stack, &result);
    IF_ERR_RETURN(error);

    processor->instructionPointer += 3;

    return STATUS_OK;
}

Errors runProgramBinary(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    processor->instructionPointer = 0;
    for (; processor->instructionPointer < processor->numberOfInstructions; ++processor->instructionPointer) {
        int commandIndex = processor->programCode[processor->instructionPointer];
        LOG_DEBUG_VARS(processor->instructionPointer, commandIndex);

        // ASK: ?????
        CommandStruct command = {};
        Errors error = getCommandByIndex(commandIndex, &command);
        IF_ERR_RETURN(error);

        if (command.commandName == "push") {
            error = pushElementToCalculator(processor);
            IF_ERR_RETURN(error);
            continue;
        }

        error = doOperationWith2Args(processor,

        // error = (*command.actionFunc)(processor);
        // IF_ERR_RETURN(error);
    }

    return STATUS_OK;
}

Errors ProcessorDestructor(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    Errors error = destructStack(&processor->stack);
    IF_ERR_RETURN(error);
    FREE(processor->programCode);
    processor = {}; // ASK: is this ok?

    FREE(fileLineBuffer);

    return STATUS_OK;
}

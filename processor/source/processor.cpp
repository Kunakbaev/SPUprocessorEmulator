#include <iostream>

#include "../../common/commands/include/commands.hpp"
#include "../../common/include/errorsHandlerDefines.hpp"
#include "../include/processor.hpp"
#include "../include/operations.hpp"
#include "../include/processorCommandsRealization.hpp"



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

    *processor = {}; // name constants ~~~~~|
    // FIXME: const int initialCapacity = 8;
    constructStack(&processor->stackOfVars, 8, PROCESSOR_DATA_TYPE_SIZE);
    processor->instructionPointer   = 0;
    processor->numberOfInstructions = 0;
    processor->programCode          = NULL;

    processor->registers = (processor_data_type*)calloc(NUM_OF_REGISTERS, sizeof(processor_data_type));
    IF_NOT_COND_RETURN(processor->registers != NULL,
                       PROCESSOR_ERROR_MEMORY_ALLOCATION_ERROR);

    // FIXME: free buffers if error
    fileLineBuffer = (char*)calloc(FILE_LINE_BUFFER_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(fileLineBuffer != NULL,
                       PROCESSOR_ERROR_MEMORY_ALLOCATION_ERROR); // free()

    RamStructErrors error = pleaseGiveMeRAM(&processor->ram);
    if (error != RAM_STATUS_OK) {
        LOG_ERROR(getRamErrorMessage(error)); // free() free()
        return PROCESSOR_ERROR_RAM_ERROR;
    }

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

    LOG_DEBUG_VARS(*numOfLines);

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors readCommandsFromFileToArray(uint8_t* array, FILE* file, int numOfLines) {
    IF_ARG_NULL_RETURN(array);
    IF_ARG_NULL_RETURN(file);

    int lineIndex = 0;
    rewind(file);
    // line is considered to be already processed, so it's just a single number
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

typedef ProcessorErrors (normalCommandFuncPtr)(Processor* processor);

struct ProcessorCommandsStruct {
    const char* commandName;
    normalCommandFuncPtr* funcPtr;
};

ProcessorCommandsStruct processorCommandsArr[] = {
    {"halt", haltCommandFunc},
    {"push", pushToProcessorStackFunc},
    {"out",  popAndPrintLastVarInStackFunc},
    {"add",  add2numsFunc},
    {"pop",  popFromProcessorStackFunc},
    {"pick", lookLastVarInVarStackFunc},
    {"sub",  sub2numsFunc},
    {"mul",  mul2numsFunc},
    {"div",  div2numsFunc},
    {"jmp",  procCommandJumpAnyway},
    {"ja",   procCommandJumpIfMore},
    {"jb",   procCommandJumpIfBelow},
    {"je",   procCommandJumpIfEqual},
};

const size_t PROCESSOR_COMMANDS_ARR_SIZE = sizeof(processorCommandsArr) / sizeof(*processorCommandsArr);

static ProcessorErrors checkProcessorCommands(Processor* processor, const char* commandName) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(commandName);

    for (size_t commandIndex = 0; commandIndex < PROCESSOR_COMMANDS_ARR_SIZE; ++commandIndex) {
        if (strcmp(processorCommandsArr[commandIndex].commandName, commandName) != 0)
            continue;

        LOG_DEBUG("----------------------------");
        LOG_DEBUG_VARS(commandName, commandIndex);
        ProcessorErrors error = (*processorCommandsArr[commandIndex].funcPtr)(processor);
        IF_ERR_RETURN(error);
    }

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors runProgramBinary(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    processor->instructionPointer = 0;
    LOG_DEBUG_VARS(processor->instructionPointer);
    while (processor->instructionPointer < processor->numberOfInstructions) {
        int commandIndex = processor->programCode[processor->instructionPointer];
        ++processor->instructionPointer;
        LOG_DEBUG_VARS(processor->instructionPointer, commandIndex);

        CommandStruct command = {};
        CommandErrors error = getCommandByIndex(commandIndex, &command);
        LOG_DEBUG_VARS(command.commandIndex, command.commandName);
        if (error != COMMANDS_STATUS_OK) {
            // FIXME: probably overload happens and that's not function that we are looking for
            LOG_ERROR(getCommandsErrorMessage(error));
            return PROCESSOR_ERROR_COMMANDS_ERROR;
        }

        IF_ERR_RETURN(checkProcessorCommands(processor, command.commandName));

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

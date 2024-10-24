#include <iostream>
#include <sys/stat.h>

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

typedef ProcessorErrors (normalCommandFuncPtr)(Processor* processor);

struct ProcessorCommandsStruct {
    const char* commandName;        // NOTE: will leave this field for debug purposes
    normalCommandFuncPtr* funcPtr;
};

ProcessorCommandsStruct processorCommandsArr[] = {
    {"push", pushToProcessorStackFunc},
    {"add",  add2numsFunc},
    {"sub",  sub2numsFunc},
    {"mul",  mul2numsFunc},
    {"div",  div2numsFunc},
    {"out",  popAndPrintLastVarInStackFunc},
    {"halt", haltCommandFunc},
    {"pop",  popFromProcessorStackFunc},
    {"pick", lookLastVarInVarStackFunc},
    {"jmp",  procCommandJumpAnyway},
    {"jb",   procCommandJumpIfBelow},
    {"ja",   procCommandJumpIfMore},
    {"je",   procCommandJumpIfEqual},
    {"call", procCommandCallFunc},
    {"ret",  procCommandReturnFromFunc},
    {"draw", procCommandDrawFunc},
    {"in",   procCommandInFromTerminal},
    {"mod",  mod2numsFunc},
    {"sqrt", sqrt1numFunc},
    {"meow", meowFunc},
};

const size_t PROCESSOR_COMMANDS_ARR_SIZE = sizeof(processorCommandsArr) / sizeof(*processorCommandsArr);

// static bool areCommandStructsEqual(const CommandStruct* first,
//                                    const CommandStruct* second) {
//     assert(first  != NULL);
//     assert(second != NULL);
//
//     return first->commandIndex == second->commandIndex &&
//            first->commandName  == second->commandName;
// }

static ProcessorErrors checkThat2CommandsArraysMatch() {
    for (size_t commandInd = 0; commandInd < PROCESSOR_COMMANDS_ARR_SIZE; ++commandInd) {
        CommandStruct command = {};
        CommandErrors error = getCommandByIndex(commandInd + 1, &command);
        if (error != COMMANDS_STATUS_OK) {
            LOG_ERROR(getCommandsErrorMessage(error));
            return PROCESSOR_ERROR_COMMANDS_ERROR;
        }

        if (strcmp(command.commandName, processorCommandsArr[commandInd].commandName) != 0) {
            return PROCESSOR_ERROR_COMMAND_ARRAY_IS_NOT_EQ_TO_COMMON;
        }
    }

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors ProcessorConstructor(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    IF_ERR_RETURN(checkThat2CommandsArraysMatch());

    *processor = {}; // name constants ~~~~~|
    // FIXME: const int initialCapacity = 8;
    const int initialCapacity = 8;
    Errors error = constructStack(&processor->stackOfVars, initialCapacity, PROCESSOR_DATA_TYPE_SIZE);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    // ASK: copypaste?
    error = constructStack(&processor->stackOfCalls, initialCapacity, sizeof(size_t));
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

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

    RamStructErrors err = pleaseGiveMeRAM(&processor->ram);
    if (err != RAM_STATUS_OK) {
        LOG_ERROR(getRamErrorMessage(err)); // free() free()
        return PROCESSOR_ERROR_RAM_ERROR;
    }

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors getFileSize(FILE* file, size_t* fileSize) {
    IF_ARG_NULL_RETURN(file);
    IF_ARG_NULL_RETURN(fileSize);

    *fileSize = 0;
    struct stat buff = {};
    int status = fstat(fileno(file), &buff);
    if (status != 0) {
        return PROCESSOR_ERROR_BAD_FSAT_STATUS;
    }

    *fileSize = buff.st_size;

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors readCommandsFromFileToArray(uint8_t* array, FILE* file, size_t numOfBytes) {
    IF_ARG_NULL_RETURN(array);
    IF_ARG_NULL_RETURN(file);

    fread(array, numOfBytes, sizeof(uint8_t), file);

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors readProgramBinary(Processor* processor, const char* binFileName) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(binFileName);

    LOG_DEBUG("read binary");
    FILE* binaryFile = fopen(binFileName, "rb");
    IF_NOT_COND_RETURN(binaryFile != NULL,
                       PROCESSOR_ERROR_COULDNT_OPEN_FILE);

    size_t numOfBytes = 0;
    ProcessorErrors error = getFileSize(binaryFile, &numOfBytes);
    IF_ERR_RETURN(error);
    LOG_DEBUG_VARS(numOfBytes);

    processor->numberOfInstructions = numOfBytes;
    processor->programCode = (uint8_t*)calloc(numOfBytes, sizeof(uint8_t));
    IF_NOT_COND_RETURN(processor->programCode,
                       PROCESSOR_ERROR_MEMORY_ALLOCATION_ERROR);
    error = readCommandsFromFileToArray(processor->programCode, binaryFile, numOfBytes);
    IF_ERR_RETURN(error);

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors checkProcessorCommands(Processor* processor, size_t commandIndex) {
    IF_ARG_NULL_RETURN(processor);
    //IF_ARG_NULL_RETURN(commandName);
    IF_NOT_COND_RETURN(commandIndex < PROCESSOR_COMMANDS_ARR_SIZE,
                       PROCESSOR_ERROR_INVALID_ARGUMENT);

    ProcessorErrors error = (*processorCommandsArr[commandIndex - 1].funcPtr)(processor);
    IF_ERR_RETURN(error);

//     for (size_t commandIndex = 0; commandIndex < PROCESSOR_COMMANDS_ARR_SIZE; ++commandIndex) {
//         if (myStrcmp(processorCommandsArr[commandIndex].commandName, commandName) != 0)
//             continue;
//
//         LOG_DEBUG("----------------------------");
//         LOG_DEBUG_VARS(commandName, commandIndex);
        // ProcessorErrors error = (*processorCommandsArr[commandIndex].funcPtr)(processor);
        // IF_ERR_RETURN(error);
//     }

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

        IF_ERR_RETURN(checkProcessorCommands(processor, command.commandIndex));

        LOG_DEBUG_VARS(processor->instructionPointer, processor->numberOfInstructions);
    }

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors ProcessorDestructor(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    Errors error = destructStack(&processor->stackOfVars);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    error = destructStack(&processor->stackOfCalls);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    FREE(processor->programCode);
    processor = {}; // ASK: is this ok?

    FREE(fileLineBuffer);

    return PROCESSOR_STATUS_OK;
}

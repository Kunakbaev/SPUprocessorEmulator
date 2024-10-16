#include <iostream>
#include <string.h>

#include "../include/assembly.hpp"
#include "../../common/commands/include/commands.hpp"




#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, ASSEMBLER_ERROR_INVALID_ARGUMENT, getAssemblerErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getAssemblerErrorMessage, COMMANDS_STATUS_OK)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getAssemblerErrorMessage)




const size_t FILE_LINE_BUFFER_SIZE = 1 << 10;

char* fileLineBuffer;

AssemblerErrors constructAssembler() {
    fileLineBuffer = (char*)calloc(FILE_LINE_BUFFER_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(fileLineBuffer != NULL,
                       ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);

    return ASSEMBLER_STATUS_OK;
}

static bool isLineCommand(const char* line) {
    if (line == NULL) { // FIXME:
        assert(false);
        return false;
    }

    return isalpha(line[0]); // ASK: is this ok?
}

static AssemblerErrors readCommandsFromFileToArray(FILE* source, FILE* dest) {
    IF_ARG_NULL_RETURN(source);
    IF_ARG_NULL_RETURN(dest);

    LOG_DEBUG("read");
    while (fgets(fileLineBuffer, FILE_LINE_BUFFER_SIZE, source)) {
        // somehow get command index
        CommandStruct command = {};

        int num = 0;
        LOG_DEBUG_VARS(fileLineBuffer);
        size_t lineLen = strlen(fileLineBuffer);
        if (lineLen >= 1)
            *(fileLineBuffer + lineLen - 1) = '\0';

        // TODO: check that num of spaces is <= 1
        // check that command is in right format
        // check that command exists
        char* firstSpacePtr = strchr(fileLineBuffer, ' ');
        LOG_DEBUG_VARS(firstSpacePtr);
        if (firstSpacePtr != NULL)
            *firstSpacePtr = '\0';
        const char* commandName = fileLineBuffer;
        const char* argument = "";
        if (firstSpacePtr != NULL)
            argument = firstSpacePtr + 1; // WARNING: can overflow
        LOG_DEBUG_VARS(commandName, argument);

        CommandErrors error = getCommandByName(commandName, &command); // ???
        if (error != COMMANDS_STATUS_OK) {
            LOG_ERROR(getCommandsErrorMessage(error));
            return ASSEMBLER_ERROR_COMMAND_ERROR;
        }

        LOG_DEBUG_VARS(command.commandIndex, command.commandName);
        num = command.commandIndex;

        fprintf(dest, "%d\n", num); // ???
        if (strlen(argument) != 0) {
            // FIXME: what to do if it's double
            fprintf(dest, "%d\n", atoi(argument));
        }
    }

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors compileProgram(const char* sourceFileName,
                               const char*   destFileName) {
    IF_ARG_NULL_RETURN(sourceFileName);

    FILE* sourceFile = fopen(sourceFileName, "r");
    IF_NOT_COND_RETURN(sourceFile != NULL,
                       ASSEMBLER_ERROR_COULDNT_OPEN_FILE);

    LOG_DEBUG("opened source file");
    FILE* destFile   = fopen(destFileName, "w"); // FIXME: close first file
    if (destFile == NULL) {
        fclose(sourceFile); // hope that it will close
        LOG_ERROR(getAssemblerErrorMessage(ASSEMBLER_ERROR_COULDNT_OPEN_FILE));
        return ASSEMBLER_ERROR_COULDNT_OPEN_FILE;
    }

    LOG_DEBUG("opened dest file");
    AssemblerErrors error = readCommandsFromFileToArray(sourceFile, destFile);
    IF_ERR_RETURN(error); // FIXME: close files

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors destructAssembler() {
    FREE(fileLineBuffer);
    return ASSEMBLER_STATUS_OK;
}

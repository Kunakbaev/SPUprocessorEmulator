#include <iostream>
#include <string.h>

#include "../include/assembly.hpp"
#include "../../common/include/commands.hpp"

const size_t FILE_LINE_BUFFER_SIZE = 1 << 10;

char* fileLineBuffer;

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

    while (fgets(fileLineBuffer, FILE_LINE_BUFFER_SIZE, source)) {
        // somehow get command index
        CommandStruct command = {};

        int num = 0;
        if (isLineCommand(fileLineBuffer)) {
            Errors error = getCommandByName(fileLineBuffer, &command); // ???
        } else {
            num = atoi(fileLineBuffer);
        }

        fprintf(dest, "%d\n", num); // ???
    }

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors compileProgram(const char* sourceFileName,
                               const char*   destFileName) {
    IF_ARG_NULL_RETURN(sourceFileName);

    FILE* sourceFile = fopen(sourceFileName, "r");
    IF_NOT_COND_RETURN(sourceFile != NULL,
                       ASSEMBLER_ERROR_COULDNT_OPEN_FILE);

    FILE* destFile   = fopen(destFileName, "w"); // FIXME: close first file
    IF_NOT_COND_RETURN(destFileName != NULL,
                       ASSEMBLER_ERROR_COULDNT_OPEN_FILE);

    AssemblerErrors error = readCommandsFromFileToArray(sourceFile, destFile);
    IF_ERR_RETURN(error); // FIXME: close files

    return ASSEMBLER_STATUS_OK;
}

Errors destructAssembler() {
    FREE(fileLineBuffer);
    return STATUS_OK;
}

#include <iostream>
#include <string.h>
#include <sys/stat.h>

#include "../include/assembly.hpp"
#include "../../common/commands/include/commands.hpp"




#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, ASSEMBLER_ERROR_INVALID_ARGUMENT, getAssemblerErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getAssemblerErrorMessage, COMMANDS_STATUS_OK)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getAssemblerErrorMessage)




const size_t FILE_BUFFER_SIZE      = 1 << 12;
const size_t FILE_LINE_BUFFER_SIZE = 1 << 10;
const size_t MAX_NUM_OF_WORDS      = 1 << 10;
const char   COMMENT_BEGIN_CHAR    = ';';

char* fileLineBuffer;
char* fileBuffer;
const char** words;

AssemblerErrors constructAssembler() {
    fileLineBuffer = (char*)calloc(FILE_LINE_BUFFER_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(fileLineBuffer != NULL,
                       ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors getFileSize(FILE* file, size_t* fileSize) {
    IF_ARG_NULL_RETURN(file);
    IF_ARG_NULL_RETURN(fileSize);

    *fileSize = 0;
    struct stat buff = {};
    int status = fstat(fileno(file), &buff);
    if (status != 0) {
        return ASSEMBLER_ERROR_FILE_STAT_ERROR;
    }

    *fileSize = buff.st_size;

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors clearCodeAfterComment(char* line) {
    IF_ARG_NULL_RETURN(line);

    bool wasCommentBegin = false;
    char* ptr = line;
    while (*ptr != '\0') {
        wasCommentBegin |= *ptr == COMMENT_BEGIN_CHAR;
        if (wasCommentBegin) {
            *ptr = '\0';
        }

        ++ptr;
    }

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors argToNumber(const char* arg, int* number, int* mask) {
    IF_ARG_NULL_RETURN(arg);
    IF_ARG_NULL_RETURN(mask);

    // TODO: rename consts
    *number = -1;
    CommandErrors error = findRegName(arg, number);
    if (error != COMMANDS_STATUS_OK) {
        LOG_ERROR(getCommandsErrorMessage(error));
        return ASSEMBLER_ERROR_COMMAND_ERROR;
    }

    if (*number != -1) {
        *mask |= 2; // this is register name
    } else {
        *mask |= 1; // this is const number
        // TODO: what to do with long double?
        *number = atoi(arg);
    }

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors getArgumentMask(char* arg, int* mask, int* first, int* second) {
    IF_ARG_NULL_RETURN(arg);

    size_t argLen = strlen(arg);
    LOG_DEBUG_VARS(argLen, arg);
    if (arg[0] == '[') {
        arg[argLen - 1] = '\0';
        ++arg; // ASK: is this ok?
        argLen -= 2;
        *mask |= 4; // this is RAM argument
    }

    *first  = -1;
    *second = -1;
    char* delimPtr = strchr(arg, '+');
    if (delimPtr == NULL) { // not a complex function
        IF_ERR_RETURN(argToNumber(arg, first, mask)); // ASK: is this ok?
    } else {
        *delimPtr = '\0';
        IF_ERR_RETURN(argToNumber(arg,          first,  mask));
        IF_ERR_RETURN(argToNumber(delimPtr + 1, second, mask));
    }

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors parseLineOfCode(char* lineOfCode, FILE* dest) {
    IF_ARG_NULL_RETURN(lineOfCode);
    IF_ARG_NULL_RETURN(dest);

    IF_ERR_RETURN(clearCodeAfterComment(lineOfCode));

    char* delimPtr = strchr(lineOfCode, ' ');
    size_t lineOfCodeLen = strlen(lineOfCode);
    lineOfCode[lineOfCodeLen - 1] = '\0';
    if (delimPtr != NULL) { // command has arguments
        *delimPtr = '\0';
    }

    char* commandName = lineOfCode;
    CommandStruct command = {};
    CommandErrors error = getCommandByName(commandName, &command);
    if (error != COMMANDS_STATUS_OK) {
        LOG_DEBUG(getCommandsErrorMessage(error));
        return ASSEMBLER_ERROR_COMMAND_ERROR;
    }

    fprintf(dest, "%d\n", command.commandIndex);
    if (delimPtr == NULL)
        return ASSEMBLER_STATUS_OK;

    int mask  = 0;
    int first = -1, second = -1;

    IF_ERR_RETURN(getArgumentMask(delimPtr + 1, &mask, &first, &second));
    LOG_DEBUG_VARS(lineOfCode, delimPtr + 1, mask, first, second);

    fprintf(dest, "%d\n", mask);
    if (first  != -1)
        fprintf(dest, "%d\n", first);
    if (second != -1)
        fprintf(dest, "%d\n", second);

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors readCommandsFromFileToArray(FILE* source, FILE* dest) {
    IF_ARG_NULL_RETURN(source);
    IF_ARG_NULL_RETURN(dest);

    while (fgets(fileLineBuffer, FILE_LINE_BUFFER_SIZE, source)) {
        LOG_DEBUG_VARS(fileLineBuffer);

        parseLineOfCode(fileLineBuffer, dest);
    }

    // counting num of words is too messy
//     words = (const char**)calloc(MAX_NUM_OF_WORDS, sizeof(const char*));
//     IF_NOT_COND_RETURN(words != NULL, ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);
//
//     size_t sizeOfFile = 0;
//     AssemblerErrors error = getFileSize(source, &sizeOfFile);
//     IF_ERR_RETURN(error);
//
//     fileBuffer = (char*)calloc(sizeOfFile + 1, sizeof(char));
//     IF_NOT_COND_RETURN(fileBuffer != NULL,
//                        ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);
//
//     fread(fileBuffer, sizeof(char), sizeOfFile, source);
//     const char* ptr = fileBuffer;
//
//     size_t prevInd = 0;
//     size_t wordInd = 0;
//     for (size_t charInd = 0; charInd < sizeOfFile; ++charInd) {
//         char ch = fileBuffer[charInd];
//         if (ch != ' ' && ch != '\t')
//             continue;
//
//         for (size_t word = 0; word < wordInd; ++word) {
//             LOG_DEBUG_VARS(word, words[word]);
//         }
//
//         words[wordInd] = fileBuffer + prevInd;
//         fileBuffer[charInd] = '\0';
//         prevInd = charInd + 1;
//
//         if (ch == '\n') {
//             parseLineOfCode(wordInd, source, dest);
//         }
//
//         ++wordInd;
//     }

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

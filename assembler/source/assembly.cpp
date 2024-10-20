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




const size_t FILE_LINE_BUFFER_SIZE = 1 << 10;
const char   COMMENT_BEGIN_CHAR    = ';'; // maybe make it assembler arg
const size_t MAX_LEN_OF_CODE       = 1 << 10;

char* fileLineBuffer;

AssemblerErrors constructAssembler(Assembler* assembler,
                                   const char* sourceFileName,
                                   const char* destFileName) {
    IF_ARG_NULL_RETURN(assembler);

    fileLineBuffer = (char*)calloc(FILE_LINE_BUFFER_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(fileLineBuffer != NULL,
                       ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);

    *assembler = {};
    assembler->instructionPointer = 0;
    assembler->lines              = NULL;
    assembler->sourceFileName     = sourceFileName;
    assembler->destFileName       = destFileName;
    assembler->programCode        = (uint8_t*)calloc(MAX_LEN_OF_CODE, sizeof(uint8_t));
    IF_NOT_COND_RETURN(assembler->programCode != NULL,
                       ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);



    return ASSEMBLER_STATUS_OK;
}

// FIXME: Хочется чтобы функции были сверху вниз по важности.

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

// deletes assebmly comment from line (all that goes after ; symbol)
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

// depending on what was in arg string, saves found int to either numArg or regArg
static AssemblerErrors argsToNumber(const char* arg, int* numArg, int* regArg, int* mask) {
    IF_ARG_NULL_RETURN(arg);
    IF_ARG_NULL_RETURN(mask);

    // checking if arg is register name
    int tmpNum = -1;
    CommandErrors error = findRegName(arg, &tmpNum);
    if (error != COMMANDS_STATUS_OK) {
        LOG_ERROR(getCommandsErrorMessage(error));
        return ASSEMBLER_ERROR_COMMAND_ERROR;
    }

    LOG_DEBUG_VARS(arg, *regArg);
    if (tmpNum != -1) {
        *mask  |= HAS_REG_ARG; // this is register nameg
        *regArg = tmpNum;
    } else {
        *mask  |= HAS_NUM_ARG; // this is const number
        // TODO: what to do with long double?
        *numArg = atoi(arg); // FIXME: strtol
    }

    return ASSEMBLER_STATUS_OK;
}

// ASK: is it ok? no error check?
// if we have ram argument, then we trim argument and set according bit in mask
static void tryRamArg(char** arg, int* mask) {
    assert(arg  != NULL);
    assert(*arg != NULL);
    assert(mask != NULL);

    size_t argLen = strlen(*arg);
    if ((*arg)[0] != '[') return;

    // in case we found pair of [] brackets
    assert((*arg)[argLen - 1] == ']');
    (*arg)[argLen - 1] = '\0';
    ++(*arg);
    *mask |= HAS_RAM_ARG; // this is RAM argument
}

// if returns true, than 2 args were successfully found, otherwise returns false
static bool trySumOfArgs(char* arg, int* mask, int* numArg, int* regArg) {
    assert(arg    != NULL);
    assert(numArg != NULL);
    assert(regArg != NULL);

    char* delimPtr = strchr(arg, '+');
    if (delimPtr == NULL)
        return false;

    LOG_DEBUG_VARS(arg);
    *delimPtr = '\0';
    argsToNumber(arg,          numArg, regArg, mask);
    argsToNumber(delimPtr + 1, numArg, regArg, mask);
    *delimPtr = '+'; // arg is not changed

    return true;
}

static AssemblerErrors getArgumentMask(char* arg, int* mask, int* numArg, int* regArg) {
    IF_ARG_NULL_RETURN(arg);

    tryRamArg(&arg, mask);
    // LOG_DEBUG_VARS(arg, mask);
    if (trySumOfArgs(arg, mask, numArg, regArg))
        return ASSEMBLER_STATUS_OK;

    IF_ERR_RETURN(argsToNumber(arg, numArg, regArg, mask));

    return ASSEMBLER_STATUS_OK;
}

// saves byte to programCode array and moves instructionPointer
static AssemblerErrors addByteToProgramCodeArray(Assembler* assembler, uint8_t byte) {
    IF_ARG_NULL_RETURN(assembler);
    IF_NOT_COND_RETURN(assembler->instructionPointer < MAX_LEN_OF_CODE,
                        ASSEMBLER_ERROR_INVALID_ARGUMENT); // TODO: error handler

    assembler->programCode[assembler->instructionPointer++] = byte;

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors saveBytesToArray(Assembler* assembler, const uint8_t* source, size_t numOfBytes) {
    IF_ARG_NULL_RETURN(assembler);
    IF_ARG_NULL_RETURN(source);

    for (size_t byteInd = 0; byteInd < numOfBytes; ++byteInd) {
        uint8_t byte = source[byteInd];
        //LOG_DEBUG_VARS(byteInd, byte);
        IF_ERR_RETURN(addByteToProgramCodeArray(assembler, byte));
    }

    return ASSEMBLER_STATUS_OK;
}

// save bytes from num (can be int or double)
static AssemblerErrors addNumBytes(Assembler* assembler, processor_data_type num) {
    IF_ARG_NULL_RETURN(assembler);
    int numOfBytes = sizeof(processor_data_type);
    //LOG_DEBUG_VARS(num, numOfBytes);
    IF_ERR_RETURN(saveBytesToArray(assembler, (uint8_t*)&num, numOfBytes));

    return ASSEMBLER_STATUS_OK;
}

// returns ptr to char* ptr to a begining of args in lineOfCode, if NULL is returned, than no args were found
static char* getCommandArgsPtrAndSepCommandName(char* lineOfCode) {
    assert(lineOfCode != NULL);

    char* delimPtr = strchr(lineOfCode, ' ');
    if (delimPtr != NULL) { // command has arguments
        *delimPtr = '\0';
        return delimPtr + 1; // FIXME: add check that space is not last char
    }

    return NULL;
}

// considers that lineOfCode already has been parsed, so prefix is command name and then goes \0
static AssemblerErrors getCommandIndex(const char* lineOfCode, int* commandIndex) {
    IF_ARG_NULL_RETURN(commandIndex);

    CommandStruct command = {};
    CommandErrors error = getCommandByName(lineOfCode, &command);
    if (error != COMMANDS_STATUS_OK) {
        LOG_ERROR(getCommandsErrorMessage(error));
        return ASSEMBLER_ERROR_COMMAND_ERROR;
    }

    *commandIndex = command.commandIndex;

    return ASSEMBLER_STATUS_OK;
}

// string is char*, however we don't wanna change it, so before return, we have to roll back to inital state
static AssemblerErrors parseLineOfCode(Assembler* assembler, char* lineOfCode) {
    // we have 2 passes when we look at the code lines, so we don't wanna change them
    // FIXME: calloc каждый раз - кринж!
    // 1) Всё таки менять строки
    // 2) Хотя бы делать calloc один раз

    IF_ARG_NULL_RETURN(lineOfCode);

    int commandIndex = -1;
    char* argPtr = getCommandArgsPtrAndSepCommandName(lineOfCode);
    IF_ERR_RETURN(getCommandIndex(lineOfCode, &commandIndex));
    IF_ERR_RETURN(addByteToProgramCodeArray(assembler, commandIndex));

    if (argPtr == NULL)
        return ASSEMBLER_STATUS_OK;

    // WTF???
    int mask   = 0;
    int numArg = -1,
        regArg = -1;

    IF_ERR_RETURN(getArgumentMask(argPtr, &mask, &numArg, &regArg));
    // LOG_DEBUG_VARS(lineOfCode, argPtr, mask, numArg, regArg);

    IF_ERR_RETURN(addByteToProgramCodeArray(assembler, mask));
    // WARNING: first we output const and then register
    LOG_DEBUG_VARS(mask, numArg, regArg);
    if (numArg  != -1)
        IF_ERR_RETURN(addNumBytes(assembler, numArg));
    if (regArg != -1)
        IF_ERR_RETURN(addByteToProgramCodeArray(assembler, regArg));

    // ASK: is this ok?
    *(argPtr - 1) = ' '; // returning string to initial state
    //lineOfCode = strcat(lineOfCode, argPtr);

    LOG_DEBUG_VARS(lineOfCode);

    return ASSEMBLER_STATUS_OK;
}

//                     FIXME: getNLinesInFile
static AssemblerErrors getNumOfLinesInFile(FILE* source, size_t* numOfLines) {
    IF_ARG_NULL_RETURN(source);
    IF_ARG_NULL_RETURN(numOfLines);

    *numOfLines = 0;
    while (fgets(fileLineBuffer, FILE_LINE_BUFFER_SIZE, source)) {
        // LOG_DEBUG_VARS(fileLineBuffer);
        ++(*numOfLines);
    }
    rewind(source);

    return ASSEMBLER_STATUS_OK;
}

// TODO: написать комментарий: почему так а не 2 функции
static AssemblerErrors readLinesFromFileAndRemoveComments(Assembler* assembler) {
    IF_ARG_NULL_RETURN(assembler);

    FILE* source = fopen(assembler->sourceFileName, "r");
    IF_NOT_COND_RETURN(source != NULL,
                       ASSEMBLER_ERROR_COULDNT_OPEN_FILE);

    assembler->numOfLines = 0;
    IF_ERR_RETURN(getNumOfLinesInFile(source, &assembler->numOfLines));

    // FIXME: +1 because i'm too afraid
    assembler->lines = (char**)calloc(assembler->numOfLines + 1, sizeof(char*));
    IF_NOT_COND_RETURN(assembler->lines != NULL,
                       ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);

    size_t lineInd = 0;
    while (fgets(fileLineBuffer, FILE_LINE_BUFFER_SIZE, source)) {
        LOG_DEBUG_VARS(fileLineBuffer);
        IF_ERR_RETURN(clearCodeAfterComment(fileLineBuffer));
        size_t lineOfCodeLen = strlen(fileLineBuffer);
        fileLineBuffer[lineOfCodeLen - 1] = '\0'; // remove \n

        LOG_DEBUG_VARS(lineOfCodeLen, fileLineBuffer);
        assembler->lines[lineInd] = (char*)calloc(lineOfCodeLen + 1, sizeof(char));
        IF_NOT_COND_RETURN(assembler->lines[lineInd] != NULL,
                           ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);
        strcpy(assembler->lines[lineInd], fileLineBuffer);
        ++lineInd;
    }

    // source file is no longer useful
    fclose(source);
    LOG_DEBUG_VARS(assembler->numOfLines, assembler->lines[0], assembler->lines[1]);

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors processCodeLines(Assembler* assembler) {
    IF_ARG_NULL_RETURN(assembler);

    assembler->instructionPointer = 0;
    for (size_t lineInd = 0; lineInd < assembler->numOfLines; ++lineInd) {
        IF_ERR_RETURN(parseLineOfCode(assembler, assembler->lines[lineInd]));
    }

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors saveProgramCodeToDestFile(const Assembler* assembler) {
    IF_ARG_NULL_RETURN(assembler);

    FILE* destFile = fopen(assembler->destFileName, "w");
    IF_NOT_COND_RETURN(destFile != NULL,
                       ASSEMBLER_ERROR_COULDNT_OPEN_FILE);

    // TODO: save to binary file
    for (size_t lineInd = 0; lineInd < assembler->instructionPointer; ++lineInd) {
        fprintf(destFile, "%d\n", assembler->programCode[lineInd]);
    }
    fclose(destFile);

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors compileProgram(Assembler* assembler) {
    IF_ARG_NULL_RETURN(assembler);

    // FIXME: ну ты понял ))
    IF_ERR_RETURN(readLinesFromFileAndRemoveComments(assembler));
    IF_ERR_RETURN(processCodeLines(assembler));
    IF_ERR_RETURN(saveProgramCodeToDestFile(assembler));

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors destructAssembler(Assembler* assembler) {
    FREE(fileLineBuffer);
    FREE(assembler->programCode);
    FREE(assembler->lines);

    assembler = {}; // clear struct values just in case

    return ASSEMBLER_STATUS_OK;
}

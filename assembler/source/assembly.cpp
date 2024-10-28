#include "assemblyCommonDefines.cpp"

const size_t FILE_SIZE             = 1 << 15;
const char   COMMENT_BEGIN_CHAR    = ';'; // maybe make it assembler arg
const size_t MAX_LEN_OF_CODE       = 1 << 10;
const char*  DELIMS = " \n\t";

char* fileBuffer;

static bool isDelim(char ch) {
    return strchr(DELIMS, ch) != NULL;
}

AssemblerErrors constructAssembler(Assembler* assembler,
                                   const char* sourceFileName,
                                   const char* destFileName) {
    IF_ARG_NULL_RETURN(assembler);

    *assembler = {};
    assembler->numOfBytesInDest   = 0;
    assembler->lines              = NULL;
    assembler->sourceFileName     = sourceFileName;
    assembler->destFileName       = destFileName;
    assembler->programCode        = (uint8_t*)calloc(MAX_LEN_OF_CODE, sizeof(uint8_t));
    IF_NOT_COND_RETURN(assembler->programCode != NULL,
                       ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);

    TABLE_OF_LABELS_ERR_CHECK(constructTableOfLabels());
    COMMANDS_ERR_CHECK(validateCommands());

    return ASSEMBLER_STATUS_OK;
}

// FIXME: Хочется чтобы функции были сверху вниз по важности.

// depending on what was in arg string, saves found int to either numArg or regArg
static AssemblerErrors argsToNumber(const char* arg, processor_data_type* numArg, int* regArg, int* mask) {
    IF_ARG_NULL_RETURN(arg);
    IF_ARG_NULL_RETURN(mask);

    // checking if arg is register name
    int tmpNum = -1;
    COMMANDS_ERR_CHECK(findRegName(arg, &tmpNum));

    LOG_DEBUG_VARS(arg, *regArg);
    if (tmpNum != -1) {
        *mask  |= HAS_REG_ARG; // this is register name
        *regArg = tmpNum;
    } else {
        *mask  |= HAS_NUM_ARG; // this is const number
        errno = 0;
        *numArg = strtold(arg, NULL);
        IF_NOT_COND_RETURN(errno == 0, ASSEMBLER_ERROR_STRTOLD_ERROR);
    }

    return ASSEMBLER_STATUS_OK;
}

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
static bool trySumOfArgs(char* arg, int* mask, processor_data_type* numArg, int* regArg) {
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

static AssemblerErrors getArgumentMask(char* arg, int* mask, processor_data_type* numArg, int* regArg) {
    IF_ARG_NULL_RETURN(arg);

    tryRamArg(&arg, mask);
    size_t argLen = strlen(arg);

    LOG_DEBUG_VARS(arg, mask);
    if (trySumOfArgs(arg, mask, numArg, regArg)) {
        if (*mask & HAS_RAM_ARG) arg[argLen] = ']';
        return ASSEMBLER_STATUS_OK;
    }

    IF_ERR_RETURN(argsToNumber(arg, numArg, regArg, mask));
    if (*mask & HAS_RAM_ARG) arg[argLen] = ']';

    return ASSEMBLER_STATUS_OK;
}

// saves byte to programCode array and moves instructionPointer
static AssemblerErrors addByteToProgramCodeArray(Assembler* assembler, uint8_t byte) {
    IF_ARG_NULL_RETURN(assembler);
    IF_NOT_COND_RETURN(assembler->numOfBytesInDest < MAX_LEN_OF_CODE,
                        ASSEMBLER_ERROR_INVALID_ARGUMENT); // TODO: error handler

    assembler->programCode[assembler->numOfBytesInDest++] = byte;

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors saveBytesToArray(Assembler* assembler, const uint8_t* source, size_t numOfBytes) {
    IF_ARG_NULL_RETURN(assembler);
    IF_ARG_NULL_RETURN(source);

    for (size_t byteInd = 0; byteInd < numOfBytes; ++byteInd) {
        uint8_t byte = source[byteInd];
        IF_ERR_RETURN(addByteToProgramCodeArray(assembler, byte));
    }

    return ASSEMBLER_STATUS_OK;
}

// save bytes from num (can be int or double)
static AssemblerErrors addNumBytes(Assembler* assembler, processor_data_type num) {
    IF_ARG_NULL_RETURN(assembler);
    int numOfBytes = sizeof(processor_data_type);
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
    IF_ARG_NULL_RETURN(lineOfCode);

    CommandStruct command = {};
    COMMANDS_ERR_CHECK(getCommandByName(lineOfCode, &command));
    *commandIndex = command.commandIndex;

    return ASSEMBLER_STATUS_OK;
}

static bool isStringLabel(const char* line) {
    assert(line != NULL);
    char* charPtr  = strchr((char*)line, ':');
    char* spacePtr = strchr((char*)line, ' ');
    return charPtr != NULL && *(charPtr + 1) == '\0' && spacePtr == NULL;
}

// if label is found, saves its code line to special array and then
static AssemblerErrors tryLabel(Assembler* assembler, char* line, bool* is) {
    IF_ARG_NULL_RETURN(assembler);
    IF_ARG_NULL_RETURN(line);
    IF_ARG_NULL_RETURN(is);

    *is = false;
    if (!isStringLabel(line))
        return ASSEMBLER_STATUS_OK;

    Label label = {line, assembler->numOfBytesInDest};
    TABLE_OF_LABELS_ERR_CHECK(addLabelName(&label));

    *is = true;
    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors saveLabelCode(Assembler* assembler, const char* arg) {
    IF_ARG_NULL_RETURN(arg);

    Label label = {};
    TABLE_OF_LABELS_ERR_CHECK(getNumOfCodeLineByLabel(arg, &label));
    LOG_DEBUG_VARS(arg, label.codeLineInd, label.labelName);

    saveBytesToArray(assembler, (const uint8_t*)&label.codeLineInd, 4);

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors tryJumpCommand(Assembler* assembler, char* lineOfCode, char* argPtr, bool* is) {
    IF_ARG_NULL_RETURN(assembler);
    IF_ARG_NULL_RETURN(lineOfCode);
    IF_ARG_NULL_RETURN(argPtr);
    IF_ARG_NULL_RETURN(is);

    *is = false;
    COMMANDS_ERR_CHECK(isJumpCommand(lineOfCode, is));
    LOG_DEBUG_VARS("isJumpCommand", *is);
    if (!*is)
        return ASSEMBLER_STATUS_OK;

    LOG_DEBUG_VARS(lineOfCode, argPtr);

    Label label = {argPtr, -1};
    TABLE_OF_LABELS_ERR_CHECK(addLabelName(&label));

    IF_ERR_RETURN(saveLabelCode(assembler, argPtr));
    *is = true;
    *(argPtr - 1) = ' '; // returning string to initial state

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors removeAllSpaceFromArgument(char* arg) {
    IF_ARG_NULL_RETURN(arg);

    char* ptrL = arg;
    char* ptrR = arg;
    while (*ptrR != '\0') {
        if (!isDelim(*ptrR)) {
            *ptrL = *ptrR;
            ++ptrL;
        }

        ++ptrR;
    }
    *ptrL = '\0';

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors saveAndValidateCommandArgs(Assembler* assembler,
                                                  char* lineOfCode, char* argPtr,
                                                  int* mask) {
    IF_ARG_NULL_RETURN(assembler);
    IF_ARG_NULL_RETURN(lineOfCode);
    IF_ARG_NULL_RETURN(argPtr);
    IF_ARG_NULL_RETURN(mask);

    processor_data_type numArg = -1;
    int                 regArg = -1;

    LOG_DEBUG_VARS(argPtr);
    IF_ERR_RETURN(getArgumentMask(argPtr, mask, &numArg, &regArg));
    LOG_DEBUG_VARS(lineOfCode, argPtr, mask, numArg, regArg);

    IF_ERR_RETURN(addByteToProgramCodeArray(assembler, *mask));
    // WARNING: first we output const and then register
    if (numArg  != -1) IF_ERR_RETURN(addNumBytes(              assembler, numArg));
    if (regArg != -1)  IF_ERR_RETURN(addByteToProgramCodeArray(assembler, regArg));
    *(argPtr - 1) = ' '; // returning string to initial state

    return ASSEMBLER_STATUS_OK;
}

// string is char*, however we don't wanna change it, so before return, we have to roll back to inital state
static AssemblerErrors parseLineOfCode(Assembler* assembler, char* lineOfCode) {
    // we have 2 passes when we look at the code lines, so we don't wanna change them
    IF_ARG_NULL_RETURN(lineOfCode);
    IF_ARG_NULL_RETURN(assembler);

    bool success = false;
    IF_ERR_RETURN(tryLabel(assembler, lineOfCode, &success));
    if (success) {
        return ASSEMBLER_STATUS_OK;
    }

    int commandIndex = -1;
    char* argPtr = getCommandArgsPtrAndSepCommandName(lineOfCode);
    LOG_DEBUG_VARS(lineOfCode, argPtr);
    IF_ERR_RETURN(getCommandIndex(lineOfCode, &commandIndex));
    IF_ERR_RETURN(addByteToProgramCodeArray(assembler, commandIndex));

    int mask = 0;
    if (argPtr != NULL) {
        IF_ERR_RETURN(removeAllSpaceFromArgument(argPtr));

        success = false;
        IF_ERR_RETURN(tryJumpCommand(assembler, lineOfCode, argPtr, &success));
        if (success)
            return ASSEMBLER_STATUS_OK;

        IF_ERR_RETURN(saveAndValidateCommandArgs(assembler, lineOfCode, argPtr, &mask));
    }

    success = false;
    COMMANDS_ERR_CHECK(checkIfGoodArgMaskForCommand(commandIndex, mask, &success));
    IF_NOT_COND_RETURN(success, ASSEMBLER_ERROR_BAD_ARGS_FOR_COMMAND);

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors processCodeLines(Assembler* assembler) {
    IF_ARG_NULL_RETURN(assembler);

    assembler->numOfBytesInDest = 0;
    for (size_t lineInd = 0; lineInd < assembler->numOfLines; ++lineInd) {
        char* line = assembler->lines[lineInd];
        IF_ERR_RETURN(parseLineOfCode(assembler, line));
    }

    return ASSEMBLER_STATUS_OK;
}

#include "inputFilePreprocessingAndFileInterfaceFuncs.cpp"

AssemblerErrors compileProgram(Assembler* assembler) {
    IF_ARG_NULL_RETURN(assembler);

    IF_ERR_RETURN(readLinesFromFileAndRemoveComments(assembler, &fileBuffer,
                                                     FILE_SIZE, COMMENT_BEGIN_CHAR, isDelim));
    IF_ERR_RETURN(processCodeLines(assembler));
    // running second time, because we didn't know where labels led to on the first run
    LOG_DEBUG("-----------------------------");
    printAllLabels();
    IF_ERR_RETURN(processCodeLines(assembler));
    IF_ERR_RETURN(saveProgramCodeToDestFile(assembler));

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors destructAssembler(Assembler* assembler) {
    IF_ARG_NULL_RETURN(assembler);

    FREE(assembler->programCode);
    FREE(assembler->lines);
    FREE(fileBuffer);

    assembler = {}; // clear struct values just in case
    destructTableOfLabels();

    return ASSEMBLER_STATUS_OK;
}

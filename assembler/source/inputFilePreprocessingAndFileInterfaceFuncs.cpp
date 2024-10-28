#include "assemblyCommonDefines.cpp"

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

static AssemblerErrors getNumOfLinesAndReplaceWith0InFile(size_t fileSize, size_t* numOfLines, char* fileBuffer) {
    IF_ARG_NULL_RETURN(numOfLines);

    *numOfLines = 0;
    for (size_t charInd = 0; charInd < fileSize; charInd++) {
        if (fileBuffer[charInd] == '\n') {
            ++(*numOfLines);
            fileBuffer[charInd] = '\0';
        }
    }
    ++(*numOfLines);

    return ASSEMBLER_STATUS_OK;
}

// deletes assebmly comment from line (all that goes after ; symbol)
static AssemblerErrors clearCodeAfterComment(char* line, const char COMMENT_BEGIN_CHAR_CONST) {
    IF_ARG_NULL_RETURN(line);

    bool wasCommentBegin = false;
    char* ptr = line;
    while (*ptr != '\0') {
        wasCommentBegin |= *ptr == COMMENT_BEGIN_CHAR_CONST;
        if (wasCommentBegin)
            *ptr = '\0';

        ++ptr;
    }

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors prepareString(char** line, const char COMMENT_CHAR_BEGIN, isDelimFuncPtr isDelim) {
    IF_ARG_NULL_RETURN(line);
    IF_ARG_NULL_RETURN(*line);

    IF_ERR_RETURN(clearCodeAfterComment(*line, COMMENT_CHAR_BEGIN));
    size_t lineOfCodeLen = strlen(*line);
    char* endPtr = *line + lineOfCodeLen - 1;
    while (lineOfCodeLen > 0 && isDelim(*endPtr)) {
        *endPtr = '\0';
        --lineOfCodeLen;
        --endPtr;
    }

    while (**line != '\0' && isDelim(**line))
        ++(*line);

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors allocateMemForLines(FILE* source, Assembler* assembler, char** fileBuffer, size_t FILE_SIZE) {
    IF_ARG_NULL_RETURN(source);
    IF_ARG_NULL_RETURN(fileBuffer);
    IF_ARG_NULL_RETURN(assembler);

    size_t fileSize = 0;
    IF_ERR_RETURN(getFileSize(source, &fileSize));
    LOG_DEBUG_VARS(fileSize);

    IF_NOT_COND_RETURN(fileSize < FILE_SIZE, ASSEBMLER_ERROR_FILE_IS_TOO_BIG);
    *fileBuffer = (char*)calloc(fileSize + 1, sizeof(char));
    LOG_DEBUG_VARS("file", *fileBuffer, fileSize);
    IF_NOT_COND_RETURN(*fileBuffer != NULL,
                       ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);
    fread(*fileBuffer, sizeof(char), fileSize, source);
    LOG_DEBUG_VARS(*fileBuffer);

    assembler->numOfLines = 0;
    IF_ERR_RETURN(getNumOfLinesAndReplaceWith0InFile(fileSize, &assembler->numOfLines, *fileBuffer));

    assembler->lines = (char**)calloc(assembler->numOfLines, sizeof(char*));
    IF_NOT_COND_RETURN(assembler->lines != NULL,
                       ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR);

    return ASSEMBLER_STATUS_OK;
}

static AssemblerErrors readLinesFromFileAndRemoveComments(Assembler* assembler, char** fileBuffer,
                                                          size_t FILE_SIZE, const char COMMENT_CHAR_BEGIN,
                                                          isDelimFuncPtr isDelim) {
    IF_ARG_NULL_RETURN(assembler);

    FILE* source = fopen(assembler->sourceFileName, "r");
    IF_NOT_COND_RETURN(source != NULL,
                       ASSEMBLER_ERROR_COULDNT_OPEN_FILE);
    IF_ERR_RETURN(allocateMemForLines(source, assembler, fileBuffer, FILE_SIZE));

    size_t lineInd    = 0;
    char* fileLinePtr = *fileBuffer;
    char* nxtLine     = NULL;
    LOG_DEBUG_VARS(assembler->numOfLines);
    for (size_t index = 0; index < assembler->numOfLines; ++index, fileLinePtr = nxtLine) {
        size_t lineOfCodeLen = strlen(fileLinePtr);
        LOG_DEBUG_VARS(index, lineInd, fileLinePtr);
        nxtLine = strchr(fileLinePtr, '\0') + 1;
        if (lineOfCodeLen == 0)
            continue;

        assembler->lines[lineInd] = fileLinePtr;
        IF_ERR_RETURN(prepareString(&assembler->lines[lineInd], COMMENT_CHAR_BEGIN, isDelim));
        lineOfCodeLen = strlen(assembler->lines[lineInd]);
        if (lineOfCodeLen != 0 && assembler->lines[lineInd] != NULL)
            ++lineInd;
    }
    assembler->numOfLines = lineInd;
    LOG_DEBUG_VARS(lineInd);

    fclose(source); // source file is no longer useful
    for (size_t i = 0; i < assembler->numOfLines; ++i) {
        LOG_DEBUG_VARS(i, assembler->lines[i]);
    }

    return ASSEMBLER_STATUS_OK;
}

AssemblerErrors saveProgramCodeToDestFile(const Assembler* assembler) {
    IF_ARG_NULL_RETURN(assembler);

    printf("saving program code to binary dest file\n");
    printAllLabels();

    TABLE_OF_LABELS_ERR_CHECK(checkAllLabelsAreDeclared());

    FILE* destFile = fopen(assembler->destFileName, "wb");
    IF_NOT_COND_RETURN(destFile != NULL,
                       ASSEMBLER_ERROR_COULDNT_OPEN_FILE);

    fwrite(assembler->programCode, assembler->numOfBytesInDest, sizeof(uint8_t), destFile);
    fclose(destFile);

    return ASSEMBLER_STATUS_OK;
}

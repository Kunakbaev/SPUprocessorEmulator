#ifndef ASSEBMLY_HEADER
#define ASSEBMLY_HEADER

#include "assemblerErrorsHandler.hpp"

struct Assembler {
    size_t instructionPointer = 0;
    uint8_t* programCode;
    size_t numOfLines;
    char** lines;
    const char* sourceFileName;
    const char* destFileName;
};

AssemblerErrors constructAssembler(Assembler* assembler,
                                   const char* sourceFileName,
                                   const char* destFileName);
AssemblerErrors compileProgram(Assembler* assembler);
AssemblerErrors destructAssembler(Assembler* assembler);

#endif

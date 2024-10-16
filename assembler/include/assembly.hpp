#ifndef ASSEBMLY_HEADER
#define ASSEBMLY_HEADER

#include "assemblerErrorsHandler.hpp"

AssemblerErrors constructAssembler();
AssemblerErrors compileProgram(const char* sourceFileName, const char* destFileName);
AssemblerErrors destructAssembler();

#endif

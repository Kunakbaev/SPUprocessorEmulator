#ifndef ASSEBMLY_HEADER
#define ASSEBMLY_HEADER

#include "assemblerErrorsHandler.hpp"

AssemblerErrors compileProgram(const char* sourceFileName, const char* destFileName);
AssemblerErrors destructAssembler();

#endif

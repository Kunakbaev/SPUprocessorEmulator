#ifndef ASSEBMLY_HEADER
#define ASSEBMLY_HEADER

#include "assemblerErrorsHandler.hpp"

Errors compileProgram(const char* sourceFileName, const char* destFileName);
Errors destructAssembler();

#endif

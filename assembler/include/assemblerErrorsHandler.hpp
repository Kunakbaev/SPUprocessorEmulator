#ifndef ASSEMBLER_INCLUDE_ASSEMBLER_ERRORS_HANDLER_HPP
#define ASSEMBLER_INCLUDE_ASSEMBLER_ERRORS_HANDLER_HPP

#include "../../common/include/commands.hpp"

// ASK: how to rewrite this enum, so errors are distributed among modules
enum AssemblerErrors {
    //  --------------------------      GENERAL ERRORS          -----------------------------
    ASSEMBLER_STATUS_OK                                   = 0,                  // no error, everything is valid
    ASSEMBLER_ERROR_INVALID_ARGUMENT                      = 1,                  // usually when argument is set to NULL
    ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR               = 2,                  // usually when memory allocation by calloc fails
    ASSEMBLER_ERROR_MEMORY_REALLOCATION_ERROR             = 3,                  // couldn't reallocate memory
    ASSEMBLER_ERROR_COULDNT_OPEN_FILE                     = 4,
};

const char* getErrorMessage(AssemblerErrors error);

#endif

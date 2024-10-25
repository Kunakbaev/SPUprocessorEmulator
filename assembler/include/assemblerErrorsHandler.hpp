#ifndef ASSEMBLER_INCLUDE_ASSEMBLER_ERRORS_HANDLER_HPP
#define ASSEMBLER_INCLUDE_ASSEMBLER_ERRORS_HANDLER_HPP

#include "../../common/include/errorsHandlerDefines.hpp"

enum AssemblerErrors {
    //  --------------------------      GENERAL ERRORS          -----------------------------
    ASSEMBLER_STATUS_OK                                   = 0,                  // no error, everything is valid
    ASSEMBLER_ERROR_INVALID_ARGUMENT                      = 1,                  // usually when argument is set to NULL
    ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR               = 2,                  // usually when memory allocation by calloc fails
    ASSEMBLER_ERROR_MEMORY_REALLOCATION_ERROR             = 3,                  // couldn't reallocate memory
    ASSEMBLER_ERROR_COULDNT_OPEN_FILE                     = 4,
    ASSEMBLER_ERROR_COMMANDS_ERROR                        = 5,
    ASSEMBLER_ERROR_FILE_STAT_ERROR                       = 6,
    ASSEMBLER_ERROR_TABLE_OF_LABELS_ERROR                 = 7,
    ASSEMBLER_ERROR_STRTOLD_ERROR                         = 8,
};

const char* getAssemblerErrorMessage(AssemblerErrors error);

#endif

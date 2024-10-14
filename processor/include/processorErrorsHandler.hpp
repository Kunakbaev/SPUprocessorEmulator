#ifndef PROCESSOR_INCLUDE_PROCESSOR_ERRORS_HANDLER_HPP
#define PROCESSOR_INCLUDE_PROCESSOR_ERRORS_HANDLER_HPP

#include "../../common/include/errorsHandlerDefines.hpp"

#define IF_PROCESSOR_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, PROCESSOR_ERROR_INVALID_ARGUMENT)

enum ProcessorErrors {
    PROCESSOR_STATUS_OK                                   = 0,                  // no error, everything is valid
    PROCESSOR_ERROR_INVALID_ARGUMENT                      = 1,                  // usually when argument is set to NULL
    PROCESSOR_ERROR_MEMORY_ALLOCATION_ERROR               = 2,                  // usually when memory allocation by calloc fails
    PROCESSOR_ERROR_MEMORY_REALLOCATION_ERROR             = 3,                  // couldn't reallocate memory
    PROCESSOR_ERROR_COULDNT_OPEN_FILE                     = 4,
    PROCESSOR_ERROR_STACK_ERROR                           = 5,                  // error occured in stack
    PROCESSOR_ERROR_BAD_INS_POINTER                       = 6,
    PROCESSOR_ERROR_COMMANDS_ERROR                        = 7,
};

const char* getErrorMessage(ProcessorErrors error);

#endif

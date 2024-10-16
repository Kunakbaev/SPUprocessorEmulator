#ifndef COMMON_COMMANDS_INCLUDE_COMMANDS_ERRORS_HANDLER_HPP
#define COMMON_COMMANDS_INCLUDE_COMMANDS_ERRORS_HANDLER_HPP

enum CommandErrors {
    COMMANDS_STATUS_OK                                   = 0,                  // no error, everything is valid
    COMMANDS_ERROR_INVALID_ARGUMENT                      = 1,                  // usually when argument is set to NULL
    COMMANDS_ERROR_MEMORY_ALLOCATION_ERROR               = 2,                  // usually when memory allocation by calloc fails
    COMMANDS_ERROR_MEMORY_REALLOCATION_ERROR             = 3,                  // couldn't reallocate memory
    COMMANDS_ERROR_COULDNT_OPEN_FILE                     = 4,
    // COMMANDS_ERROR_BAD_INDEX                             = 5,                  // command index index is bigger or equal (0 indexation) than total number of comands
    COMMANDS_ERROR_COMMAND_NOT_FOUND                     = 5,
    COMMANDS_ERROR_STACK_ERROR                           = 6,                  // error occured in stack sublib
    COMMANDS_ERROR_BAD_INSTR_PTR                         = 7,                  // instruction pointer is invalid (possibly not enough values in processor->program_code)
};

const char* getCommandsErrorMessage(CommandErrors error);

#endif

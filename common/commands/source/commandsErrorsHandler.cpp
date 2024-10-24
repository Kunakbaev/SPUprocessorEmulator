#include "../include/commandsErrorsHandler.hpp"

const char* getCommandsErrorMessage(CommandErrors error) {
    switch (error) {
        //  -------------------   GENERAL ERRORS    ---------------------------
        case COMMANDS_STATUS_OK:
            return "No erros in processor, everything is valid.\n";
        case COMMANDS_ERROR_INVALID_ARGUMENT:
            return "Commands error: invalid argument (possibly set to NULL).\n";
        case COMMANDS_ERROR_MEMORY_ALLOCATION_ERROR:
            return "Commands error: couldn't allocate memory.\n";
        case COMMANDS_ERROR_MEMORY_REALLOCATION_ERROR:
            return "Commands error: couldn't reallocate memory.\n";
        case COMMANDS_ERROR_COULDNT_OPEN_FILE:
            return "Commands error: couldn't open file.\n";
        case COMMANDS_ERROR_COMMAND_NOT_FOUND:
            return "Commands error: searched command not found.\n";
        case COMMANDS_ERROR_STACK_ERROR:
            return "Commands error: error occured in stack sublib.\n";
        case COMMANDS_ERROR_BAD_INSTR_PTR:
            return "Commands error: instruction pointer is invalid (possibly not enough values in processor->program_code).\n";
        case COMMANDS_ERROR_BAD_COMMAND_INDEX_FORMAT:
            return "Commands error: error, check that command index - 1 (one indexation to reduce errors) is equal to array index failed.\n";

        default:
            return "Unknown error.\n";
    }
}

#include "../include/errorsHandler.hpp"

const char* getErrorMessage(Errors error) {
    switch (error) {
        //  -------------------   GENERAL ERRORS    ---------------------------
        case STATUS_OK:
            return "No erros, everything is valid.\n";
        case ERROR_INVALID_ARGUMENT:
            return "Error: invalid argument (possibly set to NULL).\n";
        case ERROR_MEMORY_ALLOCATION_ERROR:
            return "Error: couldn't allocate memory.\n";
        case ERROR_MEMORY_REALLOCATION_ERROR:
            return "Error: couldn't reallocate memory.\n";
        case ERROR_COULDNT_OPEN_FILE:
            return "Error: couldn't open file.\n";

        default:
            return "Unknown error.\n";
    }
}

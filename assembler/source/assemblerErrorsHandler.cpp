#include "../include/assemblerErrorsHandler.hpp"

const char* getAssemblerErrorMessage(AssemblerErrors error) {
    switch (error) {
        //  -------------------   GENERAL ERRORS    ---------------------------
        case ASSEMBLER_STATUS_OK:
            return "No erros in assembler, everything is valid.\n";
        case ASSEMBLER_ERROR_INVALID_ARGUMENT:
            return "Assembler error: invalid argument (possibly set to NULL).\n";
        case ASSEMBLER_ERROR_MEMORY_ALLOCATION_ERROR:
            return "Assembler error: couldn't allocate memory.\n";
        case ASSEMBLER_ERROR_MEMORY_REALLOCATION_ERROR:
            return "Assembler error: couldn't reallocate memory.\n";
        case ASSEMBLER_ERROR_COULDNT_OPEN_FILE:
            return "Assembler error: couldn't open file.\n";
        case ASSEMBLER_ERROR_COMMANDS_ERROR:
            return "Assembler error: error occured in commands sublib.\n";
        case ASSEMBLER_ERROR_FILE_STAT_ERROR:
            return "Assembler error: error couldn't get file size.\n";
        case ASSEMBLER_ERROR_TABLE_OF_LABELS_ERROR:
            return "Assembler error: error occured in table of labels sublib.\n";
        case ASSEMBLER_ERROR_STRTOLD_ERROR:
            return "Assembler error: error occured during parse of number from text file.\n";
        case ASSEMBLER_ERROR_BAD_FSAT_STATUS:
            return "Assembler error: error couln't get file size, fstat bad status.\n";
        case ASSEBMLER_ERROR_FILE_IS_TOO_BIG:
            return "Assembler error: file is too big.\n";
        case ASSEMBLER_ERROR_BAD_ARGS_FOR_COMMAND:
            return "Assembler error: bad set of arguments provided to the command.\n";

        default:
            return "Unknown error.\n";
    }
}

#include "../include/processorErrorsHandler.hpp"

const char* getProcessorErrorMessage(ProcessorErrors error) {
    switch (error) {
        //  -------------------   GENERAL ERRORS    ---------------------------
        case PROCESSOR_STATUS_OK:
            return "No erros in processor, everything is valid.\n";
        case PROCESSOR_ERROR_INVALID_ARGUMENT:
            return "Processor error: invalid argument (possibly set to NULL).\n";
        case PROCESSOR_ERROR_MEMORY_ALLOCATION_ERROR:
            return "Processor error: couldn't allocate memory.\n";
        case PROCESSOR_ERROR_MEMORY_REALLOCATION_ERROR:
            return "Processor error: couldn't reallocate memory.\n";
        case PROCESSOR_ERROR_COULDNT_OPEN_FILE:
            return "Processor error: couldn't open file.\n";
        case PROCESSOR_ERROR_RAM_ERROR:
            return "Processor error: error occured in ram sublib.\n";
        case PROCESSOR_ERROR_BAD_FSAT_STATUS:
            return "Processor error: error couldn't get file size.\n";
        case PROCESSOR_ERROR_STACK_ERROR:
            return "Processor error: error occured in stack sublib.\n";
        case PROCESSOR_ERROR_COMMAND_ARRAY_IS_NOT_EQ_TO_COMMON:
            return "Processor error: array of commands in processor is not equal to array with same commands in common/commands folder (probably just wrong order of commands).\n";
        case PROCESSOR_ERROR_BAD_INS_POINTER:
            return "Processor error: bad instruction pointer.\n";

        default:
            return "Unknown error.\n";
    }
}

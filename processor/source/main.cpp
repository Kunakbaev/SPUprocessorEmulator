#include <iostream>

#include "../include/processor.hpp"
//#include "../../external/LoggerLib/include/logLib.hpp"
#include "logLib.hpp"
#include "../../external/StackStruct/include/stackLib.hpp"

#define IF_MAIN_ERR_RETURN(error) \
    do {                                                            \
        if (error != PROCESSOR_STATUS_OK) {                         \
            LOG_ERROR(getProcessorErrorMessage(error));             \
            assert(error != PROCESSOR_STATUS_OK);                   \
            return error;                                           \
        }                                                           \
    } while(0)

int main() {
    setLoggingLevel(DEBUG);
    LOG_DEBUG("Compiling program (transfer between 'compiler' code and 'working program'!\n");

    Processor processor = {};
    ProcessorErrors error = ProcessorConstructor(&processor);

    // ASK: is this relative path from place where make file is placed
    error = readProgramBinary(&processor, "../programBinCode.txt");
    IF_MAIN_ERR_RETURN(error);

    error = runProgramBinary(&processor);
    IF_MAIN_ERR_RETURN(error);

    return 0;
}

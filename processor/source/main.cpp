#include <iostream>

#include "../include/processor.hpp"
//#include "../../external/LoggerLib/include/logLib.hpp"
#include "logLib.hpp"
#include "../../external/StackStruct/include/stackLib.hpp"

int main() {
    setLoggingLevel(DEBUG);
    printf("bruh\n");
    LOG_DEBUG("Compiling program (transfer between 'compiler' code and 'working program'!\n");

    Processor processor = {};
    ProcessorErrors error = ProcessorConstructor(&processor);

    readProgramBinary(&processor, "programCodeBin.txt");
}

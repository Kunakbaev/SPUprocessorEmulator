#include <iostream>

//#include "../include/processor.hpp"
//#include "../../external/LoggerLib/include/logLib.hpp"
#include "logLib.hpp"
#include "stackLib.hpp"

int main() {
    setLoggingLevel(DEBUG);
    printf("bruh\n");
    LOG_DEBUG("Compiling program (transfer between 'compiler' code and 'working program'!\n");

    Stack stack = {};
    constructStack(&stack, 0, 4);
    int x = 10;
    pushElementToStack(&stack, &x);

    // Processor processor = {};
    // ProcessorErrors error = ProcessorConstructor(&processor);
}

#include <iostream>

#include "../include/processor.hpp"

int main() {
    setLoggingLevel(DEBUG);
    LOG_DEBUG("Compiling program (transfer between 'assebmler' code and 'binary'!\n");

    Processor processor = {};
    ProcessorErrors error = ProcessorConstructor(&processor);
    IF_ERR_RETURN(error);
}

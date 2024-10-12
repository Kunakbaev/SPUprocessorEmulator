#include <iostream>

#include "../include/processor.hpp"

int main() {
    setLoggingLevel(DEBUG);
    LOG_DEBUG("Compiling program (transfer between 'assebmler' code and 'binary'!\n");

    compileProgram("program.asm", "program_code.txt");
}

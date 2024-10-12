#include <iostream>

#include "../StackStruct/LoggerLib/include/logLib.hpp"
#include "../include/assembly.hpp"

int main() {
    setLoggingLevel(DEBUG);
    LOG_DEBUG("Compiling program (transfer between 'assebmler' code and 'binary'!\n");

    compileProgram("program.asm", "program_code.txt");
}

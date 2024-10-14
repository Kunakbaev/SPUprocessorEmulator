#include <iostream>

// #include "../StackStruct/LoggerLib/include/logLib.hpp"
// #include "../include/assembly.hpp"
// #include "../../external/LoggerLib/loglib.so"
// #include "../../external/LoggerLib/include/logLib.hpp"
#include "logLib.hpp"

/*

REMINDER:
if make file changes just run 'bear -- make run' and than change compile_commands.json file in .vscode

*/

int main() {
    setLoggingLevel(DEBUG);
    printf("I am bruh from assembler main\n");
    LOG_DEBUG("Compiling program (transfer between 'assebmler' code and 'binary'!\n");
}

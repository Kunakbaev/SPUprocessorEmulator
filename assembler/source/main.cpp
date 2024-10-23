#include <iostream>

// #include "../StackStruct/LoggerLib/include/logLib.hpp"
#include "../include/assembly.hpp"
// #include "../../external/LoggerLib/loglib.so"
// #include "../../external/LoggerLib/include/logLib.hpp"
#include "logLib.hpp"
#include "../include/tableOfLabels.hpp"

/*

REMINDER:
if make file changes just run 'bear -- make run' and than change compile_commands.json file in .vscode

*/

#define IF_MAIN_ERR_RETURN(error)                                   \
    do {                                                            \
        if (error != ASSEMBLER_STATUS_OK) {                         \
            LOG_ERROR(getAssemblerErrorMessage(error));             \
            assert(error != ASSEMBLER_STATUS_OK);                   \
            return error;                                           \
        }                                                           \
    } while(0)

int main() {
    setLoggingLevel(DEBUG);
    //printf("I am bruh from assembler main\n");
    LOG_DEBUG("Compiling program (transfer between 'assebmler' code and 'binary'!\n");

//     TableOfLabelsErrors error = TABLE_OF_LABELS_ERROR_STATUS_OK;
//     error = constructTableOfLabels();
//
//     addLabelName("first");
//     addSecondName("second");
//
//
//     error = destructTableOfLabels();

//    return 0;

    Assembler assembler = {};
    AssemblerErrors error = ASSEMBLER_STATUS_OK;
    // ../simpleAsmPrograms/iterativeFact.asm
    // ../program.asm
    // ../simpleAsmPrograms/recursiveFact.asm
    error = constructAssembler(&assembler,
                               "../program.asm",
                               "../programBinCode.bin");
    IF_MAIN_ERR_RETURN(error);

    error = compileProgram(&assembler);
    IF_MAIN_ERR_RETURN(error);

    destructAssembler(&assembler);

    return 0;
}

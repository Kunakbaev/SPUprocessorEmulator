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

#define name(arg) printf("%s\n", #arg)

int main() {
    setLoggingLevel(DEBUG);

//     name("fdklas;");
//     printf("%zu\n", (uint64_t)&"name");
//     printf("%zu\n", (uint64_t)&"ame");
//     printf("%zu\n", (uint64_t)&"me");
//     printf("%zu\n", (uint64_t)&"e");
//     // ASK: why diff in addreses is always 10 4 3 2?
//     printf("%zu\n", (uint64_t)&"random long string");
//     exit(0);
//     //printf("I am bruh from assembler main\n");
//     LOG_DEBUG("Compiling program (transfer between 'assebmler' code and 'binary'!\n");
//
//     //uint64_t bruh = (uint64_t)((void*hash me);
//     // printf("%zu\n", 'fdaskf;asd');
//     // char ch = (char)"fdsakl;dfs";
//     // printf("%c", ch);
//
//     return 0;

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
                               "../../../CodeGenerator/dest.asm",
                               "../programBinCode.bin");
    IF_MAIN_ERR_RETURN(error);

    error = compileProgram(&assembler);
    IF_MAIN_ERR_RETURN(error);

    destructAssembler(&assembler);

    return 0;
}

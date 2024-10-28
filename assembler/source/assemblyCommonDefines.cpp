#ifndef ASSEMBLER_SOURCE_COMMON_DEFINES_CPP
#define ASSEMBLER_SOURCE_COMMON_DEFINES_CPP

#include <iostream>
#include <string.h>
#include <sys/stat.h>

#include "../include/assembly.hpp"
#include "../../common/commands/include/commands.hpp"
#include "../include/tableOfLabels.hpp"



#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, ASSEMBLER_ERROR_INVALID_ARGUMENT, getAssemblerErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getAssemblerErrorMessage, ASSEMBLER_STATUS_OK)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getAssemblerErrorMessage)

#define TABLE_OF_LABELS_ERR_CHECK(error)                                            \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getTableOfLabelsErrorMessage,             \
        TABLE_OF_LABELS_ERROR_STATUS_OK, ASSEMBLER_ERROR_TABLE_OF_LABELS_ERROR)

#define COMMANDS_ERR_CHECK(error)                                                   \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getCommandsErrorMessage,                  \
        COMMANDS_STATUS_OK, ASSEMBLER_ERROR_COMMANDS_ERROR)

typedef bool (isDelimFuncPtr)(const char);

#endif

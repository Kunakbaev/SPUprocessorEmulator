#ifndef COMMON_ERRORS_HANDLER_DEFINES_HPP
#define COMMON_ERRORS_HANDLER_DEFINES_HPP

#include "utils.hpp"

// log and return happens only if error realy occured (error != STATUS_OK)
#define COMMON_IF_ERR_RETURN(error, getErrorMessageFunc)                                        \
    do {                                                            \
        if (error != STATUS_OK) {                                   \
            LOG_ERROR(getErrorMessageFunc(error));                      \
            assert(error != STATUS_OK);                             \
            return error;                                           \
        }                                                           \
    } while(0)

#define COMMON_IF_NOT_COND_RETURN(condition, error, getErrorMessageFunc)                        \
    do {                                                            \
        bool tmpCondition = (condition);                            \
        if (!tmpCondition) {                                        \
            LOG_ERROR(getErrorMessageFunc(error));                      \
            assert(tmpCondition);                                   \
            return error;                                           \
        }                                                           \
    } while(0)

// ASK: should I create tmp variable for this case too?
#define COMMON_IF_ARG_NULL_RETURN(arg, error, getErrorMessageFunc)                       \
    do {                                                            \
        if (arg == NULL) {                                          \
            LOG_ERROR(getErrorMessageFunc(error));                      \
            assert(arg != NULL);                                    \
            return error;                                           \
        }                                                           \
    } while (0)

#endif

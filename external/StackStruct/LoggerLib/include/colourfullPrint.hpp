#ifndef _COLOURFULL_PRINT_HEADER_
#define _COLOURFULL_PRINT_HEADER_

#include <stdio.h>
#include "debugMacros.hpp"

// bold:

// char* const RED_COLOR[2] = {"\033[1;31m", "\033[0m"};


/// @brief handy Colors enum, user can only choose between them
enum Colors {
    RED_COLOR     = 0,
    GREEN_COLOR   = 1,
    YELLOW_COLOR  = 2,
    BLUE_COLOR    = 3,
    MAGENTA_COLOR = 4,
    CYAN_COLOR    = 5,
    WHITE_COLOR   = 6,
    BASE_COLOR    = WHITE_COLOR,
};

/**
    \brief returns string matching color
    \param[in] color Colors enum, that is transformed to string
    \return color in string representation
*/
const char* getColor(enum Colors color);


/**
    \brief changes text color
    Changes CURRENT_COLOR value to the new color
    \param[in] color string that represents color to which we switch
*/
void changeTextColor(enum Colors newColor);


/// @brief return current text color
enum Colors getCurrentColor();

/**
    \brief printf but with color equal to CURRENT_COLOR
    \param[in] ... printf args
*/

/// @brief prints in stderror but with red color
#define doStuff(stream, color, func, ...)                                   \
    do {                                                                    \
        fprintf(stream, "%s", getColor(color));                             \
        func(stream, __VA_ARGS__);                                          \
        fprintf(stream, "%s", getColor(BASE_COLOR));                        \
    } while(0)

#define colourfullDebugToStream(stream, ...)                                \
do {                                                                        \
    doStuff(stream, GREEN_COLOR, DBG_TO_STREAM, __VA_ARGS__);               \
} while (0)

/// @brief prints in stderror but with red color
#define colourfullPrintToStream(stream, ...)                                \
do {                                                                        \
    doStuff(stream, getCurrentColor(), fprintf, __VA_ARGS__);               \
} while (0)

#define colourfullPrint(...) colourfullPrintToStream(stdout, __VA_ARGS__)

/// @brief prints in stdout but with red color
#define printError(...) doStuff(stdout, RED_COLOR, fprintf, __VA_ARGS__)

#endif



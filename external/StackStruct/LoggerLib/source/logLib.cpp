#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>

#include "../include/colourfullPrint.hpp"
#include "../include/logLib.hpp"

const char* FILE_OPENING_ERROR = "Error: couldn't open file\n";

static char timeBuffer[30] = {};
static char tmpBuffer[256] = {};
static char buffer[256]    = {};

static enum Levels loggingLevel = INFO;
static FILE* logFile = NULL;

const char* getLogMessage(enum Levels level) {
    switch (level) {
        case DEBUG  :    return "DEBUG";
        case INFO   :    return "INFO";
        case WARNING:    return "WARNING";
        case ERROR  :    return "ERROR";
        default     :    return "INFO";
    }
}

enum Colors getTextColorForLevel(enum Levels level) {
    switch (level) {
        case DEBUG  :    return GREEN_COLOR;
        case INFO   :    return WHITE_COLOR;
        case WARNING:    return YELLOW_COLOR;
        case ERROR  :    return RED_COLOR;
        default     :    return WHITE_COLOR;
    }
}

void setLoggingLevel(enum Levels level) {
    loggingLevel = level;
}

enum Levels getLoggingLevel() {
    return loggingLevel;
}

const char* getCurrentTimeFormatted() {
    struct timeval currentTime = {};
    gettimeofday(&currentTime, NULL);

    long long mils = (long long)((long double)currentTime.tv_usec / 1000); // 1000 - mils in one second
    struct tm *tm_info = localtime(&currentTime.tv_sec);
    assert(tm_info != NULL);

    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tm_info);
    sprintf(timeBuffer + strlen(timeBuffer), ":%.3Ld", mils);
    return timeBuffer;
}

static size_t getTrimDx(const char* line, size_t maxLen) {
    assert(line != NULL);

    size_t len = strlen(line);
    return len > maxLen ? len - maxLen : 0;
}

const char* getLoggingMessage(enum Levels level, const char* fileName, const char* funcName, int line) {
    assert(fileName != NULL);
    assert(funcName != NULL);

    const char* currentTime = getCurrentTimeFormatted();
    const char* logMessage = getLogMessage(level);

    //2024-08-26 16:40:51:557 | source/main.cpp:main(38):
    sprintf(tmpBuffer, "%s | %s:%s:(%d)", currentTime,
        fileName + getTrimDx(fileName, 30),
        funcName + getTrimDx(funcName, 20), line
    );

    #pragma GCC diagnostic ignored "-Wformat-overflow="
    sprintf(buffer, "%-80s%-7s:    ", tmpBuffer, logMessage);
    #pragma GCC diagnostic ignored "-Wformat-overflow="
    return buffer;
}

void stateLogFile(const char* logFileName) {
    assert(logFileName != NULL);

    // we want to add to file, not to clear it every time we relaunch our app
    logFile = fopen(logFileName, "a");
    if (logFile == NULL) {
        printError("%s", FILE_OPENING_ERROR);
    }

    //fprintf(logFile, "------------------------------------\n");
    LOG_INFO("New logging session started\n");
}

FILE* getLogFile() {
    return logFile;
}

void destructLogger() {
    if (logFile != NULL)
        fclose(logFile);
}

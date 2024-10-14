#include <stdio.h>
#include <stdbool.h>

//#define NO_LOG

#include "../include/logLib.hpp"
#include "../include/debugMacros.hpp"

int main() {
    setLoggingLevel(DEBUG);
    //stateLogFile("logFile.txt");

    LOG_FUNC_STARTED();
    LOG_FUNC_FINISHED();

    int intVar = 10;
    size_t sizeTvar = 10;
    long double longDoubleVar = 3.141592;
    const char* constCharPtrVar = "i am const char ptr";
    FILE* file = getLogFile(); // for file it will output ?, unknown type
    LOG_DEBUG_VARS(intVar, sizeTvar, constCharPtrVar, longDoubleVar, file);

    LOG_DEBUG("i am log debug");
    LOG_INFO("i am log info");
    LOG_WARNING("i am log warn");
    LOG_ERROR("i am log error");

    destructLogger();
	return 0;
}

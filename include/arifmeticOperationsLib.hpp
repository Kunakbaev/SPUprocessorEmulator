#ifndef ARIFMETIC_OPERATIONS_LIB
#define ARIFMETIC_OPERATIONS_LIB

#include "../StackStruct/include/stackLib.hpp"
#include "errorsHandler.hpp"

typedef int ArifmCalDataType;

struct ArifmeticCalculator {
    Stack stack;
};

Errors ArifmeticCalculatorConstructor();
Errors ArifmeticCalculator
Errors ArifmeticCalculatorDestructor();

#endif

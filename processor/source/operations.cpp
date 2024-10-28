#include <math.h>

#include "logLib.hpp"
#include "../include/operations.hpp"

const char* ZERO_DIVISION_ERR_MESSAGE = "Error: zero division happened during operation.\n";

processor_data_type add2nums(processor_data_type a, processor_data_type b) {
    return a + b;
}

processor_data_type sub2nums(processor_data_type a, processor_data_type b) {
    return a - b;
}

processor_data_type mul2nums(processor_data_type a, processor_data_type b) {
    return a * b;
}

processor_data_type div2nums(processor_data_type a, processor_data_type b) {
    if (b == 0) {
        LOG_ERROR(ZERO_DIVISION_ERR_MESSAGE);
        return 0;
    }

    return a / b;
}

processor_data_type mod2nums(processor_data_type a, processor_data_type b) {
    if (b == 0) {
        LOG_ERROR(ZERO_DIVISION_ERR_MESSAGE);
        return 0;
    }

    return (int)a % (int)b;
}

processor_data_type sqrt1num(processor_data_type a) {
    return sqrt(a);
}

processor_data_type sinOperation(processor_data_type a) {
    return sinl(a);
}

processor_data_type cosOperation(processor_data_type a) {
    return cosl(a);
}

processor_data_type absOperation(processor_data_type a) {
    return fabsl(a);
}

bool jmpConditionEqual(processor_data_type a, processor_data_type b) {
    return a == b;
}

bool jmpConditionLess(processor_data_type a, processor_data_type b) {
    return a < b;
}

bool jmpConditionMore(processor_data_type a, processor_data_type b) {
    return a > b;
}



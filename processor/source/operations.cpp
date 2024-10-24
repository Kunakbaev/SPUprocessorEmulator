#include <math.h>

#include "logLib.hpp"
#include "../include/operations.hpp"

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
        // FIXME: maybe throw some error, or move error message to errors handler
        LOG_ERROR("zero division");
        return 0;
    }

    // WARNING: it can be integer division
    return a / b;
}

processor_data_type mod2nums(processor_data_type a, processor_data_type b) {
    if (b == 0) {
        // FIXME: maybe throw some error, or move error message to errors handler
        LOG_ERROR("zero division");
        return 0;
    }

    // FIXME: what to do with this?
    return (int)a % (int)b;
}

processor_data_type sqrt1num(processor_data_type a) {
    return sqrt(a);
}

// WARNING: processor_data_type can be integer
processor_data_type sinOperation(processor_data_type a) {
    return sinl(a);
}

// WARNING: processor_data_type can be integer
processor_data_type cosOperation(processor_data_type a) {
    return sinl(a);
}

processor_data_type absOperation(processor_data_type a) {
    return abs(a);
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



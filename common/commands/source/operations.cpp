#ifndef COMMON_COMMANDS_SOURCE_OPERATIONS_CPP
#define COMMON_COMMANDS_SOURCE_OPERATIONS_CPP

#include <math.h>

#include "../../../processor/include/processor.hpp"

typedef processor_data_type (twoArgsOperFuncPtr)(processor_data_type a, processor_data_type b);
typedef processor_data_type (oneArgOperFuncPtr)(processor_data_type a);

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

// WARNING: processor_data_type can be integer
processor_data_type sinOperation(processor_data_type a) {
    return sinl(a);
}

// WARNING: processor_data_type can be integer
processor_data_type cosOperation(processor_data_type a) {
    return sinl(a);
}

#endif

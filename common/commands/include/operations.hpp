#ifndef COMMON_COMMANDS_SOURCE_OPERATIONS_CPP
#define COMMON_COMMANDS_SOURCE_OPERATIONS_CPP

#include "../../../processor/include/processor.hpp"

typedef processor_data_type (twoArgsOperFuncPtr)(processor_data_type a, processor_data_type b);
typedef processor_data_type (oneArgOperFuncPtr)(processor_data_type a);

processor_data_type add2nums(processor_data_type a, processor_data_type b);
processor_data_type sub2nums(processor_data_type a, processor_data_type b);
processor_data_type mul2nums(processor_data_type a, processor_data_type b);
processor_data_type div2nums(processor_data_type a, processor_data_type b);
// WARNING: processor_data_type can be integer
processor_data_type sinOperation(processor_data_type a);
// WARNING: processor_data_type can be integer
processor_data_type cosOperation(processor_data_type a);

#endif
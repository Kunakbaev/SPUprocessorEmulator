#ifndef COMMON_COMMANDS_SOURCE_OPERATIONS_CPP
#define COMMON_COMMANDS_SOURCE_OPERATIONS_CPP

// #include "../../../processor/include/processor.hpp"
#include "../../common/include/processorSettings.hpp"

typedef processor_data_type (twoArgsOperFuncPtr)  (processor_data_type a, processor_data_type b);
typedef processor_data_type (oneArgOperFuncPtr)   (processor_data_type a);
typedef bool                (jumpConditionFuncPtr)(processor_data_type a, processor_data_type b);

// FIXME: не должно быть в common
processor_data_type add2nums(processor_data_type a, processor_data_type b);
processor_data_type sub2nums(processor_data_type a, processor_data_type b);
processor_data_type mul2nums(processor_data_type a, processor_data_type b);
processor_data_type div2nums(processor_data_type a, processor_data_type b);
// WARNING: processor_data_type can be integer
processor_data_type sinOperation(processor_data_type a);
// WARNING: processor_data_type can be integer
processor_data_type cosOperation(processor_data_type a);
processor_data_type absOperation(processor_data_type a);

bool jmpConditionEqual(processor_data_type a, processor_data_type b);
bool jmpConditionLess(processor_data_type a, processor_data_type b);
bool jmpConditionMore(processor_data_type a, processor_data_type b);

#endif

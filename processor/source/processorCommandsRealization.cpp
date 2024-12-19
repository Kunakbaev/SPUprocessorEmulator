#include "../include/processor.hpp"
#include "../include/operations.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, PROCESSOR_ERROR_INVALID_ARGUMENT, getProcessorErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getProcessorErrorMessage, PROCESSOR_STATUS_OK)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getProcessorErrorMessage)

#define STACK_LIB_ERR_CHECK(error)                                     \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getErrorMessage,             \
        STATUS_OK, PROCESSOR_ERROR_STACK_ERROR)

#define RAM_MODULE_ERR_CHECK(error)                                    \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getRamErrorMessage,          \
        RAM_STATUS_OK, PROCESSOR_ERROR_RAM_ERROR)

// pops element from processor->$(stackVarName) and stores it to $(whereToStore) variable
#define POP(stackVarName, whereToStore) \
    do { \
        STACK_LIB_ERR_CHECK(popElementToStack(&processor->stackVarName, whereToStore));   \
    } while (0)

#define PUSH(stackVarName, element) \
    do { \
        STACK_LIB_ERR_CHECK(pushElementToStack(&processor->stackVarName, element));   \
    } while (0)

// pops from stack of vars  to a given variable, can return error
#define POP_VAR(whereToStore)  POP (stackOfVars,  whereToStore)

// pops from stack of calls to a given variable, can return error
#define POP_CALL(whereToStore) POP (stackOfCalls, whereToStore)

// pushes to stack of vars given variable, can return error
#define PUSH_VAR(element)      PUSH(stackOfVars,  element)

// pushes to stack of calls given variable, can return error
#define PUSH_CALL(element)     PUSH(stackOfCalls, element)

// WARNING:
// be carefull, simple text is pasted instead of define. Creates var argPtr in which stores pointer to argument for command (either number, register or memory cell)
#define GET_ARG_PTR()                                           \
    processor_data_type* argPtr = NULL;                         \
    IF_ERR_RETURN(getArgPtrForCommand(processor, &argPtr));     \

static ProcessorErrors popPrintLastVarFromVarStackAndReturnIt(Processor* processor,
                                                              processor_data_type* number) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(number);

    *number = 0;
    POP_VAR(number);

    //const char* format = sizeof(processor_data_type) == 4 ? "%df"
    if (sizeof(processor_data_type) == 4) {
        printf("last in stack : %d\n", *number);
    } else {
        printf("last in stack : %Lg\n", *number);
    }

    LOG_DEBUG_VARS("last in stack : ", *number);

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors lookLastVarInVarStackFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    processor_data_type num = 0;
    IF_ERR_RETURN(popPrintLastVarFromVarStackAndReturnIt(processor, &num));
    PUSH_VAR(&num);

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors popAndPrintLastVarInStackFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    processor_data_type num = 0;
    IF_ERR_RETURN(popPrintLastVarFromVarStackAndReturnIt(processor, &num));

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors executeOperationWith2Args(Processor* processor,
                                          twoArgsOperFuncPtr operation) {
    IF_ARG_NULL_RETURN(processor->programCode);
    IF_ARG_NULL_RETURN(operation);

    processor_data_type number_1 = 0;
    processor_data_type number_2 = 0;
    POP_VAR(&number_2);
    POP_VAR(&number_1);

    processor_data_type operationResult = (*operation)(number_1, number_2);
    LOG_DEBUG_VARS(number_1, number_2, operationResult);
    PUSH_VAR(&operationResult);

    return PROCESSOR_STATUS_OK;
}

#define PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(funcName, operationFuncPtr)              \
    ProcessorErrors funcName(Processor* processor) {                                \
        IF_ERR_RETURN(executeOperationWith2Args(processor, operationFuncPtr));      \
        return PROCESSOR_STATUS_OK;                                                 \
    }                                                                               \

PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(add2numsFunc, add2nums);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(sub2numsFunc, sub2nums);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(mul2numsFunc, mul2nums);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(div2numsFunc, div2nums);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(mod2numsFunc, mod2nums);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(lessCmpFunc,  lessCmp);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(lessOrEqCmpFunc,  lessOrEqCmp);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(greaterCmpFunc,  greaterCmp);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(greaterOrEqCmpFunc,  greaterOrEqCmp);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(isEqCmpFunc,  isEqCmp);

ProcessorErrors executeOperationWith1Arg(Processor* processor,
                                         oneArgOperFuncPtr operation) {
    IF_ARG_NULL_RETURN(processor->programCode);
    IF_ARG_NULL_RETURN(operation);

    processor_data_type argument = 0;
    POP_VAR(&argument);

    processor_data_type operationResult = (*operation)(argument);
    LOG_DEBUG_VARS(argument, operationResult);
    PUSH_VAR(&operationResult);

    return PROCESSOR_STATUS_OK;
}

#define PROCESSOR_COMMAND_FUNC_WITH_1_ARG(funcName, operationFuncPtr)           \
    ProcessorErrors funcName(Processor* processor) {                            \
        IF_ERR_RETURN(executeOperationWith1Arg(processor, operationFuncPtr));   \
        return PROCESSOR_STATUS_OK;                                             \
    }

PROCESSOR_COMMAND_FUNC_WITH_1_ARG(sqrt1numFunc, sqrt1num);

static ProcessorErrors checkNumArg(uint8_t* code, size_t* instructionPointer, processor_data_type** arg) {
    IF_ARG_NULL_RETURN(code);
    IF_ARG_NULL_RETURN(instructionPointer);
    IF_ARG_NULL_RETURN(arg);

    *arg = (processor_data_type*)(code + (*instructionPointer));
    (*instructionPointer) += sizeof(processor_data_type);

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors checkRegArg(int mask, Processor* processor,
                                   processor_data_type** arg, processor_data_type* result) {
    IF_ARG_NULL_RETURN(arg);
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(result);

    size_t* instructionPointer = &processor->instructionPointer;
    processor_data_type* num = &processor->registers[processor->programCode[*instructionPointer]];
    if (mask & HAS_NUM_ARG) {
        *result = **arg + *num;
        *arg    = result;
    } else {
        *arg    = num;
    }
    ++(*instructionPointer);

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors checkRamArg(Processor* processor, processor_data_type** arg) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(arg);

    // WARNING: bad cast, from processor data type to size_t
    size_t memoryIndex = **arg;
    LOG_DEBUG_VARS(memoryIndex, processor->ram.memory[memoryIndex]);
    RAM_MODULE_ERR_CHECK(getRamVarByIndex(&processor->ram, memoryIndex, arg));

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors getArgPtrForCommand(Processor* processor, processor_data_type** arg) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(processor->programCode);
    IF_ARG_NULL_RETURN(processor->registers);
    IF_ARG_NULL_RETURN(arg);
    //IF_ARG_NULL_RETURN(*arg);

    uint8_t* code              = processor->programCode;
    size_t* instructionPointer = &processor->instructionPointer;

    static processor_data_type result = 0;
    // instruction pointer points to a mask, cause we already looked at what type of command we are dealing with
    int mask = code[*instructionPointer];
    ++(*instructionPointer);
    LOG_DEBUG_VARS(mask);
    // WARNING:
    *arg = NULL;
    if (mask & HAS_NUM_ARG) IF_ERR_RETURN(checkNumArg(code, instructionPointer, arg));
    if (mask & HAS_REG_ARG) IF_ERR_RETURN(checkRegArg(mask, processor,          arg, &result));
    if (mask & HAS_RAM_ARG) IF_ERR_RETURN(checkRamArg(      processor,          arg));

    return PROCESSOR_STATUS_OK;
}

// executes push command if isPush flag is set to true, otherwise makes pop
ProcessorErrors pushOrPopProcessorStackCommonFunc(Processor* processor, bool isPush) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(processor->programCode);

    uint8_t* code              = processor->programCode;
    size_t* instructionPointer = &processor->instructionPointer;
    IF_ARG_NULL_RETURN(instructionPointer);

    IF_NOT_COND_RETURN(processor->instructionPointer < processor->numberOfInstructions,
                       PROCESSOR_ERROR_BAD_INS_POINTER);

    GET_ARG_PTR();

    if (isPush)
        PUSH_VAR(argPtr);
    else
        POP_VAR(argPtr);

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors pushToProcessorStackFunc(Processor* processor) {
    IF_ERR_RETURN(pushOrPopProcessorStackCommonFunc(processor, true));
    return PROCESSOR_STATUS_OK;
}

ProcessorErrors popFromProcessorStackFunc(Processor* processor) {
    IF_ERR_RETURN(pushOrPopProcessorStackCommonFunc(processor, false));
    return PROCESSOR_STATUS_OK;
}

// ends (finishes) 'program'
ProcessorErrors haltCommandFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    //LOG_DEBUG("HALT command");
    LOG_FUNC_STARTED();
    // for loop will end, because condition of loop is not satisfied any longer
    processor->instructionPointer = processor->numberOfInstructions;

    return PROCESSOR_STATUS_OK;
}

static size_t getJumpInstructionPointer(Processor* processor) {
    assert(processor != NULL);
    size_t res = *((int*)(processor->programCode + processor->instructionPointer));
    LOG_DEBUG_VARS(res, processor->programCode[processor->instructionPointer],
        processor->programCode[processor->instructionPointer + 1],  processor->programCode[processor->instructionPointer + 2]);
    return res;
}

ProcessorErrors meowFunc(Processor* processor) {
    processor_data_type number = 0;

    GET_ARG_PTR();
    if (argPtr == NULL) // if no arguments were provided we try to get it from stack of vars
        POP_VAR(&number);
    else
        number = *argPtr;

    for (size_t i = 0; i < number; ++i) {
        printf("meow\n");
    }
    printf("end\n");

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors generalJmpCommandFunc(Processor* processor, jumpConditionFuncPtr comparator) {
    IF_ARG_NULL_RETURN(processor->programCode);

    if (comparator == NULL) { // that's jmp command, without any conditions
        // WARNING: endless loop can happen
        processor->instructionPointer = getJumpInstructionPointer(processor);
        return PROCESSOR_STATUS_OK;
    }

    processor_data_type number_1 = 0;
    processor_data_type number_2 = 0;
    LOG_DEBUG_VARS(processor->stackOfVars.numberOfElements);
    POP_VAR(&number_2);
    POP_VAR(&number_1);

    processor_data_type operationResult = (*comparator)(number_1, number_2);
    LOG_DEBUG_VARS(number_1, number_2, operationResult);

    if (operationResult) { // jump happens
        processor->instructionPointer = getJumpInstructionPointer(processor);
        LOG_DEBUG_VARS(processor->instructionPointer);
    } else { // no jump
        processor->instructionPointer += sizeof(int);
    }

    return PROCESSOR_STATUS_OK;
}

#define PROCESSOR_GENERAL_JUMP_COMMAND(funcName, operationFuncPtr)              \
    ProcessorErrors funcName(Processor* processor) {                            \
        LOG_DEBUG_VARS(funcName);                                               \
        IF_ERR_RETURN(generalJmpCommandFunc(processor, operationFuncPtr));      \
        return PROCESSOR_STATUS_OK;                                             \
    }                                                                           \

PROCESSOR_GENERAL_JUMP_COMMAND(procCommandJumpIfEqual, jmpConditionEqual);
PROCESSOR_GENERAL_JUMP_COMMAND(procCommandJumpIfBelow, jmpConditionLess);
PROCESSOR_GENERAL_JUMP_COMMAND(procCommandJumpIfMore,  jmpConditionMore);
PROCESSOR_GENERAL_JUMP_COMMAND(procCommandJumpAnyway,  NULL);

ProcessorErrors procCommandCallFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    size_t tmp = processor->instructionPointer + sizeof(int); // tmp var, just in case
    PUSH_CALL(&tmp);
    LOG_DEBUG_VARS("call function", processor->instructionPointer);
    IF_ERR_RETURN(procCommandJumpAnyway(processor));

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors procCommandReturnFromFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    size_t tmp = 0; // tmp var, just in case
    POP_CALL(&tmp);
    LOG_DEBUG_VARS("return", processor->instructionPointer, tmp);
    processor->instructionPointer = tmp;

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors procCommandDrawFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    RAM_MODULE_ERR_CHECK(drawRamMemory(&processor->ram));

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors procCommandInFromTerminal(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    processor_data_type number = 0; // FIXME: doesn't work with double
    printf("print your number: ");

    if (sizeof(processor_data_type) == 4) { // FIXME: bruh, what the hell?
        scanf("%d", &number);
    } else {
        scanf("%Lg", &number);
    }

    PUSH_VAR(&number);

    return PROCESSOR_STATUS_OK;
}

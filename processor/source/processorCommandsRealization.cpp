#include "../include/processor.hpp"
#include "../include/operations.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, PROCESSOR_ERROR_INVALID_ARGUMENT, getProcessorErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getProcessorErrorMessage, PROCESSOR_STATUS_OK)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getProcessorErrorMessage)

static ProcessorErrors popPrintLastVarFromVarStackAndReturnIt(Processor* processor,
                                                              processor_data_type* number) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(number);

    *number = 0;
    LOG_DEBUG_VARS(processor->stackOfVars.numberOfElements);
    Errors error = popElementToStack(&processor->stackOfVars, number);
    if (error != NULL) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    // FIXME: do something with different types
    printf("last in stack : %d\n", *number);
    LOG_DEBUG_VARS("last in stack : ", *number);

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors lookLastVarInVarStackFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    processor_data_type num = 0;
    IF_ERR_RETURN(popPrintLastVarFromVarStackAndReturnIt(processor, &num));

    Errors error = pushElementToStack(&processor->stackOfVars, &num);
    if (error != NULL) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

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
    // TODO: define for checking error from separate lib or function
    LOG_DEBUG_VARS(processor->stackOfVars.numberOfElements);
    Errors error = popElementToStack(&processor->stackOfVars, &number_2);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    error = popElementToStack(&processor->stackOfVars, &number_1);
    //LOG_DEBUG_VARS(processor->stackOfVars.numberOfElements);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }
    processor_data_type operationResult = (*operation)(number_1, number_2);
    LOG_DEBUG_VARS(number_1, number_2, operationResult);

    error = pushElementToStack(&processor->stackOfVars, &operationResult);
    //LOG_DEBUG_VARS(processor->stackOfVars.numberOfElements);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    return PROCESSOR_STATUS_OK;
}

#define PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(funcName, operationFuncPtr)              \
    ProcessorErrors funcName(Processor* processor) {                                \
        IF_ERR_RETURN(executeOperationWith2Args(processor, operationFuncPtr));      \
    }                                                                               \

PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(add2numsFunc, add2nums);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(sub2numsFunc, sub2nums);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(mul2numsFunc, mul2nums);
PROCESSOR_COMMAND_FUNC_WITH_2_ARGS(div2numsFunc, div2nums);

ProcessorErrors executeOperationWith1Arg(Processor* processor,
                                         oneArgOperFuncPtr operation) {
    IF_ARG_NULL_RETURN(processor->programCode);
    IF_ARG_NULL_RETURN(operation);

    processor_data_type argument = 0;
    // TODO: define for checking error from separate lib or function
    LOG_DEBUG_VARS(processor->stackOfVars.numberOfElements);
    Errors error = popElementToStack(&processor->stackOfVars, &argument);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    processor_data_type operationResult = (*operation)(argument);
    LOG_DEBUG_VARS(argument, operationResult);

    error = pushElementToStack(&processor->stackOfVars, &operationResult);
    LOG_DEBUG_VARS(&processor->stackOfVars.numberOfElements);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    return PROCESSOR_STATUS_OK;
}

static ProcessorErrors getArgsFromCodeForPushOrPop(Processor* processor, processor_data_type** arg) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(processor->programCode);
    IF_ARG_NULL_RETURN(processor->registers);
    IF_ARG_NULL_RETURN(arg);
    IF_ARG_NULL_RETURN(*arg);

    uint8_t* code              = processor->programCode;
    size_t* instructionPointer = &processor->instructionPointer;

    static processor_data_type result = 0;
    // instruction pointer points to a mask, cause we already looked at what type of command we are dealing with
    int mask = code[*instructionPointer];
    ++(*instructionPointer);
    LOG_DEBUG_VARS(mask);
    if (mask & 1) {
        *arg = (processor_data_type*)(code + (*instructionPointer));
        LOG_DEBUG_VARS(code[*instructionPointer], *arg);
        (*instructionPointer) += sizeof(processor_data_type);
    }
    if (mask & 2) {
        // TODO: check that this works
        processor_data_type* num = &processor->registers[code[*instructionPointer]];
        LOG_DEBUG_VARS(code[*instructionPointer], processor->registers[code[*instructionPointer]]);
        if (mask & 1) {
            result = **arg + *num;
            LOG_DEBUG_VARS(result, **arg, *num);
            *arg    = &result;
        } else {
            *arg    = num;
        }
        ++(*instructionPointer);
    }
    if (mask & 4) {
        // WARNING: bad cast, from processor data type to size_t
        size_t memoryIndex = **arg;
        LOG_DEBUG_VARS(memoryIndex, processor->ram.memory[memoryIndex]);
        RamStructErrors error = getRamVarByIndex(&processor->ram, memoryIndex, arg);
        if (error != RAM_STATUS_OK) {
            LOG_DEBUG(getRamErrorMessage(error));
            return PROCESSOR_ERROR_RAM_ERROR;
        }
    }

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors pushToProcessorStackFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(processor->programCode);

    uint8_t* code              = processor->programCode;
    size_t* instructionPointer = &processor->instructionPointer;
    IF_ARG_NULL_RETURN(instructionPointer);

    IF_NOT_COND_RETURN(processor->instructionPointer < processor->numberOfInstructions,
                       PROCESSOR_ERROR_BAD_INS_POINTER);

    processor_data_type argNumber = 0;
    processor_data_type*   argPtr = &argNumber; // FIXME: cringe
    IF_ERR_RETURN(getArgsFromCodeForPushOrPop(processor, &argPtr));
    LOG_DEBUG_VARS(*argPtr);

    // Errors err = dumpStackLog(&processor->stackOfVars);
    // LOG_ERROR(getErrorMessage(err));
    // LOG_DEBUG("ok");

    Errors error = pushElementToStack(&processor->stackOfVars, argPtr);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    return PROCESSOR_STATUS_OK;
}

// FIXME: copy paste
ProcessorErrors popFromProcessorStackFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);
    IF_ARG_NULL_RETURN(processor->programCode);

    uint8_t* code              = processor->programCode;
    size_t* instructionPointer = &processor->instructionPointer;
    IF_ARG_NULL_RETURN(instructionPointer);

    IF_NOT_COND_RETURN(processor->instructionPointer < processor->numberOfInstructions,
                       PROCESSOR_ERROR_BAD_INS_POINTER);

    processor_data_type argNumber = 0;
    processor_data_type*   argPtr = &argNumber; // ASK: cringe?
    IF_ERR_RETURN(getArgsFromCodeForPushOrPop(processor, &argPtr));
    LOG_DEBUG_VARS(*argPtr);

    Errors error = popElementToStack(&processor->stackOfVars, &argNumber);
    LOG_DEBUG_VARS(argNumber);
    *argPtr = argNumber;

    LOG_DEBUG_VARS(processor->registers[3]);

    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    return PROCESSOR_STATUS_OK;
}

// ends (finishes) 'program'
ProcessorErrors haltCommandFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    LOG_DEBUG("halt");
    // for loop will end, because conditions is not satisfied any longer
    processor->instructionPointer = processor->numberOfInstructions;

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors generalJmpCommandFunc(Processor* processor, jumpConditionFuncPtr comparator) {
    IF_ARG_NULL_RETURN(processor->programCode);

    if (comparator == NULL) { // that's jmp command, without any conditions
        // WARNING: endless loop can happen
        processor->instructionPointer = processor->programCode[processor->instructionPointer];
        //++(processor->instructionPointer);
        return PROCESSOR_STATUS_OK;
    }

    processor_data_type number_1 = 0;
    processor_data_type number_2 = 0;
    // TODO: define for checking error from separate lib or function
    LOG_DEBUG_VARS(processor->stackOfVars.numberOfElements);
    Errors error = popElementToStack(&processor->stackOfVars, &number_2);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    error = popElementToStack(&processor->stackOfVars, &number_1);
    //LOG_DEBUG_VARS(processor->stackOfVars.numberOfElements);
    if (error != STATUS_OK) {
        LOG_ERROR(getErrorMessage(error));
        return PROCESSOR_ERROR_STACK_ERROR;
    }

    processor_data_type operationResult = (*comparator)(number_1, number_2);
    LOG_DEBUG_VARS(number_1, number_2, operationResult);

    if (operationResult) { // jump happens
        processor->instructionPointer = processor->programCode[processor->instructionPointer];
    } else { // no jump
        ++processor->instructionPointer;
    }

    return PROCESSOR_STATUS_OK;
}

#define PROCESSOR_GENERAL_JUMP_COMMAND(funcName, operationFuncPtr)              \
    ProcessorErrors funcName(Processor* processor) {                            \
        LOG_DEBUG_VARS(funcName);                                               \
        IF_ERR_RETURN(generalJmpCommandFunc(processor, operationFuncPtr));      \
    }                                                                           \

PROCESSOR_GENERAL_JUMP_COMMAND(procCommandJumpIfEqual, jmpConditionEqual);
PROCESSOR_GENERAL_JUMP_COMMAND(procCommandJumpIfBelow, jmpConditionLess);
PROCESSOR_GENERAL_JUMP_COMMAND(procCommandJumpIfMore,  jmpConditionMore);
PROCESSOR_GENERAL_JUMP_COMMAND(procCommandJumpAnyway,  NULL);

ProcessorErrors procCommandCallFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    int tmp = processor->instructionPointer + 1; // tmp var, just in case
    Errors error = pushElementToStack(&processor->stackOfCalls, &tmp);
    LOG_DEBUG_VARS("call function", processor->instructionPointer);
    IF_ERR_RETURN(procCommandJumpAnyway(processor));

    return PROCESSOR_STATUS_OK;
}

ProcessorErrors procCommandReturnFromFunc(Processor* processor) {
    IF_ARG_NULL_RETURN(processor);

    int tmp = 0; // tmp var, just in case
    Errors error = popElementToStack(&processor->stackOfCalls, &tmp);
    LOG_DEBUG_VARS("return", processor->instructionPointer, tmp);
    //IF_ERR_RETURN(procCommandJumpAnyway(processor));

    processor->instructionPointer = tmp; // ASK: is it copypaste, because I already have jumpAnyway function

    return PROCESSOR_STATUS_OK;
}

// FIXME: move to processor folder

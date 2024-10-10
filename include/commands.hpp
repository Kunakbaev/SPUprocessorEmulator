#ifndef COMMANDS
#define COMMANDS



struct CommandStruct {
    const char* commandName;

};

enum CommandsEnum {
    HALT_COMMAND        = 0,
    PUSH_COMMAND        = 1,
    POP_COMMAND         = 2,
    OUT_COMMAND         = 3,
    ADD_COMMAND         = 4,
    SUB_COMMAND         = 5,
    MUL_COMMAND         = 6,
    DIV_COMMAND         = 7,
    DUMP_COMMAND        = 9
};

CommandsEnum getCommandIndexByName(const char* command);
const char*  getCommandNameByIndex(CommandsEnum index);

#endif

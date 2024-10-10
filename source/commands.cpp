#include <inttypes.h>

#include "../include/commands.hpp"

uint64_t getCommandHash();

CommandsEnum getCommandIndexByName(const char* command) {

}

const char*  getCommandNameByIndex(CommandsEnum index) {
    switch (index) {
        case PUSH_COMMAND:          return "push";
        case ADD_COMMAND:           return "add";
        case SUB_COMMAND:           return "sub";
        case MUL_COMMAND:           return "mul";
        case DIV_COMMAND:           return "div";
        case HALT_COMMAND:          return "halt";
    }
}

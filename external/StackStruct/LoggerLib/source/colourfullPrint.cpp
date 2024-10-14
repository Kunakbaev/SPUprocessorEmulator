#include "../include/colourfullPrint.hpp"

static enum Colors CURRENT_COLOR = RED_COLOR;

void changeTextColor(enum Colors newColor) {
    CURRENT_COLOR = newColor;
}

enum Colors getCurrentColor() {
    return CURRENT_COLOR;
}

const char* getColor(enum Colors color) {
    switch (color) {
        case RED_COLOR:     return "\033[31m";
        case GREEN_COLOR:   return "\033[32m";
        case YELLOW_COLOR:  return "\033[33m";
        case BLUE_COLOR:    return "\033[34m";
        case MAGENTA_COLOR: return "\033[35m";
        case CYAN_COLOR:    return "\033[36m";
        case WHITE_COLOR:
        default:            return "\033[37m";
    };
}

#include <stdbool.h>
#include <stdlib.h>

#include "../include/debugMacros.hpp"

const char* varToFormat(bool) {
    return "%d";
}

const char* varToFormat(char){
    return "%c";
}

const char* varToFormat(char*) {
    return "%s";
}

const char* varToFormat(const char*) {
    return "%s";
}

const char* varToFormat(int){
    return "%d";
}

const char* varToFormat(long double){
    return "%Lg";
}

const char* varToFormat(double){
    return "%g";
}

const char* varToFormat(void*){
    return "%p";
}

const char* varToFormat(size_t) {
    return "%zu";
}

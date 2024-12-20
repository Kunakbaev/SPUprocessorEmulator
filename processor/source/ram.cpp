#include <time.h>

#include "../include/ram.hpp"
#include "../../common/include/errorsHandlerDefines.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, RAM_STATUS_OK, getRamErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getRamErrorMessage, RAM_STATUS_OK)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getRamErrorMessage)

const size_t      MATRIX_SIDE_SIZE      = 41;
const size_t      MAX_MEMORY_SIZE       = MATRIX_SIDE_SIZE * MATRIX_SIDE_SIZE;
const long double RAM_OPERATION_LATENCY = 0; // time to sleep in second
const char*       SLOW_RAM_MESSAGE      = "RAM is too slow";
const int         TILE_SIZE             = 25;
const int         WINDOW_SIZE           = TILE_SIZE * MATRIX_SIDE_SIZE;

// Тарань меня полностью
RamStructErrors pleaseGiveMeRAM(RamStruct* ram) {
    IF_ARG_NULL_RETURN(ram);

    *ram = {};
    sf::VideoMode mode(WINDOW_SIZE, WINDOW_SIZE);
    static sf::RenderWindow tmp(mode, "i am screen");
    ram->screen     = &tmp;
    ram->memorySize = MAX_MEMORY_SIZE;
    ram->memory     = (processor_data_type*)calloc(MAX_MEMORY_SIZE, sizeof(processor_data_type));
    IF_NOT_COND_RETURN(ram->memory != NULL,
                       RAM_ERROR_MEMORY_ALLOCATION_ERROR);

    return RAM_STATUS_OK;
}

RamStructErrors getRamVarByIndex(const RamStruct* ram, size_t index, processor_data_type** result) {
    IF_ARG_NULL_RETURN(ram);
    IF_ARG_NULL_RETURN(result);
    IF_ARG_NULL_RETURN(*result);
    IF_ARG_NULL_RETURN(ram->memory);
    IF_NOT_COND_RETURN(index < MAX_MEMORY_SIZE,
                       RAM_ERROR_BAD_VAR_INDEX);

//#ifndef NDEBUG
    LOG_DEBUG(SLOW_RAM_MESSAGE);
    sleep(RAM_OPERATION_LATENCY);
//#endif
    //*result = ram->memory[index];
    // WARNING:
    LOG_DEBUG_VARS(ram->memory[4], ram->memory[5], ram->memory[6]);
    LOG_DEBUG_VARS(index);
    *result = &ram->memory[index];

    return RAM_STATUS_OK;
}

RamStructErrors setRamVarByIndex(const RamStruct* ram, size_t index, processor_data_type value) {
    IF_ARG_NULL_RETURN(ram);
    IF_ARG_NULL_RETURN(ram->memory);
    IF_NOT_COND_RETURN(index < MAX_MEMORY_SIZE,
                       RAM_ERROR_BAD_VAR_INDEX);

//#ifndef NDEBUG
    LOG_DEBUG("RAM is too slow");
    sleep(RAM_OPERATION_LATENCY);
//#endif
    ram->memory[index] = value;

    return RAM_STATUS_OK;
}

static RamStructErrors drawRamMemorySimpleConsoleVersion(const RamStruct* ram) {
    IF_ARG_NULL_RETURN(ram);
    IF_ARG_NULL_RETURN(ram->memory);

    printf("----------------------------\n");
    printf("RAM: \n");
    // slow
    for (size_t varInd = 0, colInd = 1; varInd < ram->memorySize; ++varInd, ++colInd) {
        // TODO: sfml + graphicaly draw pixels
        // ASK: is type cast good?
        putchar((uint8_t)ram->memory[varInd] == 255 ? '@' : ' ');

        // //#ifndef NDEBUG
        //     sleep(RAM_OPERATION_LATENCY);
        // //#endif
        if (colInd * colInd >= ram->memorySize) {
            putchar('\n');
            colInd = 0;
        }
    }
    printf("\n");

    return RAM_STATUS_OK;
}

// RamStructErrors fillRectangleAtCoords

static RamStructErrors drawRamMemoryGraphicalVersion(const RamStruct* ram) {
    IF_ARG_NULL_RETURN(ram);
    IF_ARG_NULL_RETURN(ram->memory);

    if (!ram->screen->isOpen()) {
        return RAM_STATUS_OK; // is this error?
    }

    sf::Event event = {};
    while (ram->screen->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            ram->screen->close();
    }

    // very slow
    ram->screen->clear();
    for (size_t varInd = 0; varInd < ram->memorySize; ++varInd) {
        int x = varInd % MATRIX_SIDE_SIZE, y = varInd / MATRIX_SIDE_SIZE;
        x *= TILE_SIZE;
        y *= TILE_SIZE;

        uint8_t color = (rand() & 1) ? 255 : 0;
        color = (uint8_t)ram->memory[varInd];
        if (color == 0) continue; // we don't need to draw square if it's black

        sf::RectangleShape square(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        square.setFillColor(sf::Color(color, color, color, 255));
        square.setPosition((float)x, (float)y);
        ram->screen->draw(square);
        LOG_DEBUG("ok");
    }

    ram->screen->display();

    return RAM_STATUS_OK;
}


RamStructErrors drawRamMemory(const RamStruct* ram) {
    IF_ARG_NULL_RETURN(ram);
    IF_ARG_NULL_RETURN(ram->memory);

    // IF_ERR_RETURN(drawRamMemorySimpleConsoleVersion(ram));
    IF_ERR_RETURN(drawRamMemoryGraphicalVersion(ram));

    return RAM_STATUS_OK;
}

const char* getRamErrorMessage(RamStructErrors error) {
    switch (error) {
        //  -------------------   GENERAL ERRORS    ---------------------------
        case RAM_STATUS_OK:
            return "No erros in processor, everything is valid.\n";
        case RAM_ERROR_INVALID_ARGUMENT:
            return "Processor error: invalid argument (possibly set to NULL).\n";
        case RAM_ERROR_MEMORY_ALLOCATION_ERROR:
            return "Processor error: couldn't allocate memory.\n";
        case RAM_ERROR_BAD_VAR_INDEX:
            return "Ram error: bad var index.\n";

        default:
            return "Unknown error.\n";
    }
}

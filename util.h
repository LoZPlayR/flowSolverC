#ifndef UTIL_H
#define UTIL_H

// include libraries 
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

// Turns on/off debug mode
#define DEBUG 0
#define VERBOSE 0

/*
    0 - Nothing
    1 - Unsolved & Solutions
    2 - In progress
    3 - All steps (a LOT of output)
*/
#define DISPLAY_TYPE 1

enum DIRECTION { UP, DOWN, LEFT, RIGHT };

/*
    Determines the board print type
    0 - Numbers corresponding to color
    1 - Lines imitating the original game
*/
#define BOARD_PRINT_TYPE 1

// TODO: Make a debug print function

#endif
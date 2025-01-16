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

enum DIRECTION { UP, DOWN, LEFT, RIGHT };

/*
    Determines the board print type
    0 - Numbers corresponding to color
    1 - Arrows for indicating previous node
    2 - Lines imitating the original game
*/
#define BOARD_PRINT_TYPE 0

// TODO: Make a debug print function

#endif
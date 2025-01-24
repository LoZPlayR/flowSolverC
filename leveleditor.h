// This is for the level editor system. There'll prolly be a GUI
#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H
#include <ncurses.h>

#include "util.h"
#include "levelpack.h"

enum MAIN_FUNCTION {CREATE, LOAD, QUIT};

// Max side length of board
#define MAX_BOARD_SIDE 20

// Maximum total colors allowed in board
#define MAX_N 16

const char* COLOR_NAMES[16] = { "RED", "GREEN", "BLUE", "YELLOW", 
                                "ORANGE", "CYAN", "MAGENTA", "MAROON", 
                                "PURPLE", "WHITE", "GREY", "LIME", 
                                "TAN", "INDIGO", "AQUA", "PINK"};

void setup_leditor();
int main();
static void finish(int sig);

#endif
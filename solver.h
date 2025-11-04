#ifndef SOLVER_H
#define SOLVER_H

#include "move_gen.h"
#include "board.h"
#include "util.h"
#include "levelpack.h"
#include "logic.h"
#include "eval.h"

typedef struct {
    board_t* board;
    move_t* moves;
    int num_moves;
} position_t; 

typedef struct {
    float time;
    char* filename;
    int level;
    int frames;
} run_data_t;

int main(int argc, char **argv);

#endif
#ifndef SOLVER_H
#define SOLVER_H

#include "gen_moves.h"
#include "board.h"
#include "util.h"
#include "eval.h"
#include "levelpack.h"

typedef struct {
    board_t* board;
    move_t* moves;
    int num_moves;
    int curr;
} position_t; 

typedef struct {
    float time;
    char* filename;
    int level;
} run_data_t;

int main(int argc, char **argv);

#endif
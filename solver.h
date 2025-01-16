#ifndef SOLVER_H
#define SOLVER_H

#include "gen_moves.h"
#include "board.h"
#include "util.h"
#include "eval.h"

typedef struct {
    board_t* board;
    move_t* moves;
    int num_moves;
    int curr;
} position_t; 

int main();

#endif
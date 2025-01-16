#ifndef EVAL_H
#define EVAL_H

#include "board.h"
#include "util.h"

// Individual bitmap for a color
typedef uint32_t* dfs_visited_t;

bool is_solved(board_t* board);
bool is_solvable(board_t* board);

#endif
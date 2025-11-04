#ifndef LOGIC_H
#define LOGIC_H

#include "board.h"
#include "eval.h"

// For doing guaranteed moves to a board

// Remove edges between different colors
void prune_edges(board_t* board);

// Connect guaranteed paths
int fill_paths(board_t* board);

// implements the corner prune
void inverse_corner_prune(board_t* board, int r, int c);

// prunes edges in a 3x3 area around r, c
void prune_edges_area(board_t* board, int r, int c);

/// @brief Generates logically guaranteed moves and stores in buffer
/// @param board Board object
/// @param buffer move_t array for storing moves
/// @return number of moves generated
int logic_gen(board_t* board, move_t* buffer);

// possibly better logic gen
int logic_gen_complete(board_t* board, move_t* move_arr);

// Add corners
int corner_rule(board_t* board);

// Prevent lines from turning in on themselves
void corner_prune(board_t* board);

int guaranteed_spaces(board_t* board);


#endif
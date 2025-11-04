#ifndef EVAL_H
#define EVAL_H

#include "board.h"

// For evaluating the board

// Check if a board is completely solved
bool is_solved(board_t* board);

// Check for impossible edge counts in the board
bool valid_edges(board_t* board);

// Check occupied colored edges
bool valid_colors(board_t* board);

// Runs color/edge checks in the cell r, c and directly adjacent to r, c
bool validate_area(board_t* board, int r, int c);

// Ensures that there is some path to connect all lines
bool validate_paths(board_t* board);

// Ensure move will combine matching colors
bool validate_move(board_t* board, move_t move);

// Ensure board is consistent and doesn't have guaranteed moves
bool board_consistent(board_t* board);


#endif
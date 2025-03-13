#ifndef EVAL_H
#define EVAL_H

#include "board.h"
#include "util.h"
#include "gen_moves.h"

// Individual bitmap for a color
typedef uint32_t* dfs_visited_t;

// List of board locations
typedef board_location_t* line_t;

enum GUARANTEED_MOVES_RETURN { DONE, CONTINUE, FAIL };

typedef struct {
    move_t move;
    float value;
} sortable_move_t;

bool is_solved(board_t* board);
bool is_solvable(board_t* board);
void convert_to_sortable(move_t* old, sortable_move_t* new, int size);
bool evaluate_move(board_t* board, sortable_move_t* s);
int compare_sortable(const void* s1, const void* s2);
int find_guaranteed_spaces(board_t* board);

#endif
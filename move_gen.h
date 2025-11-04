#ifndef GEN_MOVES
#define GEN_MOVES


#include "util.h"


// Assume unique from guaranteed moves/logical moves. These are guesses. As such, will always
// be built off of a line (unless source)
// Still 2 bytes B) Allows for 64x64 size boards. If to == from, assume source node
typedef struct __attribute__((packed)) {
    uint8_t r : 6;
    uint8_t c : 6;
    enum DIRECTION to : 2;
    enum DIRECTION from : 2;
    uint8_t possibilities;      // How many total moves can happen in this position
    int info;                   // How many guaranteed moves result from performing this move
} move_t;

#include "board.h"

// Move generation works by iterating over the entire board, and looking for cells
// next to currently occupied cells. (i.e. canAppend)
// Will not connect lines that are different colors
// Currently, no loop detection

// Find all possible moves on the board
// Populates move_arr, returns number of moves generated
int generate_moves(board_t* board, move_t* move_arr);


// void perform_local_move(board_t* board, move_t move);
// void perform_move(board_t* src, board_t* dst, move_t move);
void print_move(move_t move);
// board_location_t get_move_dest(board_t* board, move_t move);

int compare_moves (const void* a, const void* b);

#include "eval.h"

int generate_moves_complete(board_t* board, move_t* move_arr);
#endif
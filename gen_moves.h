#ifndef GEN_MOVES
#define GEN_MOVES
#include "board.h"
#include "util.h"

// It all fits into a byte. Beautiful
typedef struct __attribute__((packed)) {
    enum COLOR color : 5;
    bool is_start : 1;
    enum DIRECTION dir : 2;
} move_t;

int generate_moves(board_t* board, move_t* move_arr);
void perform_move(board_t* src, board_t* dst, move_t move);
void print_move(move_t* move, int width);
#endif
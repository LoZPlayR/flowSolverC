#ifndef GEN_MOVES
#define GEN_MOVES
#include "board.h"
#include "util.h"

// It all fits into 2 bytes. Beautiful
// options could be scaled down to 2 bits if I ever need more space
typedef struct __attribute__((packed)) {
    enum COLOR color : 5;
    bool is_start : 1;
    enum DIRECTION dir : 2;
    uint8_t options;
} move_t;

int generate_moves(board_t* board, move_t* move_arr);
int generate_guaranteed_moves(board_t* board, move_t* move_arr);
void perform_local_move(board_t* board, move_t move);
void perform_move(board_t* src, board_t* dst, move_t move);
void print_move(move_t* move, int width);
#endif
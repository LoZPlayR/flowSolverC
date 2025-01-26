#ifndef BOARD_H
#define BOARD_H

#include "util.h"
#include "levelpack.h"

enum COLOR {EMPTY = -1, RED = 0, GREEN = 1, BLUE = 2, 
            YELLOW = 3, ORANGE = 4, CYAN = 5, MAGENTA = 6, 
            MAROON = 7, PURPLE = 8, WHITE = 9, GREY = 10, 
            LIME = 11, TAN = 12, INDIGO = 13, AQUA = 14, PINK = 15};

// defines a specific location on the board
typedef uint16_t board_location_t;

#define NO_LOC ((board_location_t){-1})

// Data that each board stores for each node
typedef struct __attribute__((packed)) {
    enum COLOR color : 5; // 16 colors + Empty
    bool up : 1;
    bool down : 1;
    bool left : 1;
    bool right : 1;
} board_node_t;

#define INVALID ((board_node_t){-1})

// TODO: Get rid of this?
#define NO_PREV -1

typedef struct {
    // An array of all nodes
    board_node_t* nodes;
    
    // The end of each line in the flow
    board_location_t* sources;

    int width;
    int height;
    int n;
} board_t;

// functions for board sources
board_location_t get_start_source(board_t* board, enum COLOR col);
board_location_t get_end_source(board_t* board, enum COLOR col);
void update_source(board_t* board, enum COLOR col, bool is_start, board_location_t new_loc);

// Functions for board_location_t //
bool loc_is_valid(board_location_t loc);
board_location_t get_loc(board_t* board, int x, int y);
board_location_t get_loc_up(board_t* board, board_location_t loc);
board_location_t get_loc_down(board_t* board, board_location_t loc);
board_location_t get_loc_left(board_t* board, board_location_t loc);
board_location_t get_loc_right(board_t* board, board_location_t loc);

// Functions for board_node_t //
enum COLOR get_node_color(board_node_t node);
bool node_up_edge(board_node_t node);
bool node_down_edge(board_node_t node);
bool node_left_edge(board_node_t node);
bool node_right_edge(board_node_t node);
void set_node_color(board_t* board, board_location_t loc, enum COLOR col);
void set_node_up_edge(board_t* board, board_location_t loc, bool new_val);
void set_node_down_edge(board_t* board, board_location_t loc, bool new_val);
void set_node_left_edge(board_t* board, board_location_t loc, bool new_val);
void set_node_right_edge(board_t* board, board_location_t loc, bool new_val);

// Functions for board_t //
// Functions for getting nodes
board_node_t get_node_at_loc(board_t* board, board_location_t loc);
board_node_t get_node_up(board_t* board, board_location_t loc);
board_node_t get_node_down(board_t* board, board_location_t loc);
board_node_t get_node_left(board_t* board, board_location_t loc);
board_node_t get_node_right(board_t* board, board_location_t loc);


// Functions for modifying nodes
void remove_edges_into_node(board_t* board, board_location_t loc);
void add_source_node(board_t* board, enum COLOR col, board_location_t loc);

// Creation and deletion
void create_board(unsolved_board_t b, board_t* board);
void destroy_board(board_t* board);
void copy_board(board_t* src, board_t* dest);

// Miscellaneous Functions //
bool are_locs_adjacent(board_t* board, board_location_t loc1, board_location_t loc2);
enum DIRECTION get_dir_between_nodes(board_t* board, board_location_t loc1, board_location_t loc2);
void print_board(board_t* board);


#endif
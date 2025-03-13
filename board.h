#ifndef BOARD_H
#define BOARD_H

#include "util.h"
#include "levelpack.h"

enum COLOR {EMPTY = -1, RED = 0, GREEN = 1, BLUE = 2, 
            YELLOW = 3, ORANGE = 4, CYAN = 5, MAGENTA = 6, 
            MAROON = 7, PURPLE = 8, WHITE = 9, GREY = 10, 
            LIME = 11, TAN = 12, INDIGO = 13, AQUA = 14, PINK = 15};

// defines a specific location on the board
// currently is just the location itself.
// locations take up 9 bits max (since 400 < 512)
// So I have 7 bits of free space rn
// Might as well store some extra data!
// If I save index and b->width, I can get
// everything I will potentially need with 2 bytes!
// Can I store more?
// Saving b->width requires 5 bytes. I have 2 more
// Is there any way to store height as well...?

// Maybe loc will be simpler. It just uses b->width
// and the index to store a position. It *can* provide
// More positions, but will only catch oob when going
// below 0? It would need to if I want the get_loc_left
// function to work. Let's do it i guess
typedef struct __attribute__((packed)) {
    uint16_t index : 9;
    uint8_t width : 5;
    // Extra 2 bits to work with
} board_location_t;

extern const board_location_t NO_LOC;

// Data that each board stores for each node
typedef  struct __attribute__((packed)) {
    enum COLOR color : 5; // 16 colors + Empty
    bool up : 1;
    bool down : 1;
    bool left : 1;
    bool right : 1;
    enum COLOR guaranteed_space : 5; // guranteed space
} board_node_t;

extern const board_node_t INVALID_NODE;

typedef struct {
    // An array of all nodes
    board_node_t* nodes;
    
    // The end of each line in the flow
    board_location_t* sources;

    int width;
    int height;
    int n;
    
    // Zobrist hash of the board
    // zob_key_t z_key;
} board_t;

// functions for board sources
board_location_t get_start_source(board_t* board, enum COLOR col);
board_location_t get_end_source(board_t* board, enum COLOR col);
void update_source(board_t* board, enum COLOR col, bool is_start, board_location_t new_loc);

// Functions for board_location_t //
int get_loc_col(board_location_t loc);
int get_loc_row(board_location_t loc);
int get_loc_index(board_location_t loc);
bool loc_is_valid(board_location_t loc);
board_location_t get_loc(board_t* board, int x, int y);
board_location_t get_loc_up(board_location_t loc);
board_location_t get_loc_down(board_location_t loc);
board_location_t get_loc_left(board_location_t loc);
board_location_t get_loc_right(board_location_t loc);
bool is_loc_corner(board_t* board, board_location_t loc);
bool locs_are_equal(board_location_t a, board_location_t b);

// Functions for board_node_t //
enum COLOR get_node_color(board_node_t* node);
bool get_node_up_edge(board_node_t* node);
bool get_node_down_edge(board_node_t* node);
bool get_node_left_edge(board_node_t* node);
bool get_node_right_edge(board_node_t* node);
enum COLOR node_guaranteed(board_node_t* node);
bool node_is_invalid(board_node_t* node);
void set_node_color(board_t* board, board_location_t loc, enum COLOR col);
void set_node_up_edge(board_t* board, board_location_t loc, bool new_val);
void set_node_down_edge(board_t* board, board_location_t loc, bool new_val);
void set_node_left_edge(board_t* board, board_location_t loc, bool new_val);
void set_node_right_edge(board_t* board, board_location_t loc, bool new_val);
bool set_node_guaranteed(board_t* board, enum COLOR col, board_location_t loc);

// Functions for board_t //
// Functions for getting nodes
board_node_t* get_node_at_loc(board_t* board, board_location_t loc);
board_node_t* get_node_up(board_t* board, board_location_t loc);
board_node_t* get_node_down(board_t* board, board_location_t loc);
board_node_t* get_node_left(board_t* board, board_location_t loc);
board_node_t* get_node_right(board_t* board, board_location_t loc);


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
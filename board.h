#ifndef BOARD_H
#define BOARD_H

#include "util.h"
#include "levelpack.h"
#include "hashtable.h"
#include "bitmap.h"

// TODO: Add invalid?
enum COLOR {
    EMPTY, RED, GREEN, BLUE, YELLOW, ORANGE, CYAN, MAGENTA, 
    MAROON, PURPLE, WHITE, GREY, LIME, TAN, INDIGO, AQUA, PINK
};

// For implementing Disjoint Set
// All cells start with a NULL parent except sources
// to 'addSet', we just set its parent to itself
// to union, we check both parents of the sets - if one is the source, it becomes the parent
// OW, we add the smaller set to the larger one
// if both are unique sources, we do a color comparison. Combine if the same, otherwise we remove the edges
// Getting parent/identity is just grabbing the parent (assuming it's not NULL)

// Holds useful information about the cell
typedef struct {
    bool isSource;              // True if the node is a source cell
    bool canAppend;             // True if the node is adjacent to an occupied cell
    bool isOccupied;            // True if there are 1 or 2 edges connect to the cell (and there is a set for this cell)
    enum COLOR col;             // The color of this node
} nodestate_t;

// Currently, -1 is invalid
#define NULL_NODE ((nodestate_t){ 0, 0, 0, -1 })

// Whether each edge can be connected to
typedef struct {
    bool n;
    bool e;
    bool s;
    bool w;
} edgestate_t;

#define NULL_EDGE ((edgestate_t){ 0, 0, 0, 0 })

// Container for cell state
typedef struct {
    nodestate_t node;
    edgestate_t edges;
} cellstate_t;

#define NULL_CELL ((cellstate_t){ NULL_NODE, NULL_EDGE })


/* Modified disjoint set
 * Have 2 'ends' of the set: parent and tail
 * Invariants:
 *  - The parent of all elements in a set should point to the parent of the set (eventually)
 *  - The tail of the parent should always point to the tail of the set
 *  - The size of the parent should be the number of elements in the set
 */ 
typedef struct {
    nodestate_t cell;       // Holds information
    key parent;             // Points to the representative of the set (usually a source node)
    key tail;               // Points to the tail of the set
    int size;               // size of this set
} boardset_t;

// Board is for retrieving cells by implementing a disjoint set
typedef struct {
    // Keep track of board info
    int width;
    int height;
    int n;
    
    // Holds the cell color and source-ness
    // will be mn bytes long
    boardset_t* cells;

    // Create bitmap for edge
    // It may be worth modifying the bitmap to also use the index instead of rowcol
    bitmap_t* edges;

    // TODO: Make an actual hash table
    hashtable_t* sources;

    // Save incorrect moves
    bitmap_t* incorrect_moves;
} board_t;

// Get index from r, c
key get_index(int width, int r, int c);

// Initalize/destroy the board
board_t* create_board(unsolved_board_t b);
void destroy_board(board_t* board);

// 'add' a cell to the board at a given row and column (set parent to itself)
void make_set(board_t*, int r, int c);

// Swaps the parent and tail of a set
key swap_set(board_t* board, key parentInd);

// Combine two sets into one
// Should always be either the parent or the tail of their sets
void board_union(board_t* board, key loc1Ind, key loc2Ind);

// Get the cell at a given position
cellstate_t get_cell(board_t* board, int r, int c);

// Prints a board
void print_board(board_t* board);

// Prints edge count at all positions of a board
void print_edgecount(board_t* board);

// Prints sources on the board
void print_sourcemap(board_t* board);

// Prints parents
void print_parents(board_t* board);

#include "move_gen.h"

// Mutates board by performing move
void perform_move(board_t* board, move_t move);

void copy_board(board_t* old, board_t* new);

// saves an incorrect move to the board 
void add_incorrect(board_t* board, move_t move);

// Checks if a move is incorrect on board
bool move_is_incorrect(board_t* board, move_t move);

#endif
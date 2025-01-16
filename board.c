#include "board.h"

// TODO: Make faster operations that do not require going thru the node type
// TODO: Finsh the pretty print board function

//////////////////////////////////
// Functions for source indices //
//////////////////////////////////

/*
    Returns the start source for a given color
    Must always return the same source 
*/
board_location_t get_start_source(board_t* board, enum COLOR col){
    return board->sources[2 * col];
}

board_location_t get_end_source(board_t* board, enum COLOR col){
    return board->sources[2 * col + 1];
}

void update_source(board_t* board, enum COLOR col, bool is_start, board_location_t new_loc){
    board->sources[2 * col + 1 - is_start] = new_loc;
}


////////////////////////////////////
// Functions for board_location_t //
////////////////////////////////////

/*
    Checks if the given board_location is valid
*/
bool loc_is_valid(board_location_t loc){
    return loc != NO_LOC;
}

/*
    Given a board type and a coordinate, return the corresponding board_location_t

    Notes: Using board_t for width/height. x and y are from top left. In debug mode,
    this will catch issues where the input loc is out of bounds.
*/
board_location_t get_loc(board_t* board, int x, int y){
    #if DEBUG
        assert(x + board->width * y >= 0);
        assert(x + board->width * y < board->width * board->height);
    #endif

    return (board_location_t) (x + board->width * y);
}

/*
    Given a board and a board_location return the board_location one spot up
*/
board_location_t get_loc_up(board_t* board, board_location_t loc){
    int _loc = (int) loc;
    return (_loc < board->width) ? NO_LOC : _loc - board->width;
}

/*
    Given a board and a board_location return the board_location one spot down
*/
board_location_t get_loc_down(board_t* board, board_location_t loc){
    int _loc = (int) loc;
    return (_loc  > (board->height - 1) * board->width) ? NO_LOC : _loc + board->width;
}

/*
    Given a board and a board_location return the board_location one spot left
*/
board_location_t get_loc_left(board_t* board, board_location_t loc){
    int _loc = (int) loc;
    return (_loc % board->width == 0) ? NO_LOC : _loc - 1;
}

/*
    Given a board and a board_location return the board_location one spot right
*/
board_location_t get_loc_right(board_t* board, board_location_t loc){
    int _loc = (int) loc;
    return (_loc % board->width == board->width - 1) ? NO_LOC : _loc + 1;
}


////////////////////////////////
// Functions for board_node_t //
////////////////////////////////

enum COLOR get_node_color(board_node_t node){
    return node.color;
}

bool node_up_edge(board_node_t node){
    return node.up;
}

bool node_down_edge(board_node_t node){
    return node.down;
}

bool node_left_edge(board_node_t node){
    return node.left;
}

bool node_right_edge(board_node_t node){
    return node.right;
}

void set_node_color(board_t* board, board_location_t loc, enum COLOR col){
    board->nodes[(int)loc].color = col;
}

void set_node_up_edge(board_t* board, board_location_t loc, bool new_val){
    board->nodes[(int)loc].up = new_val;
}

void set_node_down_edge(board_t* board, board_location_t loc, bool new_val){
    board->nodes[(int)loc].down = new_val;
}

void set_node_left_edge(board_t* board, board_location_t loc, bool new_val){
    board->nodes[(int)loc].left = new_val;
}

void set_node_right_edge(board_t* board, board_location_t loc, bool new_val){
    board->nodes[(int)loc].right = new_val;
}


///////////////////////////
// Functions for board_t //
///////////////////////////

// Functions for getting nodes

/* 
    Returns the board_node given a board_location_t
*/
board_node_t get_node_at_loc(board_t* board, board_location_t loc){
    return board->nodes[loc];
}

/*
    Returns the board_node one spot up. Returns NULL if there is no node
*/
board_node_t get_node_up(board_t* board, board_location_t loc){
    board_location_t up = get_loc_up(board, loc);
    return loc_is_valid(up) ? board->nodes[get_loc_up(board, loc)] : INVALID;
}

/*
    Returns the board_node one spot down. Returns NULL if there is no node
*/
board_node_t get_node_down(board_t* board, board_location_t loc){
    board_location_t down = get_loc_down(board, loc);
    return loc_is_valid(down) ? board->nodes[get_loc_down(board, loc)] : INVALID;
}

/*
    Returns the board_node one spot left. Returns NULL if there is no node
*/
board_node_t get_node_left(board_t* board, board_location_t loc){
    board_location_t left = get_loc_left(board, loc);
    return loc_is_valid(left) ? board->nodes[get_loc_left(board, loc)] : INVALID;
}

/*
    Returns the board_node one spot right. Returns NULL if there is no node
*/
board_node_t get_node_right(board_t* board, board_location_t loc){
    board_location_t right = get_loc_right(board, loc);
    return loc_is_valid(right) ? board->nodes[get_loc_right(board, loc)] : INVALID;
}

// Functions for modifying nodes

// TODO: Add function for adding edges to nodes

/*
    Remove all edges into the node.

    Notes: In debug mode, checks if loc is valid
*/
void remove_edges_into_node(board_t* board, board_location_t loc){
    #if DEBUG
        assert(loc_is_valid(loc));
    #endif
    
    board_location_t up = get_loc_up(board, loc);
    if (loc_is_valid(up)){
        set_node_down_edge(board, up, false);
    }

    board_location_t down = get_loc_down(board, loc);
    if (loc_is_valid(down)){
        set_node_up_edge(board, down, false);
    }

    board_location_t left = get_loc_left(board, loc);
    if (loc_is_valid(left)){
        set_node_right_edge(board, left, false);
    }

    board_location_t right = get_loc_right(board, loc);
    if (loc_is_valid(right)){
        set_node_left_edge(board, right, false);
    }
}

/*
 * Add a source node at loc of color col
 */
void add_source_node(board_t* board, enum COLOR col, board_location_t loc){
    // Col points to the first of 2 nodes in the list. 
    // The first 8 bits of these nodes are marked as 0 or -1.
    // If the first 8 bits are -1, then we can set to position = color
    // (since these are ints). Otherwise, we set to position = color + 1

    board_node_t curr = get_node_at_loc(board, loc);
    board->sources[2 * col + ((board->sources[2 * col]) == ((uint16_t) -1) ? 0 : 1)] = loc;
    set_node_color(board, loc, col);

    // Remove all edges into the node
    remove_edges_into_node(board, loc);
}

/*
    Creates a board given parameters.

    TODO: Make it take parameters
*/
board_t* create_board(void){
    int height = 5;
    int width = 5;
    int n = 3;

    board_t* board = malloc(sizeof(board_t));

    board->width = width;
    board->height = height;
    board->n = n;

    board->nodes = malloc(sizeof(board_node_t) * width * height);

    // Make nodes and add walls
    for (int j = 0; j < height; j++){
        for (int i = 0; i < width; i++){
            board_node_t* curr = &board->nodes[j * width + i];
            curr->color = EMPTY;
            curr->up = j == 0 ? false : true;
            curr->down = j == (height - 1) ? false : true;
            curr->left = i == 0 ? false : true;
            curr->right = i == (width - 1) ? false : true;
        }
    }

    board->sources = malloc(sizeof(board_location_t) * n * 2);

    // Mark every other source as unused 
    for (int i = 0; i < n; i++){
        board->sources[2 * i] = (board_location_t) -1;
        board->sources[2 * i + 1] = (board_location_t) 0;
    }

    // Add more walls here lmao

    // Add source nodes here
    // Temp: Bonus Levels -> 5x5 - easy -> 19
    add_source_node(board, RED, 1 * width + 1);
    add_source_node(board, RED, 1 * width + 3);
    add_source_node(board, GREEN, 0 * width + 4);
    add_source_node(board, GREEN, 2 * width + 2);
    add_source_node(board, BLUE, 1 * width + 4);
    add_source_node(board, BLUE, 0 * width + 1);

    return board;
}

void destroy_board(board_t* board){
    free(board->nodes);
    free(board->sources);
    free(board);
}

/*
 * Copies one board onto another board
*/
void copy_board(board_t* src, board_t* dst){
    dst->height = src->height;
    dst->width = src->width;
    dst->n = src->n;

    // copy src to dst
    for (int i = 0; i < src->width * src->height; i++){
        dst->nodes[i] = src->nodes[i];
    }

    // copy the sources
    for (int i = 0; i < 2 * src->n; i++){
        dst->sources[i] = src->sources[i];
    }
}


/////////////////////////////
// Miscellaneous Functions //
/////////////////////////////

bool are_locs_adjacent(board_t* board, board_location_t loc1, board_location_t loc2){ 
    // convert to ints
    int ind1 = (int) loc1;
    int ind2 = (int) loc2;

    if (ind1 > ind2){ // ind2 is left or up
        return (ind2 == (ind1 - 1) && (ind1 % board->width != 0)) || (ind2 == (ind1 - board->width));
    } else { // ind2 is right or down (or ind1)
        return (ind1 == (ind2 - 1) && (ind2 % board->width != 0)) || (ind1 == (ind2 - board->width));
    }
}

/*
    Gets the direction to go from loc1 to loc2. Assumes they are adjacent.
*/
enum DIRECTION get_dir_between_nodes(board_t* board, board_location_t loc1, board_location_t loc2){
    #if DEBUG
        assert(are_locs_adjacent(board, loc1, loc2));
    #endif

    // convert to ints
    int ind1 = (int) loc1;
    int ind2 = (int) loc2;

    // fuck it one liner
    return (ind1 > ind2) ? ((ind2 == (ind1 - 1)) ? LEFT : UP) : ((ind1 == (ind2 - 1)) ? RIGHT : DOWN);
}

// Print a board as nicely as possible
// TODO: Make it display an accurate board instead of an array of numbers. for corners, 
// using - | \ / and •. Bonus points if we can bold? ⌝⌟⌜⌞ are also available
// Use * for temporary head?
void print_board(board_t* board){
    // Assume 16 colors max
    // 2-wide case
    if (board->n > 8){ // 9 colors + empty
        printf("Implement print larger board\n");
    } else {
        // print top bar
        printf("•");
        for (int i = 0; i < board->width; i++){
            printf("=-=");
        }
        printf("•\n");

        // print nodes
        for (int j = 0; j < board->height; j++){
            printf("|");
            for (int i = 0; i < board->width; i++){
                board_location_t curr = get_loc(board, i, j);
                
                #if BOARD_PRINT_TYPE == 0
                    enum COLOR c_node = get_node_at_loc(board, curr).color;
                    switch (c_node)
                    {
                    case RED:
                        printf("\033[0;31m");
                    break;
                    case GREEN:
                        printf("\033[0;32m");
                    break;
                    case BLUE:
                        printf("\033[0;34m");               
                    break;
                    default:
                        printf("\033[0;37m");
                    break;
                    }
                    printf(c_node == EMPTY ? "   " : " %d ", c_node);
                    printf("\033[0;37m");
                #elif BOARD_PRINT_TYPE == 1
                    printf("use these [•⌝⌟⌜⌞|-] ");
                    printf("1 needs to be reimplemented\n");
                    break;

                    int prev = get_node_at_loc(board, curr)->prev;

                    if (prev == NO_PREV) {
                        printf(" • ");
                        continue;
                    }
                    
                    if (prev < curr){ // left or up
                        printf(prev == curr - 1 ? " < " : " ^ ");
                    } else { // down or right
                        printf(prev == curr + 1 ? " > " : " v ");
                    }
                #else
                    printf(" 2 is unimplemented\n");
                    break;
                #endif
            }
            printf("|\n");
        }

        // print bottom bar
        printf("•");
        for (int i = 0; i < board->width; i++){
            printf("=-=");
        }
        printf("•\n");
    }
}
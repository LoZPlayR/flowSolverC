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

bool loc_is_valid_internal(board_t* board, board_location_t loc){
    return loc >= 0 && loc < board->width * board->height;
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
    if (!loc_is_valid_internal(board, loc)) return NO_LOC;
    int _loc = (int) loc;
    return (_loc < board->width) ? NO_LOC : _loc - board->width;
}

/*
    Given a board and a board_location return the board_location one spot down
*/
board_location_t get_loc_down(board_t* board, board_location_t loc){
    if (!loc_is_valid_internal(board, loc)) return NO_LOC;
    int _loc = (int) loc;
    return (_loc  >= (board->height - 1) * board->width) ? NO_LOC : _loc + board->width;
}

/*
    Given a board and a board_location return the board_location one spot left
*/
board_location_t get_loc_left(board_t* board, board_location_t loc){
    if (!loc_is_valid_internal(board, loc)) return NO_LOC;
    int _loc = (int) loc;
    return (_loc % board->width == 0) ? NO_LOC : _loc - 1;
}

/*
    Given a board and a board_location return the board_location one spot right
*/
board_location_t get_loc_right(board_t* board, board_location_t loc){
    if (!loc_is_valid_internal(board, loc)) return NO_LOC;
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
void create_board(unsolved_board_t b, board_t* board){
    board->width = b.width;
    board->height = b.height;
    board->n = b.n;

    board->nodes = malloc(sizeof(board_node_t) * b.width * b.height);

    // Make nodes and add walls
    for (int j = 0; j < b.height; j++){
        for (int i = 0; i < b.width; i++){
            board_node_t* curr = &board->nodes[j * b.width + i];
            curr->color = EMPTY;
            curr->up = j == 0 ? false : true;
            curr->down = j == (b.height - 1) ? false : true;
            curr->left = i == 0 ? false : true;
            curr->right = i == (b.width - 1) ? false : true;
        }
    }

    board->sources = malloc(sizeof(board_location_t) * b.n * 2);

    // Mark every other source as unused 
    for (int i = 0; i < b.n; i++){
        board->sources[2 * i] = (board_location_t) -1;
        board->sources[2 * i + 1] = (board_location_t) 0;
    }

    // Add more walls here lmao
    

    // Add source nodes
    for (int i = 0; i < b.n; i++){
        int y1 = b.positions[4 * i];
        int x1 = b.positions[4 * i + 1];
        int y2 = b.positions[4 * i + 2];
        int x2 = b.positions[4 * i + 3];

        add_source_node(board, i, y1 * b.width + x1);
        add_source_node(board, i, y2 * b.width + x2);
    }
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
/*
    Sets the text color depending on col
*/
void set_text_color(enum COLOR col){
    switch (col){
    case RED:
        printf("\033[38;2;234;51;35m");
    break;
    case GREEN:
        printf("\033[38;2;61;139;38m");
    break;
    case BLUE:
        printf("\033[38;2;20;41;245m");             
    break;
    case YELLOW:
        printf("\033[38;2;231;223;73m");
    break;
    case ORANGE:
        printf("\033[38;2;235;142;52m");
    break;
    case CYAN:
        printf("\033[38;2;116;251;253m");
    break;
    case MAGENTA:
        printf("\033[38;2;232;54;196m");
    break;
    case MAROON:
        printf("\033[38;2;151;51;47m");
    break;
    case PURPLE:
        printf("\033[38;2;117;20;122m");
    break;
    case WHITE:
        printf("\033[38;2;255;255;255m");
    break;
    case GREY:
        printf("\033[38;2;158;160;187m");
    break;
    case LIME:
        printf("\033[38;2;117;251;76m");
    break;
    case TAN:
        printf("\033[38;2;158;137;89m");
    break;
    case INDIGO:
        printf("\033[38;2;50;41;171m");
    break;
    case AQUA:
        printf("\033[38;2;54;125;125m");
    break;
    case PINK:
        printf("\033[38;2;234;131;231m");
    break;
    default:
        printf("\033[0;37m");
    break;
    }
}

void reset_text_color(){
    printf("\033[0;37m");
}

// Print a board as nicely as possible
void print_board(board_t* board){
    printf("╔");
    for (int i = 0; i < board->width; i++){
        printf("══");
    }
    printf("═╗\n");

    // print nodes
    for (int j = 0; j < board->height; j++){
        printf("║");
        for (int i = 0; i < board->width; i++){
            board_location_t curr = get_loc(board, i, j);
            enum COLOR col = get_node_color(get_node_at_loc(board, curr));

            if (col == EMPTY) {
                printf("  ");
                continue;
            }
            
            #if BOARD_PRINT_TYPE == 0
                set_text_color(col);
                printf("%2d", col);
                reset_text_color();
            #elif BOARD_PRINT_TYPE == 1
                #define UP_MASK 1
                #define DOWN_MASK 2
                #define LEFT_MASK 4
                #define RIGHT_MASK 8

                // Need to reimplement using a new method. I will check all 4 edges.
                // There can be 0-2 edges with the same color. Depending on the arrangement,
                // I will use a switch statement to draw the correct piece

                uint8_t result = 0;
                if (get_node_color(get_node_up(board, curr)) == col) result |= UP_MASK;
                if (get_node_color(get_node_down(board, curr)) == col) result |= DOWN_MASK;
                if (get_node_color(get_node_left(board, curr)) == col) result |= LEFT_MASK;
                if (get_node_color(get_node_right(board, curr)) == col) result |= RIGHT_MASK;

                set_text_color(col);

                switch (result)
                {
                case 0: // Lone node
                    printf(" ⏺");
                break;

                // Lone nodes - assume start/end sources
                case UP_MASK:
                    printf(" ⏺");
                break;
                case DOWN_MASK:
                    printf(" ⏺");
                break;
                case LEFT_MASK:
                    printf("━⏺");
                break;
                case RIGHT_MASK:
                    printf(" ⏺");
                break;

                // All possible pairs
                case UP_MASK | DOWN_MASK:
                    printf(" ┃");
                break;
                case UP_MASK | LEFT_MASK:
                    printf("━┛");
                break;
                case UP_MASK | RIGHT_MASK:
                    printf(" ┗");
                break;
                case DOWN_MASK | LEFT_MASK:
                    printf("━┓");
                break;
                case DOWN_MASK | RIGHT_MASK:
                    printf(" ┏");
                break;
                case LEFT_MASK | RIGHT_MASK:
                    printf("━━");
                break;

                // Probably wrong
                default: 
                    printf(" ?");
                break;
                }
                reset_text_color();  
            #else
                printf("Unimplemented\n");
                break;
            #endif
        }
        printf(" ║\n");
    }

    // print bottom bar
    printf("╚");
    for (int i = 0; i < board->width; i++){
        printf("══");
    }
    printf("═╝\n");
}
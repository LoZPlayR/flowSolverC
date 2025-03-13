#include "board.h"

// TODO: Make faster operations that do not require going thru the node type
// TODO: Finsh the pretty print board function
// TODO: Add board_location_t functions for getting the 
//       row/col or the index from a location (in progress)
// TODO: Remove sanity checks in non-debug mode. In general, I want to assume
//       that I'm not passing in bad values, so there's no point in double
//       checking them. It also let's me choose to not define certain behavior (W)

const board_location_t NO_LOC = {-1, 0};
const board_node_t INVALID_NODE = {-16, 0, 0, 0, 0, -16};

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
    Returns the x coord of the given location
*/
int get_loc_col(board_location_t loc){
    return loc.index % loc.width;
}

/*
    Returns the y coord of the given location
*/
int get_loc_row(board_location_t loc){
    return loc.index / loc.width ;
}

/*
    Returns the row-major index of the given location
*/
int get_loc_index(board_location_t loc){
    return loc.index;
}

/*
    Checks if the given board_location is valid
*/
bool loc_is_valid(board_location_t loc){
    return loc.width != NO_LOC.width;
}

bool loc_is_valid_internal(board_t* board, board_location_t loc){
    if (loc.width == NO_LOC.width) return false;
    bool ret = loc.index >= 0 && loc.index < board->width * board->height;
    //if (!ret) printf("tried %d", get_loc_index(loc));
    return ret;
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

    board_location_t loc;
    loc.index = x + board->width * y;
    loc.width = board->width;

    return loc;
}

/*
    Given a board_location, returns the board_location one spot up
    Returns NO_LOC if the new position is not on the board
*/
board_location_t get_loc_up(board_location_t loc){
    board_location_t new_loc;
    new_loc.index = loc.index - loc.width;
    new_loc.width = loc.width;

    return (loc.index < loc.width) ? NO_LOC : new_loc;
}

/*
    Given a board_location, return the board_location one spot down
    May return results off of the board!
*/
board_location_t get_loc_down(board_location_t loc){
    board_location_t new_loc;
    new_loc.index = loc.index + loc.width;
    new_loc.width = loc.width;

    return new_loc;
}

/*
    Given a board_location, returns the board_location one spot left
    Returns NO_LOC if the new position is not on the board
*/
board_location_t get_loc_left(board_location_t loc){
    board_location_t new_loc;
    new_loc.index = loc.index - 1;
    new_loc.width = loc.width;

    // Wrapped from 0 to something
    return ((loc.index % loc.width) < 1) ? NO_LOC : new_loc;
}

/*
    Given a board and a board_location return the board_location one spot right
    Returns NO_LOC if the new position is not on the board
*/
board_location_t get_loc_right(board_location_t loc){
    board_location_t new_loc;
    new_loc.index = loc.index + 1;
    new_loc.width = loc.width;
    
    // Wrapped from something to 0
    return (new_loc.index % loc.width < 1) ? NO_LOC : new_loc;
}

bool is_loc_corner(board_t* board, board_location_t loc){
    return  (get_loc_row(loc) == 0 || get_loc_row(loc) == board->height - 1) && \
            (get_loc_col(loc) == 0 || get_loc_col(loc) == board->width - 1);
}

bool locs_are_equal(board_location_t a, board_location_t b){
    return a.index == b.index;
}

////////////////////////////////
// Functions for board_node_t //
////////////////////////////////

enum COLOR get_node_color(board_node_t* node){
    return node->color;
}

bool get_node_up_edge(board_node_t* node){
    return node->up;
}

bool get_node_down_edge(board_node_t* node){
    return node->down;
}

bool get_node_left_edge(board_node_t* node){
    return node->left;
}

bool get_node_right_edge(board_node_t* node){
    return node->right;
}

enum COLOR node_guaranteed(board_node_t* node){
    return node->guaranteed_space;
}

bool node_is_invalid(board_node_t* node){
    return node == &INVALID_NODE;
    // return  node.color == INVALID_NODE.color &&
    //         node.up == INVALID_NODE.up &&
    //         node.down == INVALID_NODE.down &&
    //         node.left == INVALID_NODE.left &&
    //         node.right == INVALID_NODE.right &&
    //         node.guaranteed_space == INVALID_NODE.guaranteed_space;
}

void set_node_color(board_t* board, board_location_t loc, enum COLOR col){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif
    board->nodes[loc.index].color = col;
}

void set_node_up_edge(board_t* board, board_location_t loc, bool new_val){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif
    board->nodes[loc.index].up = new_val;
}

void set_node_down_edge(board_t* board, board_location_t loc, bool new_val){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif
    board->nodes[loc.index].down = new_val;
}

void set_node_left_edge(board_t* board, board_location_t loc, bool new_val){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif
    board->nodes[loc.index].left = new_val;
}

void set_node_right_edge(board_t* board, board_location_t loc, bool new_val){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif
    board->nodes[loc.index].right = new_val;
}

/*
 * Marks a node as a guaranteed space. Returns false if the guaranteed space
 * is not EMPTY or col
 */
bool set_node_guaranteed(board_t* board, enum COLOR col, board_location_t loc){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif

    int index = get_loc_index(loc);
    bool ret =  board->nodes[index].guaranteed_space == EMPTY || 
                board->nodes[index].guaranteed_space == col;
    board->nodes[index].guaranteed_space = col;
    return ret;
}


///////////////////////////
// Functions for board_t //
///////////////////////////

// Functions for getting nodes

/* 
    Returns the board_node given a board_location_t
*/
board_node_t* get_node_at_loc(board_t* board, board_location_t loc){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif
    
    return loc_is_valid_internal(board, loc) ? \
        &(board->nodes[get_loc_index(loc)]) : (board_node_t*) &INVALID_NODE;
}

/*
    Returns the board_node one spot up. Returns NULL if there is no node
*/
board_node_t* get_node_up(board_t* board, board_location_t loc){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif
    board_location_t up = get_loc_up(loc);
    
    return loc_is_valid(up) ? \
        get_node_at_loc(board, up) : (board_node_t*) &INVALID_NODE;
}

/*
    Returns the board_node one spot down. Returns NULL if there is no node
*/
board_node_t* get_node_down(board_t* board, board_location_t loc){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif
    board_location_t down = get_loc_down(loc);
    
    return loc_is_valid_internal(board, down) ? \
        get_node_at_loc(board, get_loc_down(loc)) : (board_node_t*) &INVALID_NODE;
}

/*
    Returns the board_node one spot left. Returns NULL if there is no node
*/
board_node_t* get_node_left(board_t* board, board_location_t loc){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif
    board_location_t left = get_loc_left(loc);
    return loc_is_valid(left) ? \
        get_node_at_loc(board, get_loc_left(loc)) : (board_node_t*) &INVALID_NODE;
}

/*
    Returns the board_node one spot right. Returns NULL if there is no node
*/
board_node_t* get_node_right(board_t* board, board_location_t loc){
    #if DEBUG
        assert(loc_is_valid_internal(board, loc));
    #endif
    board_location_t right = get_loc_right(loc);

    return loc_is_valid(right) ? \
        get_node_at_loc(board, get_loc_right(loc)) : (board_node_t*) &INVALID_NODE;
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
    
    board_location_t up = get_loc_up(loc);
    if (loc_is_valid_internal(board, up)){
        set_node_down_edge(board, up, false);
    }

    board_location_t down = get_loc_down(loc);
    if (loc_is_valid_internal(board, down)){
        set_node_up_edge(board, down, false);
    }

    board_location_t left = get_loc_left(loc);
    if (loc_is_valid_internal(board, left)){
        set_node_right_edge(board, left, false);
    }

    board_location_t right = get_loc_right(loc);
    if (loc_is_valid_internal(board, right)){
        set_node_left_edge(board, right, false);
    }
}

/*
 * Add a source node at loc of color col
 */
void add_source_node(board_t* board, enum COLOR col, board_location_t loc){
    // Sources array initalized to contain indexes alternating -1 and 0 (and set board width)
    // Using col, we get the position in the array. If the index is -1, we know to
    // move to the next one 

    board_node_t* curr = get_node_at_loc(board, loc);
    
    // board_location.index has 9 bits, -1 = 2^9 - 1 = 512 - 1 = 511
    int offset = ((board->sources[2 * col].index) == 511) ? 0 : 1;
    board->sources[2 * col + offset].index = loc.index;

    set_node_color(board, loc, col);

    // Remove all edges into the node
    remove_edges_into_node(board, loc);
}

/*
    Creates a board from a compact board
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
            curr->guaranteed_space = EMPTY;
        }
    }

    board->sources = malloc(sizeof(board_location_t) * b.n * 2);

    // Mark every other source as unused 
    for (int i = 0; i < b.n; i++){
        board->sources[2 * i].index = -1;
        board->sources[2 * i].width = b.width;
        board->sources[2 * i + 1].index = 0;
        board->sources[2 * i + 1].width = b.width;
    }

    // Add more walls here lmao
    

    // Add source nodes
    for (int i = 0; i < b.n; i++){
        int y1 = b.positions[4 * i];
        int x1 = b.positions[4 * i + 1];
        int y2 = b.positions[4 * i + 2];
        int x2 = b.positions[4 * i + 3];


        add_source_node(board, i, get_loc(board, x1, y1));
        add_source_node(board, i, get_loc(board, x2, y2));
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

    // copy src nodes to dst
    for (int i = 0; i < src->width * src->height; i++){
        dst->nodes[i] = src->nodes[i];
    }

    // copy the src sources to dst
    for (int i = 0; i < 2 * src->n; i++){
        dst->sources[i] = src->sources[i];
    }
}


/////////////////////////////
// Miscellaneous Functions //
/////////////////////////////

bool are_locs_adjacent(board_t* board, board_location_t loc1, board_location_t loc2){ 
    bool ret;
    if (get_loc_index(loc1) > get_loc_index(loc2)){ // loc2 is left or up
        return (get_loc_index(loc2) == (get_loc_index(loc1) - 1) && (get_loc_index(loc1) % board->width != 0)) || (get_loc_index(loc2) == (get_loc_index(loc1) - board->width));
    } else { // loc2 is right or down (or loc1)
        return (get_loc_index(loc1) == (get_loc_index(loc2) - 1) && (get_loc_index(loc2) % board->width != 0)) || (get_loc_index(loc1) == (get_loc_index(loc2) - board->width));
    }
}

/*
    Gets the direction to go from loc1 to loc2. Assumes they are adjacent.
*/
enum DIRECTION get_dir_between_nodes(board_t* board, board_location_t loc1, board_location_t loc2){
    #if DEBUG
        assert(are_locs_adjacent(board, loc1, loc2));
    #endif

    // fuck it one liner
    return (get_loc_index(loc1) > get_loc_index(loc2)) ? ((get_loc_index(loc2) == (get_loc_index(loc1) - 1)) ? LEFT : UP) : ((get_loc_index(loc1) == (get_loc_index(loc2) - 1)) ? RIGHT : DOWN);
}
/*
    Sets the text color depending on col
    If fg is true, changes text. Otherwise sets highlight
*/
void set_text_color(enum COLOR col, bool fg){
    char a = fg ? '3' : '4';
    switch (col){
    case RED:
        printf(fg ? "\033[38;2;234;51;35m" : "\033[48;2;64;20;21m");
    break;
    case GREEN:
        printf(fg ? "\033[38;2;61;139;38m" : "\033[48;2;27;47;23m");
    break;
    case BLUE:
        printf(fg ? "\033[38;2;20;41;245m" : "\033[48;2;26;32;81m");            
    break;
    case YELLOW:
        printf(fg ? "\033[38;2;231;223;73m" : "\033[48;2;43;41;16m");
    break;
    case ORANGE:
        printf(fg ? "\033[38;2;235;142;52m" : "\033[48;2;51;37;15m");
    break;
    case CYAN:
        printf(fg ? "\033[38;2;116;251;253m" : "\033[48;2;21;46;45m");
    break;
    case MAGENTA:
        printf(fg ? "\033[38;2;232;54;196m" : "\033[48;2;54;22;48m");
    break;
    case MAROON:
        printf(fg ? "\033[38;2;151;51;47m" : "\033[48;2;53;29;27m");
    break;
    case PURPLE:
        printf(fg ? "\033[38;2;117;20;122m" : "\033[48;2;51;22;55m");
    break;
    case WHITE:
        printf(fg ? "\033[38;2;255;255;255m" : "\033[48;2;38;40;38m");
    break;
    case GREY:
        printf(fg ? "\033[38;2;158;160;187m" : "\033[48;2;41;42;42m");
    break;
    case LIME:
        printf(fg ? "\033[38;2;117;251;76m" : "\033[48;2;23;49;17m");
    break;
    case TAN:
        printf(fg ? "\033[38;2;158;137;89m" : "\033[48;2;45;40;29m");
    break;
    case INDIGO:
        printf(fg ? "\033[38;2;50;41;171m" : "\033[48;2;35;32;65m");
    break;
    case AQUA:
        printf(fg ? "\033[38;2;54;125;125m" : "\033[48;2;24;43;45m");
    break;
    case PINK:
        printf(fg ? "\033[38;2;234;131;231m" : "\033[48;2;46;31;47m");
    break;
    default:
        if (fg) printf("\033[0;37m");
    break;
    }
}

void reset_text_color(){
    printf("\033[0;37m");
}

// Print a board as nicely as possible
void print_board(board_t* board){
    reset_text_color();
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
            reset_text_color();
            set_text_color(col, true);
            set_text_color(node_guaranteed(get_node_at_loc(board, curr)), false);

            if (col == EMPTY) {
                printf("  ");
                continue;
            }
            
            #if BOARD_PRINT_TYPE == 0
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

                switch (result)
                {
                case 0: // Lone node
                
                // 1 connection - assume start/end sources
                case UP_MASK:
                case DOWN_MASK:
                case RIGHT_MASK:
                    set_text_color(col, false);
                    printf(" ⏺");
                    reset_text_color();
                break;

                case LEFT_MASK:
                    set_text_color(col, false);
                    printf("━⏺");
                    reset_text_color();
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
            #else
                printf("Unimplemented\n");
                break;
            #endif
            reset_text_color();
        }
        reset_text_color();
        printf(" ║\n");
    }
    reset_text_color();

    // print bottom bar
    printf("╚");
    for (int i = 0; i < board->width; i++){
        printf("══");
    }
    printf("═╝\n");
}
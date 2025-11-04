#include "board.h"

// Function for converting r, c to offset
key get_index(int width, int r, int c) {
    return r * width + c;
}

// Makes a set
void make_set(board_t* board, int r, int c){
    int ind = get_index(board->width, r, c);
    board->cells[ind].parent = ind;
    board->cells[ind].tail = ind;
    board->cells[ind].size = 1;
}

// How many entries to flatten on a get parent call (Prolly won't have to be more than 4)
#define FLATTEN_SIZE 4

// Finds rep loc of the given location
key get_parent(board_t* board, key currInd){
    key* update = malloc(sizeof(key) * FLATTEN_SIZE);
    int ind = 0;
    
    while(board->cells[currInd].parent != currInd){
        // printf("%d\n", currInd);
        update[(ind++) % FLATTEN_SIZE] = currInd;
        currInd = board->cells[currInd].parent;
    }

    // update all to parent
    for (int i = 0; i < ((ind < FLATTEN_SIZE) ? ind : FLATTEN_SIZE); i++){
        // printf("update[%d] = %d\n", i, update[i]);
        board->cells[update[i]].parent = currInd;
    }
    free(update);

    return currInd;
}

// returns new parent key
key swap_set(board_t* board, key parentInd) {
    #if DEBUG
        // Ensure this is the parent of the set
        assert(get_parent(board, parentInd) == parentInd);
       
    #endif

    key tailInd = board->cells[parentInd].tail;

    board->cells[parentInd].parent = tailInd;
    board->cells[tailInd].tail = parentInd;
    board->cells[tailInd].parent = tailInd;
    board->cells[tailInd].size = board->cells[parentInd].size;

    return tailInd;
}

// Combines two sets into one
void board_union(board_t* board, key loc1Ind, key loc2Ind) {
    // Check that indices are safe
    #if DEBUG
        assert(0 <= loc1Ind && loc1Ind < board->height * board->width);
        assert(0 <= loc2Ind && loc2Ind < board->height * board->width);
    #endif
    
    // Get parents and tails of each set
    key set1parent = get_parent(board, loc1Ind);
    key set2parent = get_parent(board, loc2Ind);

    key set1tail = board->cells[set1parent].tail;
    key set2tail = board->cells[set2parent].tail;

    // Should always be combining by connecting a parent or a tail
    #if DEBUG
        assert(loc1Ind == set1parent || loc1Ind == set1tail);
        assert(loc2Ind == set2parent || loc2Ind == set2tail);
    #endif

    // Not sure if this is necessary
    boardset_t* loc1 = board->cells + set1parent;
    boardset_t* loc2 = board->cells + set2parent;
    
    // Check that both locs are sets and unique
    #if DEBUG
        assert(loc1->size > 0 && loc2->size > 0);
        assert(loc1 != loc2);
    #endif

    key repParent;
    key nonRepParent;
    
    // TODO: xor this ?
    // Figure out which side becomes parent
    if (loc1->cell.isSource){
        if (loc2->cell.isSource){
            // Check that both are the same color
            #if DEBUG
                bool cond = h_get(board->sources, set1parent) == h_get(board->sources, set2parent);
                if (!cond){
                    printf("Col1: %d, col2: %d\n", h_get(board->sources, set1parent), h_get(board->sources, set2parent));
                    print_board(board);
                    print_parents(board);
                }
                assert(cond);
            #endif

            // Now ensure that 

            // Determine based off size
            if (loc1->size > loc2->size){
                // // l1 bigger, so add l2 to l1
                // loc2->parent = loc1Ind;
                // loc1->size = loc1->size + loc2->size;
                repParent = set1parent;
                nonRepParent = set2parent;
            } else {
                // // l2 bigger, so add l1 to l2
                // loc1->parent = loc2Ind;
                // loc2->size = loc2->size + loc1->size;
                repParent = set1parent;
                nonRepParent = set2parent;
            }
        } else {
            // // Add l2 to l1
            // loc2->parent = loc1Ind;
            // loc1->size = loc1->size + loc2->size;
            repParent = set1parent;
            nonRepParent = set2parent;
        }
    } else {
        if (loc2->cell.isSource){
            // // Add l1 to l2
            // loc1->parent = loc2Ind;
            // loc2->size = loc2->size + loc1->size;
            repParent = set2parent;
            nonRepParent = set1parent;
        } else {
            // Neither are sources - choose just by size
            if (loc1->size > loc2->size){
                    // // l1 bigger, so add l2 to l1
                    // loc2->parent = loc1Ind;
                    // loc1->size = loc1->size + loc2->size;
                    repParent = set1parent;
                    nonRepParent = set2parent;
            } else {
                // // l2 bigger, so add l1 to l2
                // loc1->parent = loc2Ind;
                // loc2->size = loc2->size + loc1->size;
                repParent = set2parent;
                nonRepParent = set1parent;
            }
        }
    }

    // I know which set to add to the other
    // Now I need to know how these are connected
    if (loc1Ind == set1parent){
        if (loc2Ind == set2parent) {
            // Adding parent to parent - will need to reverse one
            repParent = swap_set(board, repParent);
            board->cells[nonRepParent].parent = repParent;
            board->cells[repParent].tail = board->cells[nonRepParent].tail;
        } else {
            // Adding loc1 parent to loc2 tail

            // Case 2
            if (loc1Ind == repParent){
                repParent = swap_set(board, repParent);
                board->cells[nonRepParent].parent = repParent;
                board->cells[repParent].tail = nonRepParent;
            // Case 1 
            } else {
                board->cells[nonRepParent].parent = repParent;
                board->cells[repParent].tail = board->cells[nonRepParent].tail;
            }
        }
    } else {
        if (loc2Ind == set2parent) {
            // Adding loc1 tail to loc2 parent
            
            // Case 2
            if (loc2Ind == repParent){
                repParent = swap_set(board, repParent);
                board->cells[nonRepParent].parent = repParent;
                board->cells[repParent].tail = nonRepParent;
            
            // Case 1
            } else {
                board->cells[nonRepParent].parent = repParent;
                board->cells[repParent].tail = board->cells[nonRepParent].tail;
            }

        } else {
            // Adding tail to tail - will need to reverse one
            board->cells[nonRepParent].parent = repParent;
            board->cells[repParent].tail = nonRepParent;
        } 
    }
}

// Retrieves cell state from the board
cellstate_t get_cell(board_t* board, int r, int c){
    cellstate_t cs;
    int ind = get_index(board->width, r, c);
    cs.node = board->cells[ind].cell;

    // If it's empty, double check parent
    if (cs.node.col == EMPTY && cs.node.isOccupied == true){
        key parentInd = get_parent(board, ind);
        enum COLOR parentCol = board->cells[parentInd].cell.col;
        board->cells[ind].cell.col = parentCol;
        cs.node.col = parentCol;
    }

    // T-shaped access pattern
    cs.edges.n = bitmap_get(board->edges, c + 1, 2 * r);
    cs.edges.e = bitmap_get(board->edges, c + 1, 2 * r + 1);
    cs.edges.s = bitmap_get(board->edges, c + 1, 2 * r + 2);
    cs.edges.w = bitmap_get(board->edges, c, 2 * r + 1);

    return cs;
}

// TODO: allocate board in one contiguous chunk

// Creates a board from an unsolved board
board_t* create_board(unsolved_board_t b) {
    // Extract struct values
    int w = b.width;
    int h = b.height;
    int n = b.n;

    // Allocate space for board
    board_t* board = malloc(sizeof(board_t));
    board->height = h;
    board->width = w;
    board->n = n;
    
    // Allocate space for data
    board->cells = calloc(h * w, sizeof(boardset_t));
    
    // Create edge bitmap
    board->edges = bitmap_create(w + 1, 2 * h + 1);
    bitmap_initialize(board->edges);
    
    // Allocate space for source hash table
    board->sources = h_create(2 * n);

    // Allocate space for incorrect moves
    board->incorrect_moves = bitmap_create(6 * w, h);

    // Mark sources as sources
    for (int i = 0; i < n; i++){
        int r1 = b.positions[4 * i];
        int c1 = b.positions[4 * i + 1];
        int r2 = b.positions[4 * i + 2];
        int c2 = b.positions[4 * i + 3];

        int s1 = get_index(board->width, r1, c1);
        int s2 = get_index(board->width, r2, c2);

        // Add to sources hash table
        h_add(board->sources, s1, i + 1);
        h_add(board->sources, s2, i + 1);

        // Modify cell state
        board->cells[s1].cell.isSource = true;
        board->cells[s1].cell.canAppend = true;
        board->cells[s1].cell.col = i + 1;

        board->cells[s2].cell.isSource = true;
        board->cells[s2].cell.canAppend = true;
        board->cells[s2].cell.col = i + 1;
    }

    // Maybe I could add valid-corner bitmap? Or just add into CELL somewhere
    // A source can only have up to 2 corners - never in opposite directions - can I leverage this?

    return board;
}

// Deletes a board
void destroy_board(board_t* board) {
    bitmap_destroy(board->edges);
    bitmap_destroy(board->incorrect_moves);
    free(board->cells);
    h_destroy(board->sources);
    free(board);
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

// Nicely prints a board
void print_board(board_t* board) {
    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            cellstate_t cell = get_cell(board, r, c);
            int parent = get_parent(board, get_index(board->width, r, c));
            char* segment = "  ";
            
            // Determine which piece of the line to draw
            if (cell.node.isOccupied){
                if (cell.edges.n) {
                    if (cell.edges.e) {
                        segment = " ┗";
                    } else if (cell.edges.w) {
                        segment = "━┛";
                    } else if (cell.edges.s){
                        segment = " ┃";
                    } else { // North source
                        segment = "n⏺";
                    }
                } else if (cell.edges.e) {
                    if (cell.edges.w) {
                        segment = "━━";
                    } else if (cell.edges.s) {
                        segment = " ┏";
                    } else { // East source
                        segment = "e⏺";
                    }
                } else if (cell.edges.w) {
                    if (cell.edges.s){
                        segment = "━┓";
                    } else { // west source
                        segment = "w⏺";
                    }
                } else { // south source
                    segment = "s⏺";
                }
                // Determine line color
                set_text_color(board->cells[parent].cell.col, true);
            }
            
            // Determine which color background should be
            set_text_color(cell.node.col, false);
            printf("%s", segment);
            reset_text_color();
        }
        printf("\n");
    }
    printf("\n");
}

// Various ways to print board info
void print_edgecount(board_t* board){
   for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            cellstate_t cell = get_cell(board, r, c);
            printf("%d", cell.edges.n + cell.edges.s + cell.edges.e + cell.edges.w);
        }
        printf("\n");
    }
    printf("\n");
}
void print_sourcemap(board_t* board){
   for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            cellstate_t cell = get_cell(board, r, c);
            printf("%d", cell.node.isSource);
        }
        printf("\n");
    }
    printf("\n");
}
void print_parents(board_t* board){
   for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            int ind = get_index(board->width, r, c);
            if (board->cells[ind].cell.isOccupied){
                // printf("%2d ", board->cells[ind].parent);
                printf("(%2d, %2d) ", board->cells[ind].parent, board->cells[ind].tail);
                // printf("%2d ", get_parent(board, ind));
            } else printf("         ");
        }
        printf("\n");
    }
    printf("\n");
}


// Modifies board by performing move
// Returns false if an error occurs.
void perform_move(board_t* board, move_t move) {
    // Destruct
    int r = move.r;
    int c = move.c;
    int w = board->width;

    // Remove any edges that are not from or to?
    if (move.from != NORTH && move.to != NORTH){
        bitmap_set(board->edges, c + 1, 2 * r, 0);
    }
    if (move.from != SOUTH && move.to != SOUTH){
        bitmap_set(board->edges, c + 1, 2 * r + 2, 0);
    }
    if (move.from != EAST && move.to != EAST){
        bitmap_set(board->edges, c + 1, 2 * r + 1, 0);
    }
    if (move.from != WEST && move.to != WEST){
        bitmap_set(board->edges, c, 2 * r + 1, 0);
    }

    // Mark the current node as occupied
    int ind = get_index(w, r, c);
    board->cells[ind].cell.isOccupied = true;

    // Create a set for this point
    make_set(board, r, c);

    int fromInd;
    // Get toInd index
    switch (move.from) {
        case NORTH:
            fromInd = ind - w;
            break;
        case SOUTH:
            fromInd = ind + w;
            break;
        case EAST:
            fromInd = ind + 1;
            break;
        case WEST:
            fromInd = ind - 1;
            break;
    }

    // Combines sets when applicable
    board->cells[fromInd].cell.canAppend = true;
    if (board->cells[fromInd].cell.isOccupied) {
        board_union(board, ind, fromInd);
    } else if (board->cells[ind].cell.col != EMPTY){
        board->cells[fromInd].cell.col = board->cells[ind].cell.col;
    }

    int toInd;
    // Get toInd index
    switch (move.to) {
        case NORTH:
            toInd = ind - board->width;
            break;
        case SOUTH:
            toInd = ind + board->width;
            break;
        case EAST:
            toInd = ind + 1;
            break;
        case WEST:
            toInd = ind - 1;
            break;
    }

    // Prevent sources from double adding
    if (fromInd != toInd){
        // Combines sets when applicable
        board->cells[toInd].cell.canAppend = true;
        if (board->cells[toInd].cell.isOccupied) {
            board_union(board, ind, toInd);
        } else if (board->cells[ind].cell.col != EMPTY){
            board->cells[toInd].cell.col = board->cells[ind].cell.col;
        }
    }
    
    // Ensure parent and tail point to same color spaces
    key parent = get_parent(board, ind);
    key tail = board->cells[parent].tail;

    // TODO: Optimize getting edges
    cellstate_t tailCell = get_cell(board, tail / w, tail % w);
    cellstate_t parentCell = get_cell(board, parent / w, parent % w);

    if (tailCell.edges.n && board->cells[tail - w].cell.col == EMPTY) board->cells[tail - w].cell.col = tailCell.node.col;
    if (tailCell.edges.e && board->cells[tail + 1].cell.col == EMPTY) board->cells[tail + 1].cell.col = tailCell.node.col;
    if (tailCell.edges.s && board->cells[tail + w].cell.col == EMPTY) board->cells[tail + w].cell.col = tailCell.node.col;
    if (tailCell.edges.w && board->cells[tail - 1].cell.col == EMPTY) board->cells[tail - 1].cell.col = tailCell.node.col;

    if (!parentCell.node.isSource){
        if (parentCell.edges.n && board->cells[parent - w].cell.col == EMPTY) board->cells[parent - w].cell.col = parentCell.node.col;
        if (parentCell.edges.e && board->cells[parent + 1].cell.col == EMPTY) board->cells[parent + 1].cell.col = parentCell.node.col;
        if (parentCell.edges.s && board->cells[parent + w].cell.col == EMPTY) board->cells[parent + w].cell.col = parentCell.node.col;
        if (parentCell.edges.w && board->cells[parent - 1].cell.col == EMPTY) board->cells[parent - 1].cell.col = parentCell.node.col;
    }

    bool wasEmpty = false;
    
    // This is an empty set. Check if pointing to a colored space
    if (parentCell.node.col == EMPTY) {
        wasEmpty = true;
        if (parentCell.edges.n && !board->cells[parent - w].cell.isOccupied) board->cells[parent].cell.col = board->cells[parent - w].cell.col;
        if (parentCell.edges.e && !board->cells[parent + 1].cell.isOccupied) board->cells[parent].cell.col = board->cells[parent + 1].cell.col;
        if (parentCell.edges.s && !board->cells[parent + w].cell.isOccupied) board->cells[parent].cell.col = board->cells[parent + w].cell.col;
        if (parentCell.edges.w && !board->cells[parent - 1].cell.isOccupied) board->cells[parent].cell.col = board->cells[parent - 1].cell.col;
    }
    
    // if still empty, check tail
    if (parentCell.node.col == EMPTY) {
        if (tailCell.edges.n && !board->cells[tail - w].cell.isOccupied) board->cells[parent].cell.col = board->cells[tail - w].cell.col;
        if (tailCell.edges.e && !board->cells[tail + 1].cell.isOccupied) board->cells[parent].cell.col = board->cells[tail + 1].cell.col;
        if (tailCell.edges.s && !board->cells[tail + w].cell.isOccupied) board->cells[parent].cell.col = board->cells[tail + w].cell.col;
        if (tailCell.edges.w && !board->cells[tail - 1].cell.isOccupied) board->cells[parent].cell.col = board->cells[tail - 1].cell.col;
    }

    // The set was pointing to
    if (wasEmpty && board->cells[parent].cell.col != EMPTY) {

    }
}

void copy_board(board_t* old, board_t* new) {
    // Extract struct values
    int w = old->width;
    int h = old->height;
    int n = old->n;

    new->height = h;
    new->width = w;
    new->n = n;

    // Allocate space for data (no calloc since we overwrite)
    new->cells = malloc(h * w * sizeof(boardset_t));
    
    // Create edge bitmap
    new->edges = bitmap_create(w + 1, 2 * h + 1);
    
    // Allocate space for source hash table
    new->sources = h_create(2 * n);

    // Allocate space for incorrect moves
    new->incorrect_moves = bitmap_create(6 * w, h);

    // Mark sources as sources
    for (int i = 0; i < 2 * n; i++){
        key s1 = old->sources->keys[i];
        item i1 = old->sources->items[i];
        
        // Add to sources hash table
        h_add(new->sources, s1, i1);

        // Modify cell state
        new->cells[s1].cell.isSource = true;
        new->cells[s1].cell.canAppend = true;
        new->cells[s1].cell.col = i + 1;
    }

    // Set edges
    for (int r = 0; r < 2 * h + 1; r++){
        for (int c = 0; c < w + 1; c++){
            bitmap_set(new->edges, c, r, bitmap_get(old->edges, c, r));
        }
    }

    // Set sources
    for (int r = 0; r < h; r++){
        for (int c = 0; c < w; c++){
            boardset_t* new_cell = new->cells + get_index(w, r, c);
            boardset_t* old_cell = old->cells + get_index(w, r, c);
            *new_cell = *old_cell;
        }
    }

    // Set incorrect moves
    for (int r = 0; r < h; r++){
        for (int c = 0; c < 6 * w; c++){
            bitmap_set(new->incorrect_moves, c, r, bitmap_get(old->incorrect_moves, c, r));
        }
    }

    #if DEBUG
        assert(new->width == old->width);
        assert(new->height == old->height);
        assert(new->n == old->n);
        
        // Not the most efficient way of checking equality, but it's realistic
        for (int r = 0; r < h; r++){
            for (int c = 0; c < w; c++){
                cellstate_t old_cell = get_cell(old, r, c);
                cellstate_t new_cell = get_cell(new, r, c);

                // Check edges
                assert(old_cell.edges.n == new_cell.edges.n);
                assert(old_cell.edges.e == new_cell.edges.e);
                assert(old_cell.edges.s == new_cell.edges.s);
                assert(old_cell.edges.w == new_cell.edges.w);

                // Check node
                assert(old_cell.node.canAppend == new_cell.node.canAppend);
                assert(old_cell.node.col == new_cell.node.col);
                assert(old_cell.node.isOccupied == new_cell.node.isOccupied);
                assert(old_cell.node.isSource == new_cell.node.isSource);
            }
        }
    #endif
}

int get_move_hash(move_t move) {
    // Calculate the offset
    bool n = move.from == NORTH || move.to == NORTH;
    bool e = move.from == EAST || move.to == EAST;
    bool s = move.from == SOUTH || move.to == SOUTH;
    bool w = move.from == WEST || move.to == WEST;

    // Ensures all moves for a given position are distinct.
    // Sources also end up with different offsets for each direction
    bool four = n && !w;
    bool two = (n && w) || (!n & s & !w);
    bool one = e;

    // turn offset into int in [0, 6)
    int offset = (four << 2) + (two << 1) + one;

    #if DEBUG
        assert(0 <= offset && offset < 6);
    #endif

    return offset;
}

// Adds a move to the incorrect moves bitmap
void add_incorrect(board_t* board, move_t move) {
    int offset = get_move_hash(move);
    
    bitmap_set(board->incorrect_moves, 6 * move.c + offset, move.r, 1);
}

// Checks if a move is incorrect
bool move_is_incorrect(board_t* board, move_t move) {
    int offset = get_move_hash(move);
    return bitmap_get(board->incorrect_moves, 6 * move.c + offset, move.r);
}
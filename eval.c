#include "eval.h"

// TODO: Remove is_valid checks for locations here - I'm
//       checking valid edges, so I should always be covered.
// TODO: Split into 2 files:
//      eval_board.c
//      eval_moves.c
// TODO: Add Blackspace analysis


///////////////////////////////
// Functions for visited set //
///////////////////////////////

/*
    Checks if loc is visited
*/
bool is_location_visited(dfs_visited_t visited, board_location_t loc){
    int ind = get_loc_index(loc);
    return (visited[ind / 32]) & (1 << (ind % 32));
}

/*
    Sets loc as visited
*/
void set_location_visited(dfs_visited_t visited, board_location_t loc){
    int ind = get_loc_index(loc);
    visited[ind / 32] |= (1 << (ind % 32));
}

/*
    Creates a visited set
*/
dfs_visited_t create_visited(int width, int height){
    int area = width * height;

    dfs_visited_t visited = calloc(1 + (area / 32), sizeof(uint32_t));

    return visited;
}

/*
    Destroys a visited set
*/
void destroy_visited(dfs_visited_t visted){
    free(visted);
}

/*
    Performs bfs on board from loc to the end source node of col on board.
    Stores the shortest path in line, and the length of the line in line_end
*/
bool bfs(board_t* board, dfs_visited_t visited, board_location_t loc, enum COLOR col, line_t line, int* line_end){
    // The end goal
    board_location_t end = get_end_source(board, col);

    // queue for all indices
    board_location_t queue[board->width * board->height];

    // Keeps track of previous node
    board_location_t previous[board->width * board->height];

    queue[0] = loc;
    previous[get_loc_index(loc)] = NO_LOC;

    int curr_ind = 0;
    int end_ind = 1;

    set_location_visited(visited, loc);

    bool result = false;


    while(curr_ind != end_ind){
        board_node_t* curr_node = get_node_at_loc(board, queue[curr_ind]);
                
        // Node is saved for another color
        if (node_guaranteed(curr_node) != EMPTY && 
            node_guaranteed(curr_node) != col){
            curr_ind++;
            continue;
        }
        
        // Success?
        if (are_locs_adjacent(board, queue[curr_ind], end) || get_loc_index(queue[curr_ind]) == get_loc_index(end)){
            result = true;
            break;
        }

        // Add children
        board_location_t up = get_loc_up(queue[curr_ind]);
        if (loc_is_valid(up) && !is_location_visited(visited, up) && 
            get_node_up_edge(curr_node)){

            queue[end_ind++] = up;
            set_location_visited(visited, up);
            previous[get_loc_index(up)] = queue[curr_ind];
        }
        
        // Since i'm checking that the down edge exists, I prolly
        // don't need to verify that it's valid
        board_location_t down = get_loc_down(queue[curr_ind]);
        if (loc_is_valid(down) && !is_location_visited(visited, down) && 
            get_node_down_edge(curr_node)){

            queue[end_ind++] = down;
            set_location_visited(visited, down);
            previous[get_loc_index(down)] = queue[curr_ind];
        }

        board_location_t left = get_loc_left(queue[curr_ind]);
        if (loc_is_valid(left) && !is_location_visited(visited, left) && 
            get_node_left_edge(curr_node)){

            queue[end_ind++] = left;
            set_location_visited(visited, left);
            previous[get_loc_index(left)] = queue[curr_ind];
        }

        board_location_t right = get_loc_right(queue[curr_ind]);
        if (loc_is_valid(right) && !is_location_visited(visited, right) 
            && get_node_right_edge(curr_node)){

            queue[end_ind++] = right;
            set_location_visited(visited, right);
            previous[get_loc_index(right)] = queue[curr_ind];
        }
        curr_ind++;
    }
    

    board_location_t line_curr = queue[curr_ind];
    int line_ind = 0;
    
    // Only calculate the line if we find the end node (effectively if (while(true)))
    while (result){
        line[line_ind++] = line_curr;
        line_curr = previous[get_loc_index(line_curr)];


        // original index previous is NO_LOC
        if (!loc_is_valid(line_curr)){
            break;
        }
    }

    // give line length
    *line_end = line_ind;

    return result;
}

// does dfs on board until it finds the end with color col
// Always assume the goal is the end source node
bool dfs(board_t* board, dfs_visited_t visited, board_location_t loc, enum COLOR col){
    // The end goal
    board_location_t end = get_end_source(board, col);

    board_node_t* curr_node = get_node_at_loc(board, loc);

    // If node is for another color, do not use
    if (node_guaranteed(curr_node) != EMPTY && 
        node_guaranteed(curr_node) != col) return false;
    
    if (are_locs_adjacent(board, loc, end)) return true;

    // Otherwise, set this location as visited
    set_location_visited(visited, loc);

    // And checks the adjacent locations (if edge exists and not visited)
    // Also added short circuiting
    board_location_t up = get_loc_up(loc);
    if (loc_is_valid(up) && !is_location_visited(visited, up) && 
        get_node_up_edge(curr_node)){
        if (dfs(board, visited, up, col)) return true;
    }

    board_location_t down = get_loc_down(loc);
    if (loc_is_valid(down) && !is_location_visited(visited, down) && 
        get_node_down_edge(curr_node)){
        if (dfs(board, visited, down, col)) return true;
    }

    board_location_t left = get_loc_left(loc);
    if (loc_is_valid(left) && !is_location_visited(visited, left) && 
        get_node_left_edge(curr_node)){
        if (dfs(board, visited, left, col)) return true;
    }

    board_location_t right = get_loc_right(loc);
    if (loc_is_valid(right) && !is_location_visited(visited, right) 
        && get_node_right_edge(curr_node)){
        if (dfs(board, visited, right, col)) return true;
    }

    return false;
}

/*
    Mark all bottlenecks on the line. Iterate forwards thru the list 
*/
int mark_bottlenecks(board_t* board, enum COLOR col, line_t line, int len){
    board_location_t end = get_end_source(board, col);
    board_location_t start = get_start_source(board, col);

    int marked = 0;

    for (int i = 0; i < len - 1; i++){
        dfs_visited_t visited = create_visited(board->width, board->height);

        // Mark position as visited
        set_location_visited(visited, line[i]);

        // Node is necessary, mark it as a guaranteed space
        if (!dfs(board, visited, start, col)){
            enum COLOR curr_col = node_guaranteed(get_node_at_loc(board, line[i]));
            
            // Node is a guaranteed space already
            if (!set_node_guaranteed(board, col, line[i])) return -1;
            marked += curr_col == EMPTY;
        }
        destroy_visited(visited);
    }
    return marked;
}

bool is_color_solvable(board_t* board, enum COLOR col){
    #if DEBUG
        assert(col != EMPTY);
    #endif

    dfs_visited_t visited = create_visited(board->width, board->height);

    bool result = dfs(board, visited, get_start_source(board, col), col);

    destroy_visited(visited);

    return result;
}

bool is_solvable(board_t* board){
    #if DEBUG
        printf("Board at beginning of eval:\n");
        print_board(board);
    #endif
    for (int i = RED; i < board->n; i++){
        // Don't check finished colors
        if (locs_are_equal(get_start_source(board, i), get_end_source(board, i))) continue;
        
        if(!is_color_solvable(board, i)){
            #if DEBUG
                printf("Board unsolvable! No path from %d to %d for color %d\n", get_loc_index(get_start_source(board, i)), get_loc_index(get_end_source(board, i)), i);
            #endif
            return false;
        }
    }
    #if DEBUG
        printf("Eval complete!\n");
    #endif
    return true;
}

bool is_solved(board_t* board){
    // Check that all source nodes are connected
    for (int i = RED; i < board->n; i++){
        if(!locs_are_equal(get_start_source(board, i), get_end_source(board, i))) return false;
    }

    return true;
}

// Creates a sortable move. Minimum value is 1, invalid is 0
void convert_to_sortable(move_t* old, sortable_move_t* new, int size){
    for(int i = 0; i < size; i++){
        new[i].move = old[i];
        new[i].value = 1 / (old[i].options);
    }
}

// Checks if a move will end on a guaranteed space
void check_guaranteed(sortable_move_t* s, board_node_t* curr_node){
    s->value += (node_guaranteed(curr_node) == s->move.color); // * PARAM
}

// Boosts for following walls horizontally
void check_horizontal(sortable_move_t* s, board_node_t* start_node, board_node_t* end_node){
    s->value += (!get_node_up_edge(start_node) && !get_node_up_edge(end_node));
    s->value += (!get_node_down_edge(start_node) && !get_node_down_edge(end_node));
}

// Boosts for following walls vertically
void check_vertical(sortable_move_t* s, board_node_t* start_node, board_node_t* end_node){
    s->value += (!get_node_left_edge(start_node) && !get_node_left_edge(end_node));
    s->value += (!get_node_right_edge(start_node) && !get_node_right_edge(end_node));
}

// Checks if a move will end in a corner
void check_corner(sortable_move_t* s, board_t* board, board_location_t loc){
    s->value += is_loc_corner(board, loc);
}

// Checks for 180s
bool check_loop_v(board_t* board, sortable_move_t* s, board_node_t* left, board_node_t* right, board_location_t end){
    return false;
    
    board_location_t far_left_loc = get_loc_left(end);
    board_location_t far_right_loc = get_loc_right(end);

    // Check left side
    if (loc_is_valid(far_left_loc)){
        board_node_t* far_left = get_node_at_loc(board, far_left_loc);

        if(get_node_color(left) == s->move.color && get_node_color(far_left) == get_node_color(left)) {
            s->value = 0;
            return true;
        }
    }

    // Check right side
    if (loc_is_valid(far_right_loc)){
        board_node_t* far_right = get_node_at_loc(board, far_right_loc);
        if(get_node_color(right) == s->move.color && get_node_color(far_right) == get_node_color(right)) {
            s->value = 0;
            return true;
        }
    }
    return false;
}

bool check_loop_h(board_t* board, sortable_move_t* s, board_node_t* up, board_node_t* down, board_location_t end){
    return false;
    board_location_t far_up_loc = get_loc_up(end);
    board_location_t far_down_loc = get_loc_down(end);

    // Check up side
    if (loc_is_valid(far_up_loc)){
        board_node_t* far_up = get_node_at_loc(board, get_loc_up(end));
        if(get_node_color(up) == s->move.color && get_node_color(far_up) == get_node_color(up)) {
            s->value = 0;
            return true;
        }
    }

    // Check down side
    if (loc_is_valid(far_down_loc)){
        board_node_t* far_down = get_node_at_loc(board, get_loc_down(end));
        if(get_node_color(down) == s->move.color && get_node_color(far_down) == get_node_color(down)) {
            s->value = 0;
            return true;
        }
    }
    return false;
}

// Evaluates a move and assigns it a float
// If move is invalid, returns false.
bool evaluate_move(board_t* board, sortable_move_t* s){
    // Keep track if the move is valid
    bool is_valid = true;

    // Just check if it's on a guaranteed space for now.

    enum COLOR col = s->move.color;
    board_location_t orig = s->move.is_start ? get_start_source(board, col) : get_end_source(board, col);

    board_location_t end = get_end_source(board, s->move.color);

    // Grab all relevant nodes
    board_node_t* curr = get_node_at_loc(board, orig);
    board_node_t* up = get_node_up(board, orig);
    board_node_t* down = get_node_down(board, orig);
    board_node_t* left = get_node_left(board, orig);
    board_node_t* right = get_node_right(board, orig);

    // For direction dependent things
    switch (s->move.dir) {
    case UP:{
        board_location_t up_loc = get_loc_up(orig);
        if (locs_are_equal(up_loc, end)){
            s->value = 100;
            return is_valid;
        }
        check_guaranteed(s, up);
        check_vertical(s, curr, up);
        check_corner(s, board, up_loc);
        is_valid = check_loop_v(board, s, left, right, up_loc);
    
    break;}
    case DOWN:{
        board_location_t down_loc = get_loc_down(orig);
        if (locs_are_equal(down_loc, end)){
            s->value = 100;
            return is_valid;
        }
        check_guaranteed(s, down);
        check_vertical(s, curr, down);
        check_corner(s, board, down_loc);
        is_valid = check_loop_v(board, s, left, right, down_loc); 
    break;}
    case LEFT:{
        board_location_t left_loc = get_loc_left(orig);
        if (locs_are_equal(left_loc, end)){
            s->value = 100;
            return is_valid;
        }
        check_guaranteed(s, left);
        check_horizontal(s, curr, left);
        check_corner(s, board, left_loc);
        is_valid = check_loop_h(board, s, up, down, left_loc);
    break;}
    case RIGHT:{
        board_location_t right_loc = get_loc_right(orig);
        if (locs_are_equal(right_loc, end)){
            s->value = 100;
            return is_valid;
        }
        check_guaranteed(s, right);
        check_horizontal(s, curr, right);
        check_corner(s, board, right_loc);
        is_valid = check_loop_h(board, s, up, down, right_loc);
    break;}
    }
    return is_valid;
}

// Compares 2 sortable moves
int compare_sortable(const void* s1, const void* s2){
    return (((sortable_move_t*)s1)->value > ((sortable_move_t*)s2)->value) ? -1 : ((((sortable_move_t*)s1)->value < ((sortable_move_t*)s2)->value) ? 1 : 0);
}

// Find and mark all guaranteed spaces on a board
// Returns -1 if fail. Otherwise, returns the number of moves found
int find_guaranteed_spaces(board_t* board){
    int total = 0;
    for (int col = 0; col < board->n; col++){
        // Create a space for a shortest path line
        // TODO: Find tighter bound
        line_t line = calloc(board->width * board->height, sizeof(board_location_t));
        int line_end;
        
        dfs_visited_t visited = create_visited(board->width, board->height);
        
        if (bfs(board, visited, get_start_source(board, col), col, line, &line_end)) {
            #if DEBUG
                printf("Color %d BFS Shortest path: [", col);
                for (int i = 0; i < line_end; i++){
                    // printf("%d, ", get_loc_index(line[i]));
                    printf("(%d, %d), ", get_loc_col(line[i]), get_loc_row(line[i]));
                }
                printf("]\n");
            #endif

            // Checks for bottlenecks along the shortest path
            int new_bottlenecks = mark_bottlenecks(board, col, line, line_end);
            if (new_bottlenecks == -1) return -1;
            total += new_bottlenecks;
        }
        
        free(line);
        destroy_visited(visited);
    }
    return total;
}
// Add the notion of a 'guaranteed space' on boards. If we can identify a bottleneck (or bottlenecks)
// in the board, we should assume that they can only be used for that color. This will be
// another opportunity to identify impossible states, (if >1 color has a given guaranteed space).
// There is no reason to draw lines to guaranteed spaces, since by definition, the lines must use
// them to get to their nodes. So if they ever get blocked off, the current solution will find them

// Save previously used dfs paths? See if the last path worked first, then try to find a new path
// analogous: Check the last used path for the move node. I could do a neutral dfs from ths start,
// or assume there will be another path near the node? This could be built into dfs if I start from
// the last position, but that could be exploited. Granted, if I use deterministic dfs, it should always
// find the last used path *before* any other path. Therefore, if I want to use determinism I should
// start from the end. Otherwise it's nondeterministic and who cares.

// Moves cannot be removed down the line
// This will be useful when designing/proving evaluations

// Also, you can leave the line but not reenter the line using the bits

// Convolutions are cool and all, but not necessary. I know where the move is, and 
// I can see what changed. Rethink that idea. 

// Save shortest paths as dfs_visited_types. Easy to check if the last move modified
// the line

// Time for blackspace analysis. The following is true:
// [[ Every empty space should be connected to both the start and the end of at
// least 1 color. ]]
// If there is an empty space that cannot reach the start and the end of a color,
// it is impossible to use it to create a line.
// So how do I test for this?
// I can do bfs/dfs to get a set of empty spaces that are connected
// Then I need to run dfs 2n times from any visited space in the set (prolly the first)
// to both the start and end of every color. I can end this search as soon as I find any
// color that connects to both ends

// If I'm really cracked, I could combine both searches into one. For now, I should just
// do it in 2 steps (leave some optimizations for later ;))

// Does it make sense to do 2 steps of evaluation? Not really.
// The current alg is -
//
// Get unsolved board
// while(unsolved):
//      find/do guaranteed moves
//      generate all possible moves
//      choose a move and do it in the next frame
//      quick check that it's valid
//      if invalid, try a new move
//      otherwise, move on to next frame

// Maybe I can clear up some of the confusion of what 'eval' is by splitting into
// move vs board evaluation i.e.-

// Get unsolved board
// while(unsolved):
//      Do board evaluation
//          find/do guaranteed moves
//          do blackspace analysis
//      Generate all possible moves
//      Shuffle moves
//      Do move evaluation
//          Check for impossible states/really bad moves
//          Give all moves a value
//      Sort moves
//      Iterate thru moves:
//          perform move in next frame
//          continue to next frame

// It would simplify the ending part of the alg a lot I think.
// I take it back tbh. Not that much performance benefit overall.

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
            #if DEBUG && VERBOSE
                printf("Board unsolvable! No path from %d to %d for color %d\n", get_loc_index(get_start_source(board, i)), get_loc_index(get_end_source(board, i)), i);
            #endif
            return false;
        }
    }
    #if DEBUG && VERBOSE
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
        new[i].value = 1.0 / (old[i].options);
    }
}

// Check how many backward edges the dest has to the same color. >3 = implies
// invalid. 2 is invalid if not connected to the opposite edge. 1 is expected
bool check_dest_edges_valid(board_t* board, move_t move){
    board_location_t dest = get_move_dest(board, move);

    board_location_t opposite_source = move.is_start ? \
            get_end_source(board, move.color) : get_start_source(board, move.color);
    
    board_location_t orig = move.is_start ? get_start_source(board, move.color) : get_end_source(board, move.color);

    // If we are connecting bridges, it does not matter
    if (locs_are_equal(dest, opposite_source)) return true;

    int num_same_color = 0;

    board_node_t* up_node = get_node_up(board, dest);
    if (up_node != &INVALID_NODE){
        int up = get_node_color(up_node) == move.color && \
                          get_node_down_edge(up_node) && \
                          !locs_are_equal(get_loc_up(dest), opposite_source);
        num_same_color += up;
    }
    board_node_t* down_node = get_node_down(board, dest);
    if (down_node != &INVALID_NODE){
        int c = get_node_color(down_node) == move.color && \
                          get_node_up_edge(down_node) && \
                          !locs_are_equal(get_loc_down(dest), opposite_source);
        num_same_color += c;
    }
    board_node_t* left_node = get_node_left(board, dest);
    if (left_node != &INVALID_NODE){
        int c = get_node_color(left_node) == move.color && \
                          get_node_right_edge(left_node) && \
                          !locs_are_equal(get_loc_left(dest), opposite_source);
        num_same_color += c;
    }
    board_node_t* right_node = get_node_right(board, dest);
    if (right_node != &INVALID_NODE){
        int c = get_node_color(right_node) == move.color && \
                          get_node_left_edge(right_node) && \
                          !locs_are_equal(get_loc_right(dest), opposite_source);
        num_same_color += c;
    }

    return num_same_color == 1 ? true : false;
}

// Evaluates a move and assigns it a float
// If move is invalid, returns false.
bool evaluate_move(board_t* board, sortable_move_t* s){
    bool is_valid = true;

    enum COLOR col = s->move.color;
    board_location_t orig = s->move.is_start ? get_start_source(board, col) : get_end_source(board, col);
    board_location_t dest = get_move_dest(board, s->move);

    // Check the edges to see if move is invalid
    if(!check_dest_edges_valid(board, s->move)){
        s->value = -1;
        return false;
    }

    board_node_t* dest_node = get_node_at_loc(board, dest);

    // Add value if dest has few edges (wall crawl)
    int num_edges = get_node_up_edge(dest_node) + get_node_down_edge(dest_node) + \
                    get_node_left_edge(dest_node) + get_node_right_edge(dest_node);
    s->value += (4.0 - num_edges) / 4;

    return true;
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
            #if DEBUG && VERBOSE
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
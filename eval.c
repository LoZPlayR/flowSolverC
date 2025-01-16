#include "eval.h"

///////////////////////////////
// Functions for visited set //
///////////////////////////////

/*
    Checks if loc is visited
*/
bool is_location_visited(dfs_visited_t visited, board_location_t loc){
    int ind = (int) loc;
    return (visited[ind / 32]) & (1 << (ind % 32));
}

/*
    Sets loc as visited
*/
void set_location_visited(dfs_visited_t visited, board_location_t loc){
    int ind = (int) loc;
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

// does dfs on board until it finds the end with color col
// Always assume the goal is the end source node
bool dfs(board_t* board, dfs_visited_t visited, board_location_t loc, enum COLOR col){
    // The end goal
    board_location_t end = get_end_source(board, col);

    board_node_t curr_node = get_node_at_loc(board, loc);

    if (are_locs_adjacent(board, loc, end)) return true;

    // Otherwise, set this location as visited
    set_location_visited(visited, loc);

    // And checks the adjacent locations (if edge exists and not visited)
    // Also added short circuiting
    board_location_t up = get_loc_up(board, loc);
    if (loc_is_valid(up) && !is_location_visited(visited, up) && 
        node_up_edge(curr_node)){
        if (dfs(board, visited, up, col)) return true;
    }

    board_location_t down = get_loc_down(board, loc);
    if (loc_is_valid(down) && !is_location_visited(visited, down) && 
        node_down_edge(curr_node)){
        if (dfs(board, visited, down, col)) return true;
    }

    board_location_t left = get_loc_left(board, loc);
    if (loc_is_valid(left) && !is_location_visited(visited, left) && 
        node_left_edge(curr_node)){
        if (dfs(board, visited, left, col)) return true;
    }

    board_location_t right = get_loc_right(board, loc);
    if (loc_is_valid(right) && !is_location_visited(visited, right) 
        && node_right_edge(curr_node)){
        if (dfs(board, visited, right, col)) return true;
    }

    return false;
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
    for (int i = RED; i < board->n; i++){
        // Don't check finished colors
        if (get_start_source(board, i) == get_end_source(board, i)) continue;
        
        if(!is_color_solvable(board, i)){
            #if DEBUG
                printf("Board unsolvable! No path for color %d\n", i);
            #endif
            return false;
        }
    }
    return true;
}

bool is_solved(board_t* board){
    // Check that all source nodes are connected
    for (int i = RED; i < board->n; i++){
        if (get_start_source(board, i) != get_end_source(board, i)) return false;
    }

    return true;
}
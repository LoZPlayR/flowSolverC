#include "gen_moves.h"

// New idea: each segment added is a move. Then we can try to add to any segment. Generating moves includes generating all possible moves from the current
// head of each line. 

// TODO: Make faster operations that do not require going thru the node type

int generate_moves(board_t* board, move_t* move_arr){
    // Assume move_arr is size 4n

    int total_moves = 0;

    // Should iterate thru all colors
    for (int i = RED; i < board->n; i++){
        board_location_t start_loc = get_start_source(board, i);
        board_location_t end_loc = get_end_source(board, i);

        // Check that this color is incomplete
        if (start_loc == end_loc) continue;

        // Grab relevant nodes
        board_node_t start_node = get_node_at_loc(board, start_loc);
        board_node_t end_node = get_node_at_loc(board, end_loc);

        // Always bridge if given the option - this will work until I add walls since bridging ignores them
        // Check if bridges are possible
        if (are_locs_adjacent(board, start_loc, end_loc)) {
            move_arr[total_moves].color = i;
            move_arr[total_moves].dir = get_dir_between_nodes(board, start_loc, end_loc);
            move_arr[total_moves].is_start = true;
            total_moves++;
        }

        // Check all directions and add to moves if we can travel there
        if (node_up_edge(start_node)){
            move_arr[total_moves].color = i;
            move_arr[total_moves].dir = UP;
            move_arr[total_moves].is_start = true;
            total_moves++;
        }

        if (node_down_edge(start_node)){
            move_arr[total_moves].color = i;
            move_arr[total_moves].dir = DOWN;
            move_arr[total_moves].is_start = true;
            total_moves++;
        }

        if (node_left_edge(start_node)){
            move_arr[total_moves].color = i;
            move_arr[total_moves].dir = LEFT;
            move_arr[total_moves].is_start = true;
            total_moves++;
        }

        if (node_right_edge(start_node)){
            move_arr[total_moves].color = i;
            move_arr[total_moves].dir = RIGHT;
            move_arr[total_moves].is_start = true;
            total_moves++;
        }
    }

    return total_moves;
}
    
// Assume 2 boards that are already initialized/allocated. 
// We will copy src to dst, and then perform move on dst     
void perform_move(board_t* src, board_t* dst, move_t move){
    // copies src to dst
    copy_board(src, dst);

    board_location_t start = get_start_source(src, move.color);

    board_location_t end;

    switch (move.dir)
    {
    case UP:
        end = get_loc_up(dst, start);
    break;
    case DOWN:
        end = get_loc_down(dst, start);
    break;
    case LEFT:
        end = get_loc_left(dst, start);
    break;
    case RIGHT:
        end = get_loc_right(dst, start);
    break;
    default:
        // should never happen
        assert(false);
        break;
    }

    // update sources
    update_source(dst, move.color, move.is_start, end);

    set_node_color(dst, end, move.color);

    // Update unusable edges
    remove_edges_into_node(dst, end);
}

// Nicely prints out moves
// TODO: Make this not use move pointers
void print_move(move_t* move, int width){
    printf("Extend color %d ", move->color);

    switch (move->dir)
    {
    case DOWN:
        printf("downwards\n");
    break;
    case UP:
        printf("upwards\n");
    break;
    case LEFT:
        printf("to the left\n");
    break;
    case RIGHT:
        printf("to the right\n");
    break;
    default:
        printf("This should be impossible??\n");
    break;
    }
}

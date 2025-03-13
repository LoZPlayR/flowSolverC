#include "solver.h"

const int MAX_DEPTH = -1;

// TODO: Add hashing and some sort of caching system for flow boards
// TODO: Make Solver more organized? Add some more functions
// TOOD: Only make the boards that will be solved

/*
    Should solve the unsolved board. Print out the solution
    Returns the number of frames completed
*/
int solve(unsolved_board_t b){
    position_t* stack = calloc(b.height * b.width, sizeof(position_t));
    move_t* move_buffer = calloc(b.n * 8, sizeof(move_t));

    // Set up stack 
    for (int i = 0; i < b.height * b.width; i++){      
        stack[i].board = NULL;
        stack[i].num_moves = -1;
        
        // Allocate all space for moves
        stack[i].moves = malloc(sizeof(sortable_move_t) * b.n * 8);
        
        // Allocate all boards (will be modified in the future)
        stack[i].board = malloc(sizeof(board_t));
        create_board(b, stack[i].board);
    }
    
    // Print the original board
    #if (DISPLAY_TYPE == 1)
        print_board(stack[0].board);
    #endif

    // Prepare for recursion
    int depth = 0;
    int count = 0;

    while(true) {
        count += 1;
        #if DEBUG
            if (MAX_DEPTH != -1 && count > MAX_DEPTH) break;
        #endif

        bool board_is_valid = true;

        // Find all guaranteed moves for this frame
        while(board_is_valid){
            int guaranteed_found = find_guaranteed_spaces(stack[depth].board);
            
            // Found a contradiction - something is wrong
            if (guaranteed_found == -1){
                stack[depth].num_moves = -1;
                depth--;
                board_is_valid = false;
            }
            
            // All moves found
            if (guaranteed_found == 0){
                break;
            }
        }
        // printf("After finding g moves:\n");
        // if (board_is_valid) print_board(stack[depth].board);
        
        // Perform all guaranteed moves for this frame
        while(board_is_valid){
            // Use move buffer
            int found_moves = generate_guaranteed_moves(stack[depth].board, move_buffer);

            // No more guaranteed moves!
            if (found_moves == 0){
                break;
            }

            for (int i = 0; i < found_moves; i++){
                perform_local_move(stack[depth].board, move_buffer[i]);
            }
        }
        // printf("After doing g moves:\n");
        // if (board_is_valid) print_board(stack[depth].board);

        // Check if the board is full/solved
        if (is_solved(stack[depth].board)) {
            // Solved!
            break;
        }

        // Generate moves if at new depth. Set up stack frame
        if (stack[depth].num_moves == -1){
            #if DEBUG
                printf("Went down a level\n");
            #endif
            stack[depth].curr = 0;
            stack[depth].num_moves = generate_moves(stack[depth].board, move_buffer);

            // TODO: Shuffle moves?

            // Evaluate moves
            // Remove invalid moves from being done

            convert_to_sortable(move_buffer, stack[depth].moves, stack[depth].num_moves);
            int num_invalid = 0;

            for (int i = 0; i < stack[depth].num_moves; i++){
                num_invalid += evaluate_move(stack[depth].board, stack[depth].moves + i);
            }

            // Sort the entire set of moves - invalid ones should go to end
            qsort(stack[depth].moves, stack[depth].num_moves, \
                    sizeof(sortable_move_t), compare_sortable);

            // Remove the moves
            //stack[depth].num_moves -= num_invalid;
            
            #if DEBUG
                printf("Generated %d moves at depth %d\n", stack[depth].num_moves, depth);
                for (int i = 0; i < stack[depth].num_moves; i++){
                    print_move(&(stack[depth].moves[i].move), stack[depth].board->width);
                    printf("Value: %f\n", stack[depth].moves[i].value);
                }
            #endif
        }

        // Iterate thru moves until a valid one is found
        while(true){
            // Check if we need to move up in the next frame
            if (stack[depth].curr == stack[depth].num_moves){
                #if DEBUG
                    printf("Out of moves! Moving up to level %d...\n", depth);
                #endif
                if (depth == 0){
                    printf("No solution! Or serious error??\n");
                    return -1;
                }

                stack[depth].num_moves = -1;
                depth--;
                break;
            // Node has some more moves left - perform the current move
            } else {
                #if DEBUG
                    printf("Performing move (%d)\n", stack[depth].curr);
                    print_move(&stack[depth].moves[stack[depth].curr].move, stack[depth].board->width);
                #endif
                
                perform_move(stack[depth].board, stack[depth + 1].board, stack[depth].moves[stack[depth].curr].move);

                #if DISPLAY_TYPE == 2
                    printf("\e[1;1H\e[2J");
                #endif
                #if DISPLAY_TYPE > 1
                    print_board(stack[depth + 1].board);
                #endif
                
                stack[depth].curr++;

                // Fast pruning - only check if there are paths available
                if (is_solvable(stack[depth + 1].board)){
                    #if DEBUG
                        printf("Increasing depth\n");
                    #endif
                    depth++;
                    break;
                }
            }
        }
    }

    // Print solved board
    #if DISPLAY_TYPE == 1 || DISPLAY_TYPE == 3  
        print_board(stack[depth].board);
    #endif

    // Clean up everyting
    for (int i = 0; i < b.height * b.width; i++){
        free(stack[i].moves);
        destroy_board(stack[i].board);
    }

    free(move_buffer);
    free(stack);

    return count;
}



/*  Some notes:
    The max stack size is the area of the board

    Should prolly move into Windows/linux environment so I can actually debug/ASAN
 */
int main(int argc, char **argv){
    printf("Starting Solver\n");
    
    char buf[32];
    char *filename = (char *) buf;

    int min_ind = 0;
    int max_ind = 0;

    // Handle parameters
    switch (argc){
        case 4: // Maximum end pos found - use that
            max_ind = strtol(argv[3], NULL, max_ind);
        case 3: // Minimum start pos found - use that
            min_ind = strtol(argv[2], NULL, min_ind);
        case 2: // Levelpack specified - use that
            filename = argv[1];
        break;
        default: // Invalid - use classic
            filename = "classic";
        break;
    }


    // Load level
    level_pack_t* lp = malloc(sizeof(level_pack_t));
    // printf("Loading levelpack\n");
    if (!load_level_pack(lp, filename)){
        // printf("Could not file packs/%s.flow\n", filename);
        return -1;
    }
    
    // Free all the int coordinates.
    // I should just make a destroy function for levelpacks

    max_ind = (max_ind == 0 || max_ind > lp->num_levels) ? lp->num_levels : max_ind;

    // Record all data
    run_data_t all_data[max_ind]; 
    clock_t start, end;

    for (int i = min_ind; i < max_ind; i++){
        printf("solving board %d\n", i);
        // Measure time to solve
        start = clock();
        int frames = solve(lp->levels[i]);
        end = clock();

        all_data[i].filename = filename;
        all_data[i].level = i;
        all_data[i].time = ((double) (end - start));

        printf("Completed level %d in %f seconds. (%d frames)\n", i, ((double) (end - start)) / CLOCKS_PER_SEC, frames);
    }

    // Free everything
    // A create function couldn't hurt either
    for (int i = 0; i < lp->num_levels; i++){
        free(lp->levels[i].positions);
    }

    free(lp->levels);
    free(lp);

    printf("Done!\n");
}
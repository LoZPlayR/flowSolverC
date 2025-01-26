#include "solver.h"

const int MAX_DEPTH = 25;

/*
    Should solve the unsolved board. Print out the solution
    Returns the number of frames completed
*/
int solve(unsolved_board_t b){
    position_t* stack = calloc(b.height * b.width, sizeof(position_t));

    // Set up stack 
    for (int i = 0; i < b.height * b.width; i++){      
        stack[i].board = NULL;
        stack[i].num_moves = -1;
        
        // Allocate all space for moves (First move can have 4n, all others 3n)
        stack[i].moves = malloc(sizeof(move_t) * b.n * (i != 0 ? 3 : 4));
        
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

        // Check if the board is full/solved
        if (depth >= b.height * b.width - b.n && is_solved(stack[depth].board)) {
            // Solved!
            break;
        }

        // Generate moves if at new depth. Set up stack frame
        if (stack[depth].num_moves == -1){
            #if DEBUG
                printf("Went down a level\n");
            #endif
            stack[depth].curr = 0;
            stack[depth].num_moves = generate_moves(stack[depth].board, stack[depth].moves);

            // ORDER MOVES HERE? Pre-evaluate?
            
            #if DEBUG
                printf("Generated %d moves at depth %d\n", stack[depth].num_moves, depth);
                for (int i = 0; i < stack[depth].num_moves; i++){
                    print_move(&stack[depth].moves[i], stack[depth].board->width);
                }
            #endif
        }

        // Iterate thru moves until a valid one is found
        while(true){
            // Check if we need to move up in the next frame
            if (stack[depth].curr == stack[depth].num_moves){
                #if DEBUG
                    printf("Out of moves! Moving up a level...\n");
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
                    print_move(&stack[depth].moves[stack[depth].curr], stack[depth].board->width);
                #endif
                
                perform_move(stack[depth].board, stack[depth + 1].board, stack[depth].moves[stack[depth].curr]);

                #if DISPLAY_TYPE == 2
                    printf("\e[1;1H\e[2J");
                    print_board(stack[depth + 1].board);
                #endif
                
                stack[depth].curr++;

                // Prune
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
    #if DISPLAY_TYPE == 1
        print_board(stack[depth].board);
    #endif

    // Clean up everyting
    for (int i = 0; i < b.height * b.width; i++){
        free(stack[i].moves);
        destroy_board(stack[i].board);
    }

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
    int max_ind = -1;

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

    max_ind = (max_ind == -1) ? lp->num_levels : max_ind;

    // Record all data
    run_data_t all_data[lp->num_levels]; 
    clock_t start, end;

    for (int i = min_ind; i < lp->num_levels; i++){
        printf("solving board %d\n", i);
        // Measure time to solve
        start = clock();
        solve(lp->levels[i]);
        end = clock();

        all_data[i].filename = filename;
        all_data[i].level = i;
        all_data[i].time = ((double) (end - start));

        printf("Completed level %d in %f seconds\n", i, ((double) (end - start)) / CLOCKS_PER_SEC);
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
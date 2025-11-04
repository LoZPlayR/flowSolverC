#include "solver.h"

// TODO: Make logic work/identify bad boards?
// Maybe make eval step after running a guess step?

/// @brief Mutates board by performing all possible guaranteed moves
/// @param board Board object
/// @param move_buffer Buffer for storing moves
/// @return Number of moves performed (-1 if board is invalid)
int logic_module(board_t* board, move_t* move_buffer){
    int num_changes = 0;
    
    while (true){
        // prepare board
        prune_edges(board); 
        corner_prune(board);
    
        int num_moves = logic_gen_complete(board, move_buffer);
        
        // main loop (for now)
        while (num_moves){
            for (int i = 0; i < num_moves; i++){
                move_t curr_move = move_buffer[i];
                
                // Ensure the move is valid
                if (!validate_move(board, curr_move)) return -1;

                perform_move(board, curr_move);
    
                // Check for correctness
                if (!validate_area(board, curr_move.r, curr_move.c)) return -1;

                // Spot clean
                prune_edges_area(board, curr_move.r, curr_move.c);
                inverse_corner_prune(board, curr_move.r, curr_move.c);
                
                // Check for correctness
                if (!validate_area(board, curr_move.r, curr_move.c)) return -1;
            }
    
            // Clean the whole board
            // May become unnecessary when I implement spot cleaning @ tails
            prune_edges(board); 
            corner_prune(board);
    
            // Revalidate entire board
            if (!valid_edges(board) || !valid_colors(board)) return -1;
    
            // continue the loop
            num_changes += num_moves;
            num_moves = logic_gen_complete(board, move_buffer);
        }

        int guaranteedCount = guaranteed_spaces(board);
        if (guaranteedCount == -1) return -1;

        num_changes += guaranteedCount;

        if (guaranteedCount == 0) break;
    }

    // Ensure there's still paths
    if (!validate_paths(board)) return -1;

    // last check to ensure board is valid
    if (!(valid_edges(board) && valid_colors(board))) return -1;
    return num_changes;
}

// Tests a move 
bool test_move(board_t* board, move_t* move){
    board_t* test = malloc(sizeof(board_t));
    move_t* buffer = malloc(sizeof(move_t) * board->height * board->width);

    copy_board(board, test);
    perform_move(test, *move);

    // Attempt logic module
    int info = logic_module(test, buffer);
    bool success = !(info == -1);

    move->info = info;

    free(test);
    free(buffer);

    return success;
}

int MAX_FRAMES = 0;

int solve(unsolved_board_t b, int num){
    int frames = 0;
    int skips = 0;
    int invalid = 0;
    position_t* stack = malloc(sizeof(stack_t) * b.height * b.width);
    
    // prep stack
    for (int i = 0; i < b.height * b.width; i++){
        stack[i].num_moves = -1;
    }

    int currPosInd = 0;
    stack[0].board = create_board(b);

    while(currPosInd != -1){
        position_t* currPos = stack + currPosInd;
        
        if (MAX_FRAMES > 0 && frames >= MAX_FRAMES) {
            break;
        }
        if (currPosInd == -1){
            printf("No solution?\n");
            break;
        }
        
        #if DISPLAY_TYPE > 1
            #if DISPLAY_TYPE == 2
                if (frames % (1 << DISPLAY_SPEED) == 0){
                    system("clear");
                    printf("Solving board %d\n", num);
                    print_board(currPos->board);
                    printf("Total Moves Skipped: %d\n", skips);
                    printf("Total boards invalidated: %d\n", invalid);
                }
            #else
                print_board(currPos->board);
            #endif
        #endif

        // set up board
        if (currPos->num_moves == -1){
            currPos->moves = malloc(b.height * b.width * sizeof(move_t) * 6);


            // Do all logical steps. If false, board is invalid & step back
            // since im pre-running on all child moves, this will never be false
            if (logic_module(currPos->board, currPos->moves) == -1){
                free(currPos->moves);
                destroy_board(currPos->board);
                currPos->num_moves = -1;
                currPosInd--;
                invalid++;
                continue;
            }

            #if DEBUG
                assert(board_consistent(currPos->board));
            #endif

            // Check if we're done
            if (is_solved(currPos->board)){
                #if DISPLAY_TYPE
                    print_board(currPos->board);
                #endif
                printf("Solved!\n");
                break;
            }
            
            // Generate all possible moves for this layer and don't move down
            currPos->num_moves = generate_moves_complete(currPos->board, currPos->moves);
            // Test all moves 1 level.
            for (int i = 0; i < currPos->num_moves; i++){
                if (!move_is_incorrect(currPos->board, currPos->moves[i]) &&
                    !test_move(currPos->board, currPos->moves + i)){
                    add_incorrect(currPos->board, currPos->moves[i]);
                    invalid++;
                }
            }

            // Now sort all the moves
            qsort(currPos->moves, currPos->num_moves, sizeof(move_t), compare_moves);
        } else if (currPos->num_moves == 0){
            // Out of moves. Reset and go back
            currPos->num_moves = -1;
            destroy_board(currPos->board);
            free(currPos->moves);
            currPosInd--;
            invalid++;

            // Set last used move to incorrect
            if (currPosInd == -1){
                printf("No solution?\n");
                break;
            }

            position_t* t = stack + currPosInd;
            add_incorrect(t->board, t->moves[t->num_moves]);
        } else {
            // Grab move, copy it to next board and increment
            move_t currMove = currPos->moves[--currPos->num_moves];

            // If move is incorrect, skip it and move to next
            if (move_is_incorrect(currPos->board, currMove)){
                skips++;
                continue;
            }
            
            stack[currPosInd + 1].board = malloc(sizeof(board_t));

            copy_board(currPos->board, stack[currPosInd + 1].board);
            perform_move(stack[currPosInd + 1].board, currMove);
            currPosInd += 1;
        }
        frames++;
    }
    
    // Destroy stack
    for (int i = 0; i < currPosInd; i++){
        destroy_board(stack[i].board);
        free(stack[i].moves);
    }

    free(stack);

    printf("Skips: %d\n", skips);

    return frames;
}

/*  Some notes:
 *  The max stack size is the area of the board
 */
int main(int argc, char **argv){
    printf("Starting Solver\n");
    
    char buf[32];
    char *filename = (char *) buf;

    // CHANGE BACK TO 0 0
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
    
    printf("Loading levelpack...\n");
    if (!load_level_pack(lp, filename)){
        printf("Could not file packs/%s.flow\n", filename);
        return -1;
    }

    printf("Success!\n");
    
    // Free all the int coordinates.
    // TODO: I should just make a destroy function for levelpacks

    max_ind = (max_ind == 0 || max_ind > lp->num_levels) ? lp->num_levels : max_ind;

    // Record all data
    run_data_t all_data[max_ind]; 
    clock_t start, end;

    for (int i = min_ind; i < max_ind; i++){
        printf("solving board %d\n", i);
        // Measure time to solve
        start = clock();
        int frames = solve(lp->levels[i], i);
        end = clock();

        all_data[i].filename = filename;
        all_data[i].level = i;
        all_data[i].time = ((double) (end - start));
        all_data[i].frames = frames;

        printf("Completed level %d in %f seconds. (%d frames)\n", i, ((double) (end - start)) / CLOCKS_PER_SEC, frames);
    }
    double total_time = 0;

    for (int i = min_ind; i < max_ind; i++){
        total_time += all_data[i].time;
    }
    printf("Completed all levels in %f seconds!\n", total_time / CLOCKS_PER_SEC);

    // Free everything
    // A create function couldn't hurt either
    for (int i = 0; i < lp->num_levels; i++){
        free(lp->levels[i].positions);
    }

    free(lp->levels);
    free(lp);

    printf("Done!\n");
}
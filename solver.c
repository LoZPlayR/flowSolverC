#include "solver.h"

const int MAX_DEPTH = 25;

/*  Some notes:
    The max stack size is the area of the board.
    TODO: Set up evaluation
    TODO: Set up board saving

    Should prolly move into Windows/linux environment so I can actually debug/ASAN
 */
int main(void){
    printf("Start\n");
    clock_t start, end, setup_done, cleanup_done;

    start = clock();

    // Some logic to choose a board here

    // Time to set up solver

    int n = 3;
    int width = 5;
    int height = 5;
    int area = width * height;

    position_t* stack = calloc(height * width, sizeof(position_t));

    for (int i = 0; i < height * width; i++){      
        stack[i].board = NULL;
        stack[i].num_moves = -1;
        
        // Allocate all space for moves (First move can have 4n, all others 3n)
        stack[i].moves = malloc(sizeof(move_t) * n * (i != 0 ? 3 : 4));
        
        // Allocate all boards (will be modified in the future)
        stack[i].board = create_board();
    }

    print_board(stack[0].board);

    int depth = 0;
    int count = 0;
    
    setup_done = clock();

    while(true) {
        count += 1;
        #if DEBUG
            if (MAX_DEPTH != -1 && count > MAX_DEPTH) break;
        #endif

        // Check if the board is full
        if (depth >= area - n && is_solved(stack[depth].board)) {
            printf("solved!\n");
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

                #if DEBUG
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

    end = clock();

    print_board(stack[depth].board);

    for (int i = 0; i < 10; i++){
        free(stack[i].moves);
        destroy_board(stack[i].board);
    }

    free(stack);
    cleanup_done = clock();


    printf("count = %d\n", count);
    float total_time = ((double) (end - setup_done)) / CLOCKS_PER_SEC;
    printf("Final running time: %f seconds\nAvg frames/second: %f\n", total_time, count / total_time);
    printf("Setup time: %fs\nCleanup time: %fs", ((double) (setup_done - start)) / CLOCKS_PER_SEC, ((double) (cleanup_done - end)) / CLOCKS_PER_SEC);

}
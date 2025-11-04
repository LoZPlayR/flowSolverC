#include "move_gen.h"

// Max total moves: 4 * m * n

// Find all possible moves on the board
int generate_moves(board_t* board, move_t* move_arr) {
    // Reuse for computing adjacent cells
    enum DIRECTION* occupied_cells = malloc(sizeof(enum DIRECTION) * 4);
    enum DIRECTION* unoccupied_cells = malloc(sizeof(enum DIRECTION) * 4);
    int total_moves = 0;
    
    // Iterate thru moves
    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            cellstate_t cell = get_cell(board, r, c);
            int occupied_cells_len = 0;
            int unoccupied_cells_len = 0;
            // Find non-occupied cells that have least 1 edge connecting to an occupied cell (or it is a source)
            if (!cell.node.isOccupied && cell.node.canAppend){
                cellstate_t cellNorth = NULL_CELL;
                cellstate_t cellSouth = NULL_CELL;
                cellstate_t cellEast  = NULL_CELL;
                cellstate_t cellWest  = NULL_CELL;

                // Identify the adjacent cells that are occupied vs not occupied
                if (cell.edges.n) {
                    cellNorth = get_cell(board, r - 1, c);
                    
                    // Ensure that we only connect to adjacent cells that are same color as the current (or one is EMPTY)
                    // May be obselete with prune_edges function
                    if (cell.node.col == EMPTY || cellNorth.node.col == cell.node.col || cellNorth.node.col == EMPTY){
                        if (cellNorth.node.isOccupied){
                            occupied_cells[occupied_cells_len] = NORTH;
                            occupied_cells_len += 1;
                        } else {
                            unoccupied_cells[unoccupied_cells_len] = NORTH;
                            unoccupied_cells_len += 1;
                        }
                    }
                }

                if (cell.edges.s) {
                    cellSouth = get_cell(board, r + 1, c);
                    if (cell.node.col == EMPTY || cellSouth.node.col == cell.node.col || cellSouth.node.col == EMPTY){
                        if (cellSouth.node.isOccupied){
                            occupied_cells[occupied_cells_len] = SOUTH;
                            occupied_cells_len += 1;
                        } else {
                            unoccupied_cells[unoccupied_cells_len] = SOUTH;
                            unoccupied_cells_len += 1;
                        }
                    }
                }

                if (cell.edges.e) {
                    cellEast = get_cell(board, r, c + 1);
                    if (cell.node.col == EMPTY || cellEast.node.col == cell.node.col || cellEast.node.col == EMPTY){
                        if (cellEast.node.isOccupied){
                            occupied_cells[occupied_cells_len] = EAST;
                            occupied_cells_len += 1;
                        } else {
                            unoccupied_cells[unoccupied_cells_len] = EAST;
                            unoccupied_cells_len += 1;
                        }
                    }
                }

                if (cell.edges.w) {
                    cellWest = get_cell(board, r, c - 1);
                    if (cell.node.col == EMPTY || cellWest.node.col == cell.node.col || cellWest.node.col == EMPTY){
                        if (cellWest.node.isOccupied){
                            occupied_cells[occupied_cells_len] = WEST;
                            occupied_cells_len += 1;
                        } else {
                            unoccupied_cells[unoccupied_cells_len] = WEST;
                            unoccupied_cells_len += 1;
                        }
                    }
                }
                
                // Keep track of how many moves can happen from this position
                int possibilities = 0;

                // Handle Source moves
                if (cell.node.isSource) {
                    #if DEBUG
                        if (occupied_cells_len >= 2){
                            print_board(board);
                            printf("%d %d\n", r, c);
                        }
                        assert(occupied_cells_len < 2);
                    #endif

                    for (int toInd = 0; toInd < occupied_cells_len; toInd++){
                        // Choose to cell
                        cellstate_t toCell;    
                        switch (occupied_cells[toInd]){
                            case NORTH:
                                toCell = cellNorth;
                                break;
                            case SOUTH:
                                toCell = cellSouth;
                                break;
                            case EAST:
                                toCell = cellEast;
                                break;
                            case WEST:
                                toCell = cellWest;
                                break;
                        }
                        
                        // May not need to add to cells of the same color - should alr be handled logically
                        if (toCell.node.col == EMPTY || toCell.node.col == cell.node.col){
                            move_arr[total_moves].r = r;
                            move_arr[total_moves].c = c;
                            move_arr[total_moves].from = occupied_cells[toInd];
                            move_arr[total_moves].to = occupied_cells[toInd];
                            total_moves += 1;
                            possibilities += 1;
                        }
                    }

                    for (int toInd = 0; toInd < unoccupied_cells_len; toInd++){
                        // Choose to cell
                        cellstate_t toCell;    
                        switch (unoccupied_cells[toInd]){
                            case NORTH:
                                toCell = cellNorth;
                                break;
                            case SOUTH:
                                toCell = cellSouth;
                                break;
                            case EAST:
                                toCell = cellEast;
                                break;
                            case WEST:
                                toCell = cellWest;
                                break;
                        }
                        
                        if (toCell.node.col == EMPTY || toCell.node.col == cell.node.col){
                            move_arr[total_moves].r = r;
                            move_arr[total_moves].c = c;
                            move_arr[total_moves].from = unoccupied_cells[toInd];
                            move_arr[total_moves].to = unoccupied_cells[toInd];
                            total_moves += 1;
                            possibilities += 1;
                        }
                    }
                // Handle non-source moves
                } else {
                    #if DEBUG
                        assert(occupied_cells_len < 3);
                    #endif
                    // Attempt to match adjacent, same-color occuppied cells to each other
                    for (int fromInd = 0; fromInd < occupied_cells_len; fromInd++) {
                        // Choose from cell
                        cellstate_t fromCell;
                        switch (occupied_cells[fromInd]){
                            case NORTH:
                                fromCell = cellNorth;
                                break;
                            case SOUTH:
                                fromCell = cellSouth;
                                break;
                            case EAST:
                                fromCell = cellEast;
                                break;
                            case WEST:
                                fromCell = cellWest;
                                break;
                        }
                        
                        // Match to occupied cell
                        for (int toInd = fromInd + 1; toInd < occupied_cells_len; toInd++) {
                            // Choose to cell
                            cellstate_t toCell;
                            
                            switch (occupied_cells[toInd]){
                                case NORTH:
                                    toCell = cellNorth;
                                    break;
                                case SOUTH:
                                    toCell = cellSouth;
                                    break;
                                case EAST:
                                    toCell = cellEast;
                                    break;
                                case WEST:
                                    toCell = cellWest;
                                    break;
                            }
                            
                            // I only need to check that to.col == from.col when from is nonempty
                            if (fromCell.node.col == EMPTY || toCell.node.col == EMPTY || toCell.node.col == fromCell.node.col) {
                                move_arr[total_moves].r = r;
                                move_arr[total_moves].c = c;
                                move_arr[total_moves].from = occupied_cells[fromInd];
                                move_arr[total_moves].to = occupied_cells[toInd];
                                total_moves += 1;
                                possibilities += 1;
                            }
                        }
    
                        // Match to unoccupied cells
                        for (int toInd = 0; toInd < unoccupied_cells_len; toInd++) {
                            // Choose to cell
                            cellstate_t toCell;
                            
                            switch (unoccupied_cells[toInd]){
                                case NORTH:
                                    toCell = cellNorth;
                                    break;
                                case SOUTH:
                                    toCell = cellSouth;
                                    break;
                                case EAST:
                                    toCell = cellEast;
                                    break;
                                case WEST:
                                    toCell = cellWest;
                                    break;
                            }
                            
                            // I only need to check that to.col == from.col when from is nonempty
                            if (fromCell.node.col == EMPTY || toCell.node.col == EMPTY || toCell.node.col == fromCell.node.col) {
                                move_arr[total_moves].r = r;
                                move_arr[total_moves].c = c;
                                move_arr[total_moves].from = occupied_cells[fromInd];
                                move_arr[total_moves].to = unoccupied_cells[toInd];
                                total_moves += 1;
                                possibilities += 1;
                            }
                        }
                    }
                }
                
                #if DEBUG
                    // Should never be guaranteed - this means logic is failing somewhere
                    
                    if (possibilities <= 1){
                        printf("%d possibilities at (%d, %d)\n", possibilities, r, c);
                        print_board(board);
                    }
                    assert(possibilities > 1);
                #endif
                // Now set all added moves' possibilities
                for (int i = 0; i < possibilities; i++){
                    move_arr[total_moves - i - 1].possibilities = possibilities;
                }
            }
        }
    }
    free(occupied_cells);
    free(unoccupied_cells);

    return total_moves;
}

// Literally try all moves i can
// max moves: 6 * (w * h - 2 * n) + 4 * n = 6 * w * h - 8 * n < 6 * w * h
int generate_moves_complete(board_t* board, move_t* move_arr){
    int total_moves = 0;
    
    // Iterate thru moves
    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            cellstate_t cell = get_cell(board, r, c);

            // Skip occupied cells
            if (cell.node.isOccupied) continue;

            int possibilities = 0;

            if (cell.node.isSource){
                if (cell.edges.n){
                    move_t m = {.r = r, .c = c, .from=NORTH, .to=NORTH};
                    if (validate_move(board, m) && !move_is_incorrect(board, m)){
                        move_arr[total_moves++] = m;
                        possibilities++;
                    }
                }
                if (cell.edges.e){
                    move_t m = {.r = r, .c = c, .from=EAST, .to=EAST};
                    if (validate_move(board, m) && !move_is_incorrect(board, m)){
                        move_arr[total_moves++] = m;
                        possibilities++;
                    }
                }
                if (cell.edges.s){
                    move_t m = {.r = r, .c = c, .from=SOUTH, .to=SOUTH};
                    if (validate_move(board, m) && !move_is_incorrect(board, m)){
                        move_arr[total_moves++] = m;
                        possibilities++;
                    }
                }
                if (cell.edges.w){
                    move_t m = {.r = r, .c = c, .from=WEST, .to=WEST};
                    if (validate_move(board, m) && !move_is_incorrect(board, m)){
                        move_arr[total_moves++] = m;
                        possibilities++;
                    }
                }
            } else {
                // Try all moves given edge states (use validate_move for color matching)
                if (cell.edges.n) {
                    if (cell.edges.e){
                        move_t m = {.r = r, .c = c, .from=NORTH, .to=EAST};
                        if (validate_move(board, m) && !move_is_incorrect(board, m)){
                            move_arr[total_moves++] = m;
                            possibilities++;
                        }
                    }
                    if (cell.edges.s){
                        move_t m = {.r = r, .c = c, .from=NORTH, .to=SOUTH};
                        if (validate_move(board, m) && !move_is_incorrect(board, m)){
                            move_arr[total_moves++] = m;
                            possibilities++;
                        }
                    }
                    if (cell.edges.w){
                        move_t m = {.r = r, .c = c, .from=NORTH, .to=WEST};
                        if (validate_move(board, m) && !move_is_incorrect(board, m)){
                            move_arr[total_moves++] = m;
                            possibilities++;
                        }
                    }
                }
                if (cell.edges.e) {
                    if (cell.edges.s){
                        move_t m = {.r = r, .c = c, .from=EAST, .to=SOUTH};
                        if (validate_move(board, m) && !move_is_incorrect(board, m)){
                            move_arr[total_moves++] = m;
                            possibilities++;
                        }
                    }
                    if (cell.edges.w){
                        move_t m = {.r = r, .c = c, .from=EAST, .to=WEST};
                        if (validate_move(board, m) && !move_is_incorrect(board, m)){
                            move_arr[total_moves++] = m;
                            possibilities++;
                        }
                    }
                }
                if (cell.edges.s){
                    if (cell.edges.w){
                        move_t m = {.r = r, .c = c, .from=SOUTH, .to=WEST};
                        if (validate_move(board, m) && !move_is_incorrect(board, m)){
                            move_arr[total_moves++] = m;
                            possibilities++;
                        }
                    }
                }      
            }

            // now do possibilities
            // #if DEBUG
            //     if (possibilities <= 1){
            //         print_board(board);
            //         printf("Found %d moves at (%d, %d)\n", possibilities, r, c);
            //     }
            //     assert(possibilities > 1);
            // #endif

            // Now set all added moves' possibilities
            for (int i = 0; i < possibilities; i++){
                move_arr[total_moves - i - 1].possibilities = possibilities;
            }
        }
    }

    return total_moves;
}

void print_move(move_t move) {
    char* corner;
    if (move.from == NORTH || move.to == NORTH) {
        if (move.from == EAST || move.to == EAST) {
            corner = " ┗";
        } else if (move.from == WEST || move.to == WEST) {
            corner = "━┛";
        } else if (move.from == SOUTH || move.to == SOUTH){
            corner = " ┃";
        } else { // Both are north
            corner = "n⏺";
        }
    } else if (move.from == EAST || move.to == EAST) {
        if (move.from == WEST || move.to == WEST) {
            corner = "━━";
        } else if (move.from == SOUTH || move.to == SOUTH) {
            corner = " ┏";
        } else { // Both are east
            corner = "e⏺";
        }
    } else if (move.from == WEST || move.to == WEST) {
        if (move.from == SOUTH || move.to == SOUTH){
            corner = "━┓";
        } else { // Both are west
            corner = "w⏺";
        }
    } else { // Both are south
        corner = "s⏺";
    }
    printf("Connect %s at (%d, %d). (%d possibilities), (%d info)\n", corner, move.r, move.c, move.possibilities, move.info);
}

// for sorting moves from least possibilities to most
int compare_moves (const void* a, const void* b){
    return ((move_t*)a)->info - ((move_t*)b)->info;
}
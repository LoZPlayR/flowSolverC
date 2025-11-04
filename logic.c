#include "logic.h"

// I may not need to check N and W edges?
void prune_edges(board_t* board){
    int w = board->width;
    
    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < w; c++){
            cellstate_t cell = get_cell(board, r, c);
            enum COLOR currCol = cell.node.col;

            if (currCol == EMPTY) continue;
             
            // Check all surrounding cells
            // if (cell.edges.n){
            //     cellstate_t comp = get_cell(board, r - 1, c);
            //     enum COLOR compCol = comp.node.col;

            //     if (compCol != EMPTY && compCol != currCol){
            //         bitmap_set(board->edges, c + 1, 2 * r, 0);
            //     }
            // }
            if (cell.edges.e){
                cellstate_t comp = get_cell(board, r, c + 1);
                enum COLOR compCol = comp.node.col;

                if (compCol != EMPTY && compCol != currCol){
                    bitmap_set(board->edges, c + 1, 2 * r + 1, 0);
                }
            }
            if (cell.edges.s){
                cellstate_t comp = get_cell(board, r + 1, c);
                enum COLOR compCol = comp.node.col;

                if (compCol != EMPTY && compCol != currCol){
                    bitmap_set(board->edges, c + 1, 2 * r + 2, 0);
                }
            }
            // if (cell.edges.w){
            //     cellstate_t comp = get_cell(board, r, c - 1);
            //     enum COLOR compCol = comp.node.col;

            //     if (compCol != EMPTY && compCol != currCol){
            //         bitmap_set(board->edges, c, 2 * r + 1, 0);
            //     }
            // }
        }
    }
}

// prunes a 3x3 area
void prune_edges_area(board_t* board, int r, int c){
    int w = board->width;
    int h = board->height;

    // Get bounds
    int lo_r = r == 0 ? 0 : r - 1;
    int hi_r = r == h - 1 ? h : r + 2;
    int lo_c = c == 0 ? 0 : c - 1;
    int hi_c = c == w - 1 ? w : c + 2;

    // Do the 3x3 area
    for (int r_i = lo_r; r_i < hi_r; r_i++){
        for (int c_i = lo_c; c_i < hi_c; c_i++){
            cellstate_t cell = get_cell(board, r, c);
            enum COLOR currCol = cell.node.col;

            if (currCol == EMPTY) continue;
            if (cell.edges.e){
                cellstate_t comp = get_cell(board, r, c + 1);
                enum COLOR compCol = comp.node.col;

                if (compCol != EMPTY && compCol != currCol){
                    bitmap_set(board->edges, c + 1, 2 * r + 1, 0);
                }
            }

            if (cell.edges.s){
                cellstate_t comp = get_cell(board, r + 1, c);
                enum COLOR compCol = comp.node.col;

                if (compCol != EMPTY && compCol != currCol){
                    bitmap_set(board->edges, c + 1, 2 * r + 2, 0);
                }
            }
        }
    }
}

// Prunes edges by looking at the corner [[ ONLY RUN WITH OCCUPIED EDGES ]]
void inverse_corner_prune(board_t* board, int r, int c){
    cellstate_t cell = get_cell(board, r, c);
    bool n, e, s, w;
    n = cell.edges.n;
    e = cell.edges.e;
    s = cell.edges.s;
    w = cell.edges.w;

    #if DEBUG
        assert(cell.node.isOccupied);
    #endif
    
    if (n == s || cell.node.isSource) return;

    #if DEBUG
        assert(n + e + s + w == 2);
    #endif

    int r2 = r + (n ? -1 : 1);
    int c2 = c + (w ? -1 : 1);

    
    // ensure on board and non-source
    if (0 <= r2 && 0 <= c2 && board->height > r2 && board->width > c2 && !get_cell(board, r2, c2).node.isSource){
        if (n) {
            bitmap_set(board->edges, c2 + 1, 2 * r2 + 2, 0);
        } else {
            bitmap_set(board->edges, c2 + 1, 2 * r2, 0);
        }

        if (e){
            bitmap_set(board->edges, c2, 2 * r2 + 1, 0);
        } else {
            bitmap_set(board->edges, c2 + 1, 2 * r2 + 1, 0);
        }
    }
}

// searches for unoccupied spaces with 2 edges (1 if source)
// checks for matching colors, occupied spaces
// returns number of changes occured
int fill_paths(board_t* board){
    int anyUpdate = 0;
    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            bool update = false;
            cellstate_t cell = get_cell(board, r, c);

            if (cell.node.isOccupied) continue;

            enum COLOR col = cell.node.col;
            bool n, e, s, w;
            n = cell.edges.n;
            e = cell.edges.e;
            s = cell.edges.s;
            w = cell.edges.w;
            
            // tracking same-colored edges
            bool nCol = false;
            bool eCol = false;
            bool sCol = false;
            bool wCol = false;

            // tracking occupied edges
            bool nOcc = false;
            bool eOcc = false;
            bool sOcc = false;
            bool wOcc = false;

            if (n) {
                cellstate_t nCell = get_cell(board, r - 1, c);
                nCol = nCell.node.col == col;
                nOcc = nCell.node.isOccupied;
            }
            if (e) {
                cellstate_t eCell = get_cell(board, r, c + 1);
                eCol = eCell.node.col == col;
                eOcc = eCell.node.isOccupied;
            }
            if (s) {
                cellstate_t sCell = get_cell(board, r + 1, c);
                sCol = sCell.node.col == col;
                sOcc = sCell.node.isOccupied;
            }
            if (w) {
                cellstate_t wCell = get_cell(board, r, c - 1);
                wCol = wCell.node.col == col;
                wOcc = wCell.node.isOccupied;
            }

            int edgeCount = n + e + s + w;
            int colEdgeCount = col == EMPTY ? 0 : nCol + eCol + sCol + wCol;
            int occEdgeCount = nOcc + eOcc + sOcc + wOcc;
            
            if (cell.node.isSource) {
                if (edgeCount == 1) {
                    update = true;
        
                    move_t move;
                    move.r = r;
                    move.c = c;
    
                    if (n) {
                        move.from = NORTH;
                        move.to = NORTH;
                    } else if (e) {
                        move.from = EAST;
                        move.to = EAST;
                    } else if (s) {
                        move.from = SOUTH;
                        move.to = SOUTH;
                    } else {
                        move.from = WEST;
                        move.to = WEST;
                    }
                    perform_move(board, move);
                } else if (colEdgeCount == 1) {
                    update = true;
        
                    move_t move;
                    move.r = r;
                    move.c = c;
                    
                    if (nCol) {
                        move.from = NORTH;
                        move.to = NORTH;
                    } else if (eCol) {
                        move.from = EAST;
                        move.to = EAST;
                    } else if (sCol) {
                        move.from = SOUTH;
                        move.to = SOUTH;
                    } else {
                        move.from = WEST;
                        move.to = WEST;
                    }
                    perform_move(board, move);
                } else if (occEdgeCount == 1){
                    update = true;
        
                    move_t move;
                    move.r = r;
                    move.c = c;
                    
                    if (nOcc) {
                        move.from = NORTH;
                        move.to = NORTH;
                    } else if (eOcc) {
                        move.from = EAST;
                        move.to = EAST;
                    } else if (sOcc) {
                        move.from = SOUTH;
                        move.to = SOUTH;
                    } else {
                        move.from = WEST;
                        move.to = WEST;
                    }
                    perform_move(board, move);
                }
            } else {
                if (edgeCount == 2) {
                    update = true;
        
                    move_t move;
                    move.r = r;
                    move.c = c;
                    
                    if (n){
                        move.from = NORTH;
                        if (e){
                            // NE
                            move.to = EAST;
                        } else if (s) {
                            // NS
                            move.to = SOUTH;
                        } else {
                            // NW
                            move.to = WEST;
                        }
                    } else if (e) {
                        move.from = EAST;
                        if (s) {
                            // SE
                            move.to = SOUTH;
                        } else {
                            // EW
                            move.to = WEST;
                        }
                    } else {
                        // SW
                        move.from = SOUTH;
                        move.to = WEST;
                    }
                    perform_move(board, move);
                } else if (colEdgeCount == 2){
                    update = true;
        
                    move_t move;
                    move.r = r;
                    move.c = c;
                    
                    if (nCol){
                        move.from = NORTH;
                        if (eCol){
                            // NE
                            move.to = EAST;
                        } else if (sCol) {
                            // NS
                            move.to = SOUTH;
                        } else {
                            // NW
                            move.to = WEST;
                        }
                    } else if (eCol) {
                        move.from = EAST;
                        if (sCol) {
                            // SE
                            move.to = SOUTH;
                        } else {
                            // EW
                            move.to = WEST;
                        }
                    } else {
                        // SW
                        move.from = SOUTH;
                        move.to = WEST;
                    }
                    perform_move(board, move);
                } else if (occEdgeCount == 2) {
                    update = true;
        
                    move_t move;
                    move.r = r;
                    move.c = c;
                    
                    if (nOcc){
                        move.from = NORTH;
                        if (eOcc){
                            // NE
                            move.to = EAST;
                        } else if (sOcc) {
                            // NS
                            move.to = SOUTH;
                        } else {
                            // NW
                            move.to = WEST;
                        }
                    } else if (eOcc) {
                        move.from = EAST;
                        if (sOcc) {
                            // SE
                            move.to = SOUTH;
                        } else {
                            // EW
                            move.to = WEST;
                        }
                    } else {
                        // SW
                        move.from = SOUTH;
                        move.to = WEST;
                    }          
                    perform_move(board, move);
                }
            }
            
            // Do any pruning?
            if (update){
                anyUpdate++;
                prune_edges_area(board, r, c);
                inverse_corner_prune(board, r, c);
            }
        }
    }
    return anyUpdate;
}


// searches for unoccupied spaces with 2 edges (1 if source)
// checks for matching colors, occupied spaces
// returns whether a change has occurred
///

/// @brief Generates logically guaranteed moves and stores in buffer
/// @param board Board object
/// @param buffer move_t array for storing moves
/// @return number of moves generated
int logic_gen(board_t* board, move_t* buffer){
    int count = 0;
    
    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            cellstate_t cell = get_cell(board, r, c);

            if (cell.node.isOccupied) continue;

            enum COLOR col = cell.node.col;
            bool n, e, s, w;
            n = cell.edges.n;
            e = cell.edges.e;
            s = cell.edges.s;
            w = cell.edges.w;
            
            // tracking same-colored edges
            bool nCol = false;
            bool eCol = false;
            bool sCol = false;
            bool wCol = false;

            // tracking occupied edges
            bool nOcc = false;
            bool eOcc = false;
            bool sOcc = false;
            bool wOcc = false;

            if (n) {
                cellstate_t nCell = get_cell(board, r - 1, c);
                nCol = nCell.node.col == col;
                nOcc = nCell.node.isOccupied;
            }
            if (e) {
                cellstate_t eCell = get_cell(board, r, c + 1);
                eCol = eCell.node.col == col;
                eOcc = eCell.node.isOccupied;
            }
            if (s) {
                cellstate_t sCell = get_cell(board, r + 1, c);
                sCol = sCell.node.col == col;
                sOcc = sCell.node.isOccupied;
            }
            if (w) {
                cellstate_t wCell = get_cell(board, r, c - 1);
                wCol = wCell.node.col == col;
                wOcc = wCell.node.isOccupied;
            }

            int edgeCount = n + e + s + w;
            int colEdgeCount = col == EMPTY ? 0 : nCol + eCol + sCol + wCol;
            int occEdgeCount = nOcc + eOcc + sOcc + wOcc;
            
            if (cell.node.isSource) {
                if (edgeCount == 1) {
                    move_t move;
                    move.r = r;
                    move.c = c;
    
                    if (n) {
                        move.from = NORTH;
                        move.to = NORTH;
                    } else if (e) {
                        move.from = EAST;
                        move.to = EAST;
                    } else if (s) {
                        move.from = SOUTH;
                        move.to = SOUTH;
                    } else {
                        move.from = WEST;
                        move.to = WEST;
                    }
                    buffer[count] = move;
                    count += 1;
                } else if (colEdgeCount == 1) {
                    move_t move;
                    move.r = r;
                    move.c = c;
                    
                    if (nCol) {
                        move.from = NORTH;
                        move.to = NORTH;
                    } else if (eCol) {
                        move.from = EAST;
                        move.to = EAST;
                    } else if (sCol) {
                        move.from = SOUTH;
                        move.to = SOUTH;
                    } else {
                        move.from = WEST;
                        move.to = WEST;
                    }
                    buffer[count] = move;
                    count += 1;
                } else if (occEdgeCount == 1){
                    move_t move;
                    move.r = r;
                    move.c = c;
                    
                    if (nOcc) {
                        move.from = NORTH;
                        move.to = NORTH;
                    } else if (eOcc) {
                        move.from = EAST;
                        move.to = EAST;
                    } else if (sOcc) {
                        move.from = SOUTH;
                        move.to = SOUTH;
                    } else {
                        move.from = WEST;
                        move.to = WEST;
                    }
                    buffer[count] = move;
                    count += 1;
                }
            } else {
                if (edgeCount == 2) {
                    move_t move;
                    move.r = r;
                    move.c = c;
                    
                    if (n){
                        move.from = NORTH;
                        if (e){
                            // NE
                            move.to = EAST;
                        } else if (s) {
                            // NS
                            move.to = SOUTH;
                        } else {
                            // NW
                            move.to = WEST;
                        }
                    } else if (e) {
                        move.from = EAST;
                        if (s) {
                            // SE
                            move.to = SOUTH;
                        } else {
                            // EW
                            move.to = WEST;
                        }
                    } else {
                        // SW
                        move.from = SOUTH;
                        move.to = WEST;
                    }
                    buffer[count] = move;
                    count += 1;
                } else if (colEdgeCount == 2){
                    move_t move;
                    move.r = r;
                    move.c = c;
                    
                    if (nCol){
                        move.from = NORTH;
                        if (eCol){
                            // NE
                            move.to = EAST;
                        } else if (sCol) {
                            // NS
                            move.to = SOUTH;
                        } else {
                            // NW
                            move.to = WEST;
                        }
                    } else if (eCol) {
                        move.from = EAST;
                        if (sCol) {
                            // SE
                            move.to = SOUTH;
                        } else {
                            // EW
                            move.to = WEST;
                        }
                    } else {
                        // SW
                        move.from = SOUTH;
                        move.to = WEST;
                    }
                    buffer[count] = move;
                    count += 1;
                } else if (occEdgeCount == 2) {
                    move_t move;
                    move.r = r;
                    move.c = c;
                    
                    if (nOcc){
                        move.from = NORTH;
                        if (eOcc){
                            // NE
                            move.to = EAST;
                        } else if (sOcc) {
                            // NS
                            move.to = SOUTH;
                        } else {
                            // NW
                            move.to = WEST;
                        }
                    } else if (eOcc) {
                        move.from = EAST;
                        if (sOcc) {
                            // SE
                            move.to = SOUTH;
                        } else {
                            // EW
                            move.to = WEST;
                        }
                    } else {
                        // SW
                        move.from = SOUTH;
                        move.to = WEST;
                    }          
                    buffer[count] = move;
                    count += 1;
                }
            }
        }
    }
    return count;
}

// Parallel to move_gen_complete. Instead, looks for moves that have exactly 1 possibility
// max size move_arr = w*l - n
int logic_gen_complete(board_t* board, move_t* move_arr){
    int total_moves = 0;
    
    // Iterate thru moves
    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            cellstate_t cell = get_cell(board, r, c);

            // Skip occupied cells
            if (cell.node.isOccupied) continue;

            int possibilities = 0;
            move_t tempMove = {.r = r, .c = c, .possibilities = 1};
            move_t lastValid = tempMove;

            if (cell.node.isSource){
                if (cell.edges.n){
                    tempMove.from = NORTH;
                    tempMove.to = NORTH;
                    if (validate_move(board, tempMove) && !move_is_incorrect(board, tempMove)){
                        possibilities++;
                        lastValid = tempMove;
                    }
                }
                if (cell.edges.e){
                    tempMove.from = EAST;
                    tempMove.to = EAST;

                    if (validate_move(board, tempMove) && !move_is_incorrect(board, tempMove)){
                        possibilities++;
                        lastValid = tempMove;
                    }
                }
                if (cell.edges.s){
                    tempMove.from = SOUTH;
                    tempMove.to = SOUTH;
                    if (validate_move(board, tempMove) && !move_is_incorrect(board, tempMove)){
                        possibilities++;
                        lastValid = tempMove;
                    }
                }
                if (cell.edges.w){
                    tempMove.from = WEST;
                    tempMove.to = WEST;
                    if (validate_move(board, tempMove) && !move_is_incorrect(board, tempMove)){
                        possibilities++;
                        lastValid = tempMove;
                    }
                }
            } else {
                // Try all moves given edge states (use validate_move for color matching)
                if (cell.edges.n) {
                    if (cell.edges.e){
                        tempMove.from = NORTH;
                        tempMove.to = EAST;
                        if (validate_move(board, tempMove) && !move_is_incorrect(board, tempMove)){
                            possibilities++;
                            lastValid = tempMove;
                        }
                    }
                    if (cell.edges.s){
                        tempMove.from = NORTH;
                        tempMove.to = SOUTH;
                        if (validate_move(board, tempMove) && !move_is_incorrect(board, tempMove)){
                            possibilities++;
                            lastValid = tempMove;
                        }
                    }
                    if (cell.edges.w){
                        tempMove.from = NORTH;
                        tempMove.to = WEST;
                        if (validate_move(board, tempMove) && !move_is_incorrect(board, tempMove)){
                            possibilities++;
                            lastValid = tempMove;
                        }
                    }
                }
                if (cell.edges.e) {
                    if (cell.edges.s){
                        tempMove.from = EAST;
                        tempMove.to = SOUTH;
                        if (validate_move(board, tempMove) && !move_is_incorrect(board, tempMove)){
                            possibilities++;
                            lastValid = tempMove;
                        }
                    }
                    if (cell.edges.w){
                        tempMove.from = EAST;
                        tempMove.to = WEST;
                        if (validate_move(board, tempMove) && !move_is_incorrect(board, tempMove)){
                            possibilities++;
                            lastValid = tempMove;
                        }
                    }
                }
                if (cell.edges.s){
                    if (cell.edges.w){
                        tempMove.from = SOUTH;
                        tempMove.to = WEST;
                        if (validate_move(board, tempMove) && !move_is_incorrect(board, tempMove)){
                            possibilities++;
                            lastValid = tempMove;
                        }
                    }
                }      
            }
            if (possibilities == 1){
                move_arr[total_moves++] = lastValid;
            } else {
                cellstate_t nCell = cell.edges.n ? get_cell(board, r - 1, c) : NULL_CELL;
                cellstate_t eCell = cell.edges.e ? get_cell(board, r, c + 1) : NULL_CELL;
                cellstate_t sCell = cell.edges.s ? get_cell(board, r + 1, c) : NULL_CELL;
                cellstate_t wCell = cell.edges.w ? get_cell(board, r, c - 1) : NULL_CELL;
                
                int occupiedEdges = (cell.edges.n && nCell.node.isOccupied) + 
                                (cell.edges.e && eCell.node.isOccupied) + 
                                (cell.edges.s && sCell.node.isOccupied) + 
                                (cell.edges.w && wCell.node.isOccupied); 
                
                // Occupied edges restrict to 1 move
                if (occupiedEdges == 1 + !cell.node.isSource) {
                    if (cell.edges.n && nCell.node.isOccupied){
                        lastValid.from = NORTH;
                        if (cell.edges.e && eCell.node.isOccupied){
                            lastValid.to = EAST;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        } else if (cell.edges.s && sCell.node.isOccupied){
                            lastValid.to = SOUTH;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        } else if (cell.edges.w && wCell.node.isOccupied){
                            lastValid.to = WEST;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        } else {
                            // Must be source node
                            lastValid.to = NORTH;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                    } else if (cell.edges.e && eCell.node.isOccupied){
                        lastValid.from = EAST;
                        if (cell.edges.s && sCell.node.isOccupied){
                            lastValid.to = SOUTH;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                        else if (cell.edges.w && wCell.node.isOccupied){
                            lastValid.to = WEST;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        } else {
                            // Must be source node
                            lastValid.to = EAST;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                    } else if (cell.edges.s && sCell.node.isOccupied){
                        lastValid.from = SOUTH;
                        if (cell.edges.w && wCell.node.isOccupied){
                            lastValid.to = WEST;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        } else {
                            // Must be source node
                            lastValid.to = SOUTH;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                    } else { // Must be west sorce node
                        lastValid.from = WEST;
                        lastValid.to = WEST;
                        move_arr[total_moves++] = lastValid;
                        continue;
                    }
                }

                int matchingEdges = (cell.edges.n && (nCell.node.col == cell.node.col)) + 
                                    (cell.edges.e && (eCell.node.col == cell.node.col)) + 
                                    (cell.edges.s && (sCell.node.col == cell.node.col)) + 
                                    (cell.edges.w && (wCell.node.col == cell.node.col)); 
                
                // Colors restrict to 1 move 
                if (cell.node.col == EMPTY) {
                    // If there's an empty edge, it can be used
                    if (matchingEdges != 0) continue;

                    // No empty edge means there must be a pair of same-colored edges (or incorrect board)
                    if (cell.edges.n) {
                        lastValid.from = NORTH;
                        if (cell.edges.e && nCell.node.col == eCell.node.col) {
                            lastValid.to = EAST;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                        if (cell.edges.s && nCell.node.col == sCell.node.col){
                            lastValid.to = SOUTH;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                        if (cell.edges.w && nCell.node.col == wCell.node.col){
                            lastValid.to = WEST;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                    }
                    if (cell.edges.e) {
                        lastValid.from = EAST;
                        if (cell.edges.s && eCell.node.col == sCell.node.col){
                            lastValid.to = SOUTH;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                        if (cell.edges.w && eCell.node.col == wCell.node.col){
                            lastValid.to = WEST;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                    }
                    if (cell.edges.s) {
                        lastValid.from = SOUTH;
                        if (cell.edges.w && eCell.node.col == wCell.node.col){
                            lastValid.to = WEST;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                    }
                } else {
                    // If there are enough edges matching the color, I know the shape of this space
                    if (matchingEdges == 1 + !cell.node.isSource){
                        if (cell.edges.n && nCell.node.col == cell.node.col){
                            lastValid.from = NORTH;
                            if (cell.edges.e && eCell.node.col == cell.node.col) {
                                lastValid.to = EAST;
                                move_arr[total_moves++] = lastValid;
                                continue;
                            } else if (cell.edges.s && sCell.node.col == cell.node.col) {
                                lastValid.to = SOUTH;
                                move_arr[total_moves++] = lastValid;
                                continue;
                            } else if (cell.edges.w && wCell.node.col == cell.node.col) {
                                lastValid.to = WEST;
                                move_arr[total_moves++] = lastValid;
                                continue;
                            } else {
                                // Must be a source node w/ 1 edge
                                lastValid.to = NORTH;
                                move_arr[total_moves++] = lastValid;
                                continue;
                            }
                        } else if (cell.edges.e && eCell.node.col == cell.node.col) {
                            lastValid.from = EAST;
                            if (cell.edges.s && sCell.node.col == cell.node.col) {
                                lastValid.to = SOUTH;
                                move_arr[total_moves++] = lastValid;
                                continue;
                            } else if (cell.edges.w && wCell.node.col == cell.node.col) {
                                lastValid.to = WEST;
                                move_arr[total_moves++] = lastValid;
                                continue;
                            } else {
                                // Must be a source node w/ 1 edge
                                lastValid.to = EAST;
                                move_arr[total_moves++] = lastValid;
                                continue;
                            }
                        } else if (cell.edges.s && sCell.node.col == cell.node.col) {
                            lastValid.from = SOUTH;
                            if (cell.edges.w && wCell.node.col == cell.node.col) {
                                lastValid.to = WEST;
                                move_arr[total_moves++] = lastValid;
                                continue;
                            } else {
                                // Must be a source node w/ 1 edge
                                lastValid.to = SOUTH;
                                move_arr[total_moves++] = lastValid;
                                continue;
                            }
                        } else {
                            // Must be source node pointing west
                            lastValid.from = WEST;
                            lastValid.to = WEST;
                            move_arr[total_moves++] = lastValid;
                            continue;
                        }
                    }
                }
            }
        }
    }

    return total_moves;
}

// Implements the corner rule (returns number of changes that occurred)
int corner_rule(board_t* board){
    int update = 0;
    int hi = board->height;
    int wi = board->width;

    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            cellstate_t cell = get_cell(board, r, c);
            if (!cell.node.isOccupied || cell.node.isSource) continue;
            
            bool n, e, s, w;
            n = cell.edges.n;
            e = cell.edges.e;
            s = cell.edges.s;
            w = cell.edges.w;
            
            #if DEBUG
                assert(n + e + s + w == 2);
            #endif
            
            // Remove NS (ns both true) and EW (ns both false) cells
            if (n == s) continue;
            
            int vert = n ? -1 : 1;
            int horz = w ? -1 : 1;
            
            int cur_r = r + vert;
            int cur_c = c + horz;
            
            // TODO: turn into a for loop?
            while (true){
                // Don't go off the board
                if (0 > cur_r || 0 > cur_c) break;
                if (cur_r == hi || cur_c == wi) break;

                cellstate_t currCell = get_cell(board, cur_r, cur_c);
                
                // find empty cells
                if (currCell.node.isOccupied || currCell.node.isSource) break;
                update++;

                move_t move;
                move.r = cur_r;
                move.c = cur_c;
                
                if (n){
                    move.from = NORTH;
                    if (e){
                        // NE
                        move.to = EAST;
                    } else {
                        // NW
                        move.to = WEST;
                    }
                } else {
                    move.from = SOUTH;
                    if (e) {
                        // SE
                        move.to = EAST;
                    } else {
                        // EW
                        move.to = WEST;
                    }
                }
                perform_move(board, move);
                cur_r += vert;
                cur_c += horz;
            }
        }
    }
    return update;
}

// Prevent lines from turning in on themselves
void corner_prune(board_t* board){
    bool update = false;
    int hi = board->height;
    int wi = board->width;

    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            cellstate_t cell = get_cell(board, r, c);
            if (cell.node.isOccupied) continue;

            bool n, e, s, w;
            n = cell.edges.n;
            e = cell.edges.e;
            s = cell.edges.s;
            w = cell.edges.w;

            if (n){
                cellstate_t nCell = get_cell(board, r - 1, c);
                if (nCell.node.isOccupied && !nCell.node.isSource){
                    // Remove eastern edge
                    if (nCell.edges.e) bitmap_set(board->edges, c + 1, 2 * r + 1, 0);
                    // Remove western edge
                    else if (nCell.edges.w) bitmap_set(board->edges, c, 2 * r + 1, 0);
                    // Above cell is straight - do nothing
                }
            }
            if (s) {
                cellstate_t sCell = get_cell(board, r + 1, c);
                if (sCell.node.isOccupied && !sCell.node.isSource) {
                    // Remove eastern edge
                    if (sCell.edges.e) bitmap_set(board->edges, c + 1, 2 * r + 1, 0);
                    // Remove western edge
                    else if (sCell.edges.w) bitmap_set(board->edges, c, 2 * r + 1, 0);
                    // Below cell is straight - do nothing
                }
            }

            if (e) {
                cellstate_t eCell = get_cell(board, r, c + 1);
                if (eCell.node.isOccupied && !eCell.node.isSource) {
                    // Remove northern edge
                    if (eCell.edges.n) bitmap_set(board->edges, c + 1, 2 * r, 0);
                    // Remove southern edge
                    else if (eCell.edges.s) bitmap_set(board->edges, c + 1, 2 * r + 2, 0);
                    // Right cell is straight - do nothing
                }
            }

            if (w) {
                cellstate_t wCell = get_cell(board, r, c - 1);
                if (wCell.node.isOccupied && !wCell.node.isSource) {
                    // Remove northern edge
                    if (wCell.edges.n) bitmap_set(board->edges, c + 1, 2 * r, 0);
                    // Remove southern edge
                    else if (wCell.edges.s) bitmap_set(board->edges, c + 1, 2 * r + 2, 0);
                    // Left cell is straight - do nothing
                }
            }

        }
    }
}

// Uses dfs to find spaces necessary to complete lines. Marks them with appropriate color
int guaranteed_spaces(board_t* board){
    int w = board->width;
    int change = 0;
    // Make a buffer for all spaces in dfs (currently using move_t cuz y not?)
    key* path = malloc(sizeof(key) * board->height * w);
    key* parent = malloc(sizeof(key) * board->height * w);
    key* stack = malloc(4 * sizeof(key) * board->height * w);
    
    for (int i = 0; i < board->n; i += 2){
        enum COLOR col = (i >> 1) + 1; // should correspond to color?
        int pathLen = 0;

        // initial dfs to get a path + set peth_len
        bitmap_t* visited = bitmap_create(w, board->height);

        
        key start = board->sources->keys[i];
        key goal = board->sources->keys[i + 1];
        
        stack[0] = start;
        int stack_ind = 1;
        parent[start] = start;
        bool foundPath = false;

        while(stack_ind) {
            key curr = stack[--stack_ind];

            if (curr == goal) {
                foundPath = true;
                break;
            }

            int r = curr / w;
            int c = curr % w;
            cellstate_t cell = get_cell(board, r, c);

            // Skip visited cells and cells that are the wrong color
            if (bitmap_get(visited, c, r)) continue;
            if (cell.node.col != EMPTY && cell.node.col != col) continue;

            if (cell.edges.n && !bitmap_get(visited, c, r - 1)){
                key next = get_index(w, r - 1, c);
                stack[stack_ind++] = next;
                parent[next] = curr;
            }
            if (cell.edges.e && !bitmap_get(visited, c + 1, r)){
                key next = get_index(w, r, c + 1);
                stack[stack_ind++] = next;
                parent[next] = curr;
            }
            if (cell.edges.s && !bitmap_get(visited, c, r + 1)){
                key next = get_index(w, r + 1, c);
                stack[stack_ind++] = next;
                parent[next] = curr;
            }
            if (cell.edges.w && !bitmap_get(visited, c - 1, r)){
                key next = get_index(w, r, c - 1);
                stack[stack_ind++] = next;
                parent[next] = curr;
            }
            
            bitmap_set(visited, c, r, 1);
        }

        bitmap_destroy(visited);

        // Failed to find a path - must be unsolvable (move onto evaluation)
        // TODO: Make this able to return a failure
        if (!foundPath) return -1;

        // Construct path
        key currSegment = parent[goal];
        while(currSegment != start){
            path[pathLen++] = currSegment;
            currSegment = parent[currSegment];
        }

        // Iterate thru path
        for (int j = 0; j < pathLen; j++) {
            key currPath = path[j];

            int curr_r = currPath / w;
            int curr_c = currPath % w;

            cellstate_t currExclude = get_cell(board, curr_r, curr_c);
            
            // Skip cells that are already correct color 
            if (currExclude.node.col == col) continue;
            
            // secondary dfs to try and draw path len without (reuse stack)
            bitmap_t* visited2 = bitmap_create(w, board->height);
            bitmap_set(visited2, curr_c, curr_r, 1); // Do not use the current excluded path
            stack[0] = start;
            stack_ind = 1;

            bool success = false;
            while(stack_ind) {
                key curr = stack[--stack_ind];

                if (curr == goal) {
                    success = true;
                    break;
                }

                int r = curr / w;
                int c = curr % w;
                cellstate_t cell = get_cell(board, r, c);

                // Skip visited cells and cells that are the wrong color
                if (bitmap_get(visited2, c, r)) continue;
                if (cell.node.col != EMPTY && cell.node.col != col) continue;

                if (cell.edges.n) stack[stack_ind++] = get_index(w, r - 1, c);
                if (cell.edges.e) stack[stack_ind++] = get_index(w, r, c + 1);
                if (cell.edges.s) stack[stack_ind++] = get_index(w, r + 1, c);
                if (cell.edges.w) stack[stack_ind++] = get_index(w, r, c - 1);
                
                bitmap_set(visited2, c, r, 1);
            }

            bitmap_destroy(visited2);

            // All paths go thru this point, so set the color.
            if (!success) {
                board->cells[currPath].cell.col = col;
                change++;
            }
        }
    }

    // clean up mess. 
    free(path);
    free(parent);
    free(stack);
    
    return change;
}

// write more ?
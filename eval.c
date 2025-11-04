#include "eval.h"

//get_cell(board, parent / w, parent % w);

// Check if a board is completely solved
bool is_solved(board_t* board){
    bitmap_t* seen = bitmap_create(board->width, board->height);
    int w = board->width;

    key* stack = malloc(sizeof(key) * board->width * board->height);
    int ind = 0;

    // iterate thru sources to find all paths
    for (int i = 0; i < board->n; i += 2){
        key start = board->sources->keys[i];
        key goal = board->sources->keys[i + 1];

        stack[0] = start;
        ind = 1;
        bool success = false;

        while (ind){
            key curr = stack[--ind];

            if (curr == goal){
                success = true;
                break;
            }

            int r = curr / w;
            int c = curr % w;
            cellstate_t cell = get_cell(board, r, c);
            
            if (bitmap_get(seen, c, r)) continue;

            int edgeCount = cell.edges.n + cell.edges.e + cell.edges.s + cell.edges.w;
            
            if (edgeCount > 2){
                free(stack);
                bitmap_destroy(seen);
                return false;
            }
                

            if (cell.edges.n) stack[ind++] = get_index(w, r - 1, c);
            if (cell.edges.e) stack[ind++] = get_index(w, r, c + 1);
            if (cell.edges.s) stack[ind++] = get_index(w, r + 1, c);
            if (cell.edges.w) stack[ind++] = get_index(w, r, c - 1);
            
            bitmap_set(seen, c, r, 1);
        }

        if (!success){
            free(stack);
            bitmap_destroy(seen);
            return false;
        }
    }

    free(stack);
    bitmap_destroy(seen);
    return true;
}

// Ensures a space has valid edges
// TODO: Inline
bool valid_edges_single(board_t* board, int r, int c){
    // Ensure valid input
    #if DEBUG
        assert(r < board->height && c < board->width);
        assert(0 <= r && 0 <= c);
    #endif
    
    cellstate_t cell = get_cell(board, r, c);
    int edgeCount = cell.edges.n + cell.edges.s + cell.edges.e + cell.edges.w;
    
    if (!cell.node.isOccupied && edgeCount < (1 + !cell.node.isSource)){
        return false;
    }
    if (cell.node.isOccupied && edgeCount != (1 + !cell.node.isSource)){
        return false;
    }
    
    int occupiedEdgeCount = (cell.edges.n && get_cell(board, r - 1, c).node.isOccupied) +
                            (cell.edges.e && get_cell(board, r, c + 1).node.isOccupied) +
                            (cell.edges.s && get_cell(board, r + 1, c).node.isOccupied) +
                            (cell.edges.w && get_cell(board, r, c - 1).node.isOccupied);
    
    if (occupiedEdgeCount > 2 - cell.node.isSource) {
        return false;
    }
    return true;
}

// Check for impossible edge counts in the board
bool valid_edges(board_t* board){
    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            if (!valid_edges_single(board, r, c)) return false;            
        }
    }
    return true;
}

// Ensures colored edges are correct
// 2 cases:
// ### Case 1 - (r, c) color C is != EMPTY
// - in this case, we count for adjacent cells with color C (or empty). > 0 if source, > 1 ow
// ### Case 2 - (r, c) is empty (=> non source)
// - in this case, we check for matching adjacent cells. There must be one match
bool valid_colors_single(board_t* board, int r, int c){
    #if DEBUG
        assert(r < board->height && c < board->width);
        assert(0 <= r && 0 <= c);
    #endif

    cellstate_t cell = get_cell(board, r, c);
    
    // Ensure all occupied adjacent cells are matching color or empty
    cellstate_t nCell = NULL_CELL;
    cellstate_t eCell = NULL_CELL;
    cellstate_t sCell = NULL_CELL;
    cellstate_t wCell = NULL_CELL;
    
    if (cell.edges.n) nCell = get_cell(board, r - 1, c);
    if (cell.edges.s) sCell = get_cell(board, r + 1, c);
    if (cell.edges.e) eCell = get_cell(board, r, c + 1); 
    if (cell.edges.w) wCell = get_cell(board, r, c - 1);

    // Figures out how many edges are the same color as the node (or empty)
    if (cell.node.col != EMPTY){
        int colEdges =  (nCell.node.col == EMPTY || nCell.node.col == cell.node.col) +
                        (eCell.node.col == EMPTY || eCell.node.col == cell.node.col) +
                        (sCell.node.col == EMPTY || sCell.node.col == cell.node.col) +
                        (wCell.node.col == EMPTY || wCell.node.col == cell.node.col);

        if (colEdges < 1 + !cell.node.isSource){
            return false;
        }

        // Ensure no occupied adjacent cells are different colors (NULL_CELLs are not occupied)
        if (nCell.node.isOccupied && nCell.node.col != EMPTY && nCell.node.col != cell.node.col) return false;
        if (eCell.node.isOccupied && eCell.node.col != EMPTY && eCell.node.col != cell.node.col) return false;
        if (sCell.node.isOccupied && sCell.node.col != EMPTY && sCell.node.col != cell.node.col) return false;
        if (wCell.node.isOccupied && wCell.node.col != EMPTY && wCell.node.col != cell.node.col) return false;
    
        // Figures out how many edges match colors (or are empty)
    // Since source cells are colored from the start, these will never be source cells
    } else {
        bool match = false;
        
        // i.e. NULL_CELL
        if (nCell.node.col != -1){

            // Any other colored edge qualifies a match
            if (nCell.node.col == EMPTY){
                match = eCell.node.col != -1 || sCell.node.col != -1 || wCell.node.col != -1;
            
            // Must match north cell (or be empty also)
            } else {
                match = eCell.node.col == nCell.node.col || eCell.node.col == EMPTY ||
                        sCell.node.col == nCell.node.col || sCell.node.col == EMPTY ||
                        wCell.node.col == nCell.node.col || wCell.node.col == EMPTY;
            }
        }
        
        // skip if match already found
        if (!match && eCell.node.col != -1){
            // Any other colored edge qualifies a match
            if (eCell.node.col == EMPTY){
                match = sCell.node.col != -1 || wCell.node.col != -1;
            
            // Must match east cell
            } else {
                match = sCell.node.col == eCell.node.col || sCell.node.col == EMPTY ||
                        wCell.node.col == eCell.node.col || wCell.node.col == EMPTY;
            }
        }

        // skip if match already found
        if (!match && sCell.node.col != -1){
            // Any other colored edge qualifies a match
            if (sCell.node.col == EMPTY){
                match = wCell.node.col != -1;
            
            // Must match south cell
            } else {
                match = wCell.node.col == sCell.node.col || wCell.node.col == EMPTY;
            }
        }

        // No need for a west case, since it would have been found in one of the previous
        if (!match) {
            return false;
        }
    }

    // Do a check for loopbacks
    if (!cell.edges.n && r > 0) nCell = get_cell(board, r - 1, c);
    if (!cell.edges.e && c < board->width - 1) eCell = get_cell(board, r, c + 1);
    if (!cell.edges.s && r < board->height - 1) sCell = get_cell(board, r + 1, c);
    if (!cell.edges.w && c > 0) wCell = get_cell(board, r, c - 1);

    if (cell.node.col != EMPTY) {
        int matching =  (nCell.node.col == cell.node.col) + 
                        (eCell.node.col == cell.node.col) + 
                        (sCell.node.col == cell.node.col) + 
                        (wCell.node.col == cell.node.col);
        if (matching > 1 + !cell.node.isSource) {
            return false;
        }
    }
    return true;
}

// Check for conflicting colored edges
bool valid_colors(board_t* board){
    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            if (!valid_colors_single(board, r, c)){
                return false;
            }
        }
    }
    return true;
}

// Runs color/edge checks in the cell r, c and directly adjacent to r, c
bool validate_area(board_t* board, int r, int c){
    int w = board->width;
    int h = board->height;


    #if DEBUG
        assert(0 <= r && r <= h);
        assert(0 <= c && c <= w);
    #endif


    // Get bounds
    int lo_r = r == 0 ? 0 : r - 1;
    int hi_r = r == h - 1 ? h : r + 2;
    int lo_c = c == 0 ? 0 : c - 1;
    int hi_c = c == w - 1 ? w : c + 2;

    // Do the 3x3 area
    for (int r_i = lo_r; r_i < hi_r; r_i++){
        for (int c_i = lo_c; c_i < hi_c; c_i++){
            if (!(valid_edges_single(board, r_i, c_i) && valid_colors_single(board, r_i, c_i))){
                return false;
            }
        }
    }
    
    // // check all sides
    // bool n, e, s, w, p;
    
    // n = (r == 0) || ;
    // e = (c == wi - 1) || (valid_edges_single(board, r, c + 1) && valid_colors_single(board, r, c + 1));
    // s = (r == hi - 1) || (valid_edges_single(board, r + 1, c) && valid_colors_single(board, r + 1, c));
    // w = (c == 0) || (valid_edges_single(board, r, c - 1) && valid_colors_single(board, r, c - 1));
    // p = valid_edges_single(board, r, c) && valid_colors_single(board, r, c);

    return true;
}

// Ensure there is some path to connect all lines
bool validate_paths(board_t* board){
    int w = board->width;

    key* stack = malloc(4 * sizeof(key) * board->width * board->height);
    int ind = 0;

    // iterate thru sources and find a path
    for (int i = 0; i < board->n; i += 2){
        bitmap_t* seen = bitmap_create(board->width, board->height);
        key start = board->sources->keys[i];
        key goal = board->sources->keys[i + 1];

        stack[0] = start;
        ind = 1;
        bool success = false;

        while (ind){
            key curr = stack[--ind];

            if (curr == goal){
                success = true;
                break;
            }

            int r = curr / w;
            int c = curr % w;
            cellstate_t cell = get_cell(board, r, c);
            
            if (bitmap_get(seen, c, r)) continue;
                
            if (cell.edges.n) stack[ind++] = get_index(w, r - 1, c);
            if (cell.edges.e) stack[ind++] = get_index(w, r, c + 1);
            if (cell.edges.s) stack[ind++] = get_index(w, r + 1, c);
            if (cell.edges.w) stack[ind++] = get_index(w, r, c - 1);
            
            bitmap_set(seen, c, r, 1);
        }
        bitmap_destroy(seen);

        if (!success){
            free(stack);
            return false;
        }
    }

    free(stack);
    return true;
}

// Check if a move is valid/will throw an error (currently just checks colors)
bool validate_move(board_t* board, move_t move){
    int r = move.r;
    int c = move.c;

    cellstate_t cell = get_cell(board, r, c);
    cellstate_t fromCell = NULL_CELL;
    cellstate_t toCell = NULL_CELL;

    switch (move.from){
        case NORTH:
            fromCell = get_cell(board, r - 1, c);
        break;
        case SOUTH:
            fromCell = get_cell(board, r + 1, c);
        break;
        case EAST:
            fromCell = get_cell(board, r, c + 1);
        break;
        case WEST:
            fromCell = get_cell(board, r, c - 1);
        break;
    }

    switch (move.to){
        case NORTH:
            toCell = get_cell(board, r - 1, c);
        break;
        case SOUTH:
            toCell = get_cell(board, r + 1, c);
        break;
        case EAST:
            toCell = get_cell(board, r, c + 1);
        break;
        case WEST:
            toCell = get_cell(board, r, c - 1);
        break;
    }
    
    // Should be impossible
    #if DEBUG
        assert(fromCell.node.col != -1);
        assert(toCell.node.col != -1);
    #endif

    // All nonempty cells must match
    enum COLOR col = cell.node.col;

    // Check if the cell color and from cell match.
    if (col == EMPTY) col = fromCell.node.col;
    else if (fromCell.node.col != EMPTY && fromCell.node.col != col) return false;
    
    // If col is empty, last 2 were empty. Ow, they match. Ensure toCell is empty or matches
    if (col == EMPTY) return true;
    else return toCell.node.col == EMPTY || toCell.node.col == col;
}

// Check if the board is logically consistent
bool board_consistent(board_t* board){
    for (int r = 0; r < board->height; r++){
        for (int c = 0; c < board->width; c++){
            cellstate_t cell = get_cell(board, r, c);

            int availableEdges = cell.edges.n + cell.edges.e + cell.edges.s + cell.edges.w;
            
            cellstate_t nCell = cell.edges.n ? get_cell(board, r - 1, c) : NULL_CELL;
            cellstate_t eCell = cell.edges.e ? get_cell(board, r, c + 1) : NULL_CELL;
            cellstate_t sCell = cell.edges.s ? get_cell(board, r + 1, c) : NULL_CELL;
            cellstate_t wCell = cell.edges.w ? get_cell(board, r, c - 1) : NULL_CELL;

            int occupiedEdges = (cell.edges.n && nCell.node.isOccupied) + 
                                (cell.edges.e && eCell.node.isOccupied) + 
                                (cell.edges.s && sCell.node.isOccupied) + 
                                (cell.edges.w && wCell.node.isOccupied); 

            if (cell.node.isOccupied){
                if (cell.node.isSource) {
                    // Make sure there is only 1 edge
                    if (availableEdges != 1){
                        print_board(board);
                        printf("Inconsistent board: Found a source node at (%d, %d) with %d != 1 available edges. (Incorrect board)\n", r, c, availableEdges);
                        return false;
                    }

                    // Make sure edge that it points to is the same color
                    if ((cell.edges.n && (nCell.node.col != cell.node.col)) ||
                        (cell.edges.e && (eCell.node.col != cell.node.col)) ||
                        (cell.edges.s && (sCell.node.col != cell.node.col)) ||
                        (cell.edges.w && (wCell.node.col != cell.node.col)) ){
                        
                        print_board(board);
                        printf("Inconsistent board: Found a source node at (%d, %d) pointing to incorrect color. (Incorrect Board)\n", r, c);
                        return false;
                    }
                } else {
                    // Make sure there's only 2 edges
                    if (availableEdges != 2){
                        print_board(board);
                        printf("Inconsistent board: Found an occupied node at (%d, %d) with %d != 2 available edges. (Incorrect board)\n", r, c, availableEdges);
                        return false;
                    }
                                           
                    // Make sure cells that it points to match the color (including empty)             
                    if ((cell.edges.n && (nCell.node.col != cell.node.col)) ||
                        (cell.edges.e && (eCell.node.col != cell.node.col)) ||
                        (cell.edges.s && (sCell.node.col != cell.node.col)) ||
                        (cell.edges.w && (wCell.node.col != cell.node.col)) ){
                        
                        print_board(board);
                        printf("Inconsistent board: Found an occupied node at (%d, %d) pointing to incorrect color. (Incorrect board)\n", r, c);
                        return false;
                    }
                }
            } else {
                if (cell.node.isSource) {
                    // Make sure there are more than 1 available edge
                    if (availableEdges < 2){
                        print_board(board);
                        printf("Inconsistent board: Found an unoccupied source at (%d, %d) with %d < 2 available edges (%s).\n", r, c, availableEdges, availableEdges == 1 ? "Guaranteed Move" : "Incorrect Board");
                        return false;
                    }
                    
                    // If there is only 1 empty edge and other non-matching edges, return false
                    int emptyEdges = (cell.edges.n && nCell.node.col == EMPTY) + 
                                     (cell.edges.e && eCell.node.col == EMPTY) + 
                                     (cell.edges.s && sCell.node.col == EMPTY) + 
                                     (cell.edges.w && wCell.node.col == EMPTY);
                    
                    int matchingEdges = (cell.edges.n && (nCell.node.col == cell.node.col)) + 
                                        (cell.edges.e && (eCell.node.col == cell.node.col)) + 
                                        (cell.edges.s && (sCell.node.col == cell.node.col)) + 
                                        (cell.edges.w && (wCell.node.col == cell.node.col));
                    
                    // If there is only a single empty edge and all other edges do not match, it's a guaranteed move
                    if (emptyEdges == 1 && matchingEdges == 0) {
                        print_board(board);
                        printf("Inconsistent board: Found an unoccupied source at (%d, %d) with one valid edge (Guaranteed move).\n", r, c);
                        return false;
                    }

                    // If there's more than 1 matching edge, board is incorrect. If there is exactly, one, then there's a guaranteed move
                    if (matchingEdges > 0){
                        print_board(board);
                        printf("Inconsistent board: Found an unoccupied source at (%d, %d) with %d matching edges (%s).\n", r, c, matchingEdges, matchingEdges == 1 ? "Guaranteed Move" : "Incorrect board");
                        return false;
                    }

                    // If there is an occupied edge it's a guaranteed move.
                    if (occupiedEdges != 0) {
                        print_board(board);
                        printf("Inconsistent board: Found an unoccupied source at (%d, %d) with an occupied edge (%s).\n", r, c, occupiedEdges == 1 ? "Guaranteed Move" : "Incorrect board");
                        return false;
                    }
                } else {
                    // Make sure there are more than 2 available edges
                    if (availableEdges < 3){
                        print_board(board);
                        printf("Inconsistent board: Found an unoccupied cell at (%d, %d) with %d < 3 available edges (%s).\n", r, c, availableEdges, availableEdges == 2 ? "Guaranteed Move" : "Incorrect Board");
                        return false;
                    }

                    // Make sure there are less than 1 occupied edge
                    if (occupiedEdges > 1){
                        print_board(board);
                        printf("Inconsistent board: Found an unoccupied cell at (%d, %d) with %d occupied edges (%s).\n", r, c, occupiedEdges, occupiedEdges == 2 ? "Guaranteed Move" : "Incorrect Board");
                        return false;
                    }

                    if (cell.node.col == EMPTY) {
                        int coloredEdges =  (cell.edges.n && (nCell.node.col != EMPTY) && nCell.node.isOccupied) + 
                                            (cell.edges.e && (eCell.node.col != EMPTY) && eCell.node.isOccupied) + 
                                            (cell.edges.s && (sCell.node.col != EMPTY) && sCell.node.isOccupied) + 
                                            (cell.edges.w && (wCell.node.col != EMPTY) && wCell.node.isOccupied);

                        // Make sure no occupied edges have a color
                        if (coloredEdges != 0){
                            print_board(board);
                            printf("Inconsistent board: Found an unoccupied cell at (%d, %d) with %d colored occupied edges (Guaranteed Move).\n", r, c, coloredEdges);
                            return false;
                        }
                    } else {
                        int matchingEdges = (cell.edges.n && (nCell.node.col == cell.node.col)) + 
                                            (cell.edges.e && (eCell.node.col == cell.node.col)) + 
                                            (cell.edges.s && (sCell.node.col == cell.node.col)) + 
                                            (cell.edges.w && (wCell.node.col == cell.node.col));

                        // Make sure there's not multiple matching edges
                        if (matchingEdges > 1){
                            print_board(board);
                            printf("Inconsistent board: Found an unoccupied cell at (%d, %d) with %d matching colored edges (%s).\n", r, c, matchingEdges, matchingEdges == 2 ? "Guaranteed Move" : "Incorrect board");
                            return false;
                        }

                        int emptyEdges =    (cell.edges.n && (nCell.node.col == EMPTY)) + 
                                            (cell.edges.e && (eCell.node.col == EMPTY)) + 
                                            (cell.edges.s && (sCell.node.col == EMPTY)) + 
                                            (cell.edges.w && (wCell.node.col == EMPTY));
                        
                        // If there's one empty edge and no matching edges, there's a guaranteed move
                        if (emptyEdges == 1 && matchingEdges == 0){
                            print_board(board);
                            printf("Inconsistent board: Found an unoccupied cell at (%d, %d) with one open neighbor (Guaranteed Move).\n", r, c);
                            return false;
                        }

                        // All occupied edges should color match
                        if ((cell.edges.n && (nCell.node.col != cell.node.col) && nCell.node.isOccupied) ||
                            (cell.edges.e && (eCell.node.col != cell.node.col) && eCell.node.isOccupied) ||
                            (cell.edges.s && (sCell.node.col != cell.node.col) && sCell.node.isOccupied) ||
                            (cell.edges.w && (wCell.node.col != cell.node.col) && wCell.node.isOccupied) ){
                            
                            print_board(board);
                            printf("Inconsistent board: Found an unoccupied cell at (%d, %d) with miscolored occupied cell (Incorrect board).\n", r, c);
                            return false;
                            
                        }
                    }
                }
            }
        }
    }
    return true;
}
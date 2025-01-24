// This is for the level editor system. There'll prolly be a GUI
#include "leveleditor.h"

void setup_pairs(){
    /*
        Assigned all colors to their index + 1
    */
    init_pair(1, COLOR_RED,     COLOR_BLACK);
    init_pair(2, COLOR_GREEN,   COLOR_BLACK);
    init_pair(3, COLOR_BLUE,    COLOR_BLACK);
    init_pair(4, COLOR_YELLOW,  COLOR_BLACK);
    init_pair(5, 8, COLOR_BLACK);
    init_pair(6, COLOR_CYAN,    COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(8, 9, COLOR_BLACK);
    init_pair(9, 10, COLOR_BLACK);
    init_pair(10, COLOR_WHITE, COLOR_BLACK);
    init_pair(11, 11, COLOR_BLACK);
    init_pair(12, 12, COLOR_BLACK);
    init_pair(13, 13, COLOR_BLACK);
    init_pair(14, 14, COLOR_BLACK);
    init_pair(15, 15, COLOR_BLACK);
    init_pair(16, 16, COLOR_BLACK);

    // 8 - ORANGE
    init_color(8, 921, 554, 203);

    // 9 - MAROON
    init_color(9, 590, 199, 183);

    // 10 - PURPLE
    init_color(10, 457, 78, 477);
    
    // 11 - GREY
    init_color(11, 629, 621, 726);

    // 12 - LIME
    init_color(12, 457, 980, 297);

    // 13 - TAN
    init_color(13, 617, 535, 336);

    // 14 - INDIGO
    init_color(14, 195, 160, 668);

    // 15 - AQUA
    init_color(15, 211, 488, 492);

    // 16 - PINK
    init_color(16, 930, 516, 890);

}

void setup_leditor(){
    initscr();
    keypad(stdscr, TRUE);
    nonl(); 
    cbreak();

    curs_set(0);
    if (has_colors()) {
        start_color();
        setup_pairs();
    }
}

void clear_content(){
    clear();
    box(stdscr, 0, 0);
    mvprintw(0, 2, "Level Editor V0.1");
    refresh();
}

/*
    Prompts the user for an option between creating, loading, and 
    quitting
    Returns 0 if the user wants to create a new pack and 1 if
    the user wants to load from the packs/ directory. Returns 2
    if the user wants to quit
*/
enum MAIN_FUNCTION main_menu(){
    char** options = malloc(3 * sizeof(char*));
    options[0] = "[0] Create new Level Pack ";
    options[1] = "[1] Load existing Level Pack ";
    options[2] = "[2] Quit";
    enum MAIN_FUNCTION curr_option = 0;

    mvprintw(2, 3, "Select Function:");

    // loop for valid input or selection
    while(true) {
        for (int i = 0; i < 3; i++){
            if (i == curr_option){
                attron(A_REVERSE);
            }
            mvprintw(4 + i, 4, options[i]);
            attroff(A_REVERSE); 
        }

        move(2, 20);

        
        refresh();
        int last = getch();
        bool exit = false;

        switch (last)
        {
        case '0':
            curr_option = CREATE;
            exit = true;
        break;
        case '1':
            curr_option = LOAD;
            exit = true;
        break;
        case '2':
            curr_option = QUIT;
            exit = true;
        break;
        case KEY_UP:
            curr_option = curr_option == CREATE ? CREATE : curr_option - 1;
        break;
        case KEY_DOWN:
            curr_option = curr_option == QUIT ? QUIT : curr_option + 1;
        break;
        default:
        break;
        }

        if (last == 13 || exit){
            break;
        }
    }

    // Reset screen to empty
    clear_content();

    return curr_option;
}

/*
    Prompts the user for a file name. Writes to filename
    and returns the length of the name.

    TODO: Add a confirmation
*/
int prompt_file(char* filename){
    // Set up string input
    echo();
    keypad(stdscr, FALSE);

    while(true){
        clear_content();

        // Prompt for name of level pack
        mvprintw(2, 3, "Enter the name of the new file");
        if (filename[0] != '\0') printw(" (Enter for %s.flow)", filename);
        move(4, 3);
        refresh();
        
        
        bool valid = true;
        
        // Ensuring a null input is read as such
        //filename[0] = '\0';

        // Get the inputted filename
        mvscanw(4, 3, "%s", filename);

        int len = 0;

        // Must only contain letters and 
        // numbers and underscores/hyphens
        for (char* j = filename; *j != '\0'; j++){
            char curr = *j;
            if (!((curr >= 'A' && curr <= 'Z') ||
            (curr >= 'a' && curr <= 'z') || 
            (curr >= '0' && curr <= '9') ||
            (curr == '_') || (curr == '-'))) {
                valid = false;
                break;
            }
            len++;
        }

        // Must be more than 0 chars (I'm not doing a
        // default option yet)
        if (len == 0) valid = false;

        if (valid){
            // Reset char input
            keypad(stdscr, TRUE);
            noecho();
            clear_content();
            refresh();
            return len;
        }
    }

}

/*
    Prompts user to choose a file
*/
int choose_file(int num, char* filenames){
    // Goes from 0 - num
    int option = 0;

    while(true){
        // draw everything
        clear_content();
        mvprintw(1, 1, "Choose file: ");

        // draw all files
        for (int i = 0; i < num; i++){
            if (option == i) attron(A_REVERSE);
            mvprintw(2 + i, 1, "%s", filenames + 32 * i);
            attroff(A_REVERSE);
        }

        refresh();

        // process input
        int last = getch();
        switch (last) {
            case KEY_UP:
                option = option > 0 ? option - 1 : option;
            break;
            case KEY_DOWN:
                option = option < num - 1 ? option + 1 : option;
            break;
            default:
            break;
        }
        if (last == 13){
            return option;
        }
    }
}

/*
    Draws the board at (3, 1)
    Also removes out of bounds nodes from board
*/
void draw_board(unsolved_board_t* board){
    
    mvvline(3, 1, '|', 1 + board->height);
    mvvline(3, 2 + board->width, '|', 1 + board->height);

    attron(A_UNDERLINE);
    mvhline(3, 1, ' ', 1 + 2 * board->width);
    for (int i = 0; i < board->height; i++){
        move(4 + i, 2);
        for (int j = 0; j < board->width; j++){
            printw(" |");
        }
    }

    for (int i = 0; i < board->n; i++){
        int y1 = board->positions[4 * i];
        int x1 = board->positions[4 * i + 1];

        int y2 = board->positions[4 * i + 2];
        int x2 = board->positions[4 * i + 3];

        // Color pair 0 is white/black. 1 is Red and so on
        attron(COLOR_PAIR(i + 1));

        // if valid, draw
        if (x1 != -1 && y1 != -1 ){
            if (x1 < board->width && y1 < board->height){
                mvaddch(4 + y1, 2 + 2 * x1, '0');
            } else { // Catch invalid indices
                board->positions[4 * i] = -1;
                board->positions[4 * i + 1] = -1;
            }
        }

        if (x2 != -1 && y2 != -1){
            if (x2 < board->width && y2 < board->height){
                mvaddch(4 + y2, 2 + 2 * x2, '1');
            } else { // Catch invalid indices
                board->positions[4 * i + 2] = -1;
                board->positions[4 * i + 3] = -1;
            }
        }
        attroff(COLOR_PAIR(i + 1));
    }
    attroff(A_UNDERLINE);

    move(100, 1);
}

/*
    Verifies that all of a board's nodes are present on the board
*/
bool valid_board(unsolved_board_t* board){
    for (int i = 0; i < 2 * board->n; i++){
        // Check if coordinate is not set
        if (board->positions[2 * i] == (uint8_t) -1 || 
            board->positions[2 * i + 1] == (uint8_t) -1){
            return false;
        }
        // Check for any duplicates
        for (int j = i + 1; j < 2 * board->n; j++){
            if ((board->positions[2 * i] == board->positions[2 * j]) &&
                 board->positions[2 * i + 1] == board->positions[2 * j + 1]){
                return false;
            }
        }
    }

    return true;
}

/*
    Handles interacting with the board
*/
void level_editor_nodes(unsolved_board_t* board){
    // position within the editor
    int x = 0;
    int y = 0;

    // selected the content or not
    bool selected = false;

    /*
        0 - Setting color
        1 - Setting source
    */
    bool curr_option = 0;

    /*
        For setting nodes
    */
    int color = 0;
    bool source = 0;

    
    while(true) {
        // draw everything
        clear_content();

        if (selected){
            move(1, 1);
            if (curr_option == 0){
                attron(A_REVERSE);
                attron(COLOR_PAIR(color+1));
            }
            printw(" Color: %s", COLOR_NAMES[color]);
            attroff(A_REVERSE);
            attroff(COLOR_PAIR(color+1));
            if(curr_option == 1) attron(A_REVERSE);
            printw(" Source: %d", source);
            attroff(A_REVERSE);
            
        } else{
            mvprintw(1, 1, "[Arrows - Move] [Enter - Add Node] [Q - Quit]");
        }
        
        draw_board(board);

        // highlights current node
        curs_set(1);
        move(4 + y, 2 + 2 * x);
        // process input
        int last = getch();
        curs_set(0);
        switch(last){
            case KEY_UP:
                if (!selected && y > 0){
                    y--;
                }
                if (selected){ // Mess with Menu
                    if (!curr_option){
                        color = (color + 1) % board->n;
                    } else { // Flip Type
                        source = !source;
                    }
                }
            break;
            case KEY_DOWN:
                if (!selected && y < board->height - 1){
                    y++;
                }
                if (selected){ // Mess with Menu
                    if (!curr_option){ 
                        color--;
                        if (color < 0) color = board->n - 1;
                    } else { // Flip Type
                        source = !source;
                    }
                }
            break;
            case KEY_LEFT:
                if (!selected && x > 0){
                    x--;
                }
                if (selected){
                    curr_option = 0;
                }
            break;
            case KEY_RIGHT:
                if (!selected && x < board->width - 1){
                    x++;
                }
                if (selected){
                    curr_option = 1;
                }
            break;
            default:
            break;
        }
        if (last == 'q'){
            if (!selected) break;
            selected = false;
        } else if (last == 13){
            if (selected){ // Set corresponding node
                int ind = 4 * color + 2 * source;
                board->positions[ind] = y;
                board->positions[ind + 1] = x;
            }
            selected = !selected;
            curr_option = 0;
        }
    }
}

/*
    Handles the top menu of the editor
*/
void level_editor_menu(unsolved_board_t* board){
    // Initialize board
    if (board->width == 0){
        board->width = 5;
        board->height = 5;
        board->n = 3;
        board->positions = malloc(64 * sizeof(uint8_t));
        for (int i = 0; i < 64; i++){
            board->positions[i] = -1;
        }
    }

    /*
        Between 0 - 4
        0 - Change width
        1 - Change Height
        2 - Change N
        3 - Edit Positions
        4 - Exit
    */
    int curr_option = 0;

    while(true){
        // draw everything

        clear_content();
        // Draw the Menu
        move(1, 1);  
        if (curr_option == 0) attron(A_REVERSE);
        printw(" W:%2d ", board->width);
        attroff(A_REVERSE);
        if (curr_option == 1) attron(A_REVERSE);
        printw(" H:%2d ", board->height);
        attroff(A_REVERSE);
        if (curr_option == 2) attron(A_REVERSE);
        printw(" N:%2d ", board->n);
        attroff(A_REVERSE);
        if (curr_option == 3) attron(A_REVERSE);
        printw(" Nodes ");
        attroff(A_REVERSE);
        if (curr_option == 4) attron(A_REVERSE);
        if(!valid_board(board)) attron(COLOR_PAIR(1)); // Red if invalid board
        printw(" Exit ");
        attroff(A_REVERSE);
        attroff(COLOR_PAIR(1));

        // Draw the board
        draw_board(board);

        refresh();
        int last = getch();

        // process the input
        switch(last) {
            case KEY_UP:
            switch(curr_option) {
                case 0:
                    if (board->width < MAX_BOARD_SIDE) board->width += 1;
                break;
                case 1:
                    if (board->height < MAX_BOARD_SIDE) board->height += 1;
                break;
                case 2:
                    if (board->n < MAX_N) {
                        board->n += 1;
                    }
                break;
                default:
                break;
            }
            break;
            case KEY_DOWN:
            switch(curr_option) {
                case 0:
                    if (board->width > 5) board->width -= 1;
                break;
                case 1:
                    if (board->height > 5) board->height -= 1;
                break;
                case 2:
                    if (board->n > 3) board->n -= 1;
                break;
                default:
                break;
            }
            break;
            case KEY_LEFT:
                if (curr_option > 0){
                    curr_option--;
                }
            break;
            case KEY_RIGHT:
                if (curr_option < 4){
                    curr_option++;
                }
            break;
            default:
            break;
        }

        if (last == 13){
            // Do the next menu
            if (curr_option == 3){
                level_editor_nodes(board);
            }
            // Exit
            if (curr_option == 4){
                break;
            }
        }
    }
}

void level_pack_editor(level_pack_t* lp, char* filename){
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    
    // Make the editor
    int curr_level = 0;

    /*
        Between 0 - 4
        0 - level_sel
        1 - Edit Level
        2 - Save Levelpack
        3 - Exit
    */
    int curr_option = 0;

    while(true){
        clear_content();

        move(1, 1);  
        if (curr_option == 0) attron(A_REVERSE);
        printw(" %3d ", curr_level + 1);
        attroff(A_REVERSE);
        if (curr_option == 1) attron(A_REVERSE);
        printw(" Edit ");
        attroff(A_REVERSE);
        if (curr_option == 2) attron(A_REVERSE);
        printw(" Save ");
        attroff(A_REVERSE);
        if (curr_option == 3) attron(A_REVERSE);
        printw(" Exit ");
        attroff(A_REVERSE);
        
        // draw the menu
        mvhline(2, 1, '=', xMax-2);

        // Finish drawing the menu and also add a way to escape using exit
        
        // Prompt user to initialize a new board
        if (curr_level == lp->num_levels){
            mvprintw(3, 1, "Select Edit to create a board");
        } else{ // Show current board on screen
            draw_board(&(lp->levels[curr_level]));
        }

        //mvprintw(29, 10, "Cur option: %d, cur level: %d", curr_option, curr_level);
        
        refresh();
        // process next input
        int last = getch();

        switch(last){
            case KEY_UP:
                if (curr_option == 0 && curr_level < lp->num_levels){
                    curr_level++;
                }
            break;
            case KEY_DOWN:
                if (curr_option == 0 && curr_level > 0){
                    curr_level--;
                }
            break;
            case KEY_LEFT:
                if (curr_option > 0){
                    curr_option--;
                }
            break;
            case KEY_RIGHT:
                if (curr_option < 3){
                    curr_option++;
                }
            break;
            default:
            break;
        }
        if (last == 13){
            switch (curr_option)
            {
            case 1:
            // Edit Level

            // width = 0 assumes i'll need to start from scratch. 
            if (curr_level == lp->num_levels){
                lp->levels[curr_level].width = 0;
                lp->num_levels += 1;
            }
            
            // Run Level Editor Here
            level_editor_menu(&(lp->levels[curr_level]));

            // Assume level has been edited - Reset to option = 0
            curr_option = 0;
            break;
            case 2:
            // Save level

            prompt_file(filename);
            save_level_pack(lp, filename);

            break;           
            default:
            // level_sel, exit do nothing
            break;
            }

            // Exit
            if (curr_option == 3){
                break;
            }
        }
    }
}

int main(){
    // Set up ncurses
    setup_leditor();

    // Set up necessary vars
    level_pack_t* lp = malloc(sizeof(level_pack_t));
    char filename[32];
    bool edited = false;

    while (true){
        int len = 0;
        // Remove all the content from the screen
        clear_content();

        // Choose option from main menu
        int main_choice = main_menu();

        switch (main_choice) {
        case CREATE: {
            // Create an empty DS
            lp->num_levels = 0;
            lp->levels = malloc(256 * sizeof(unsolved_board_t));
            filename[0] = '\0';
        }
        break;
        case LOAD: {
            // Search for packs/*.flow
            int total_packs = count_packs();
            char* all_names = calloc(total_packs * 32, sizeof(char));
            
            
            get_pack_names(all_names);


            int pack = choose_file(total_packs, all_names);

            // Write into filename
            for (char* i = (all_names + 32 * pack); *i != '\0'; i++){
                filename[len] = *i;
                len++;
            }
            filename[len] = '\0';

            for (int i =0; i < len; i++){
                assert(filename[i] == *(all_names + 32 * pack + i));
            }
            
            // Load DS
            bool success = load_level_pack(lp, filename);
            if (!success){
                mvprintw(0, 0, "Failed to find [%s]", filename);
                refresh();
                getch();
            }
        }
        break;
        case QUIT:
            // Quit the application
            finish(0);
            // Free everything
            if (edited){
                for (int i = 0; i < lp->num_levels; i++){
                    free(lp->levels[i].positions);
                }
                free(lp->levels);
            }
            free(lp);
            return 1;
        break;
        default:
            // Should never happen
            assert(0);
        break;
        }
        
        // Edit the level pack
        edited = true;
        level_pack_editor(lp, filename);
    }
}

static void finish(int sig)
{
    endwin();

    /* do your non-curses wrapup here */

    return;
}

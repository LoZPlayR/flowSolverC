#include "levelpack.h"

/*
    Takes a char* and returns packs/filename.flow
*/
void add_fixes(char* filename, char* out){
    char prefix[7] = "packs/";
    char suffix[6] = ".flow";
    
    int len = 0;
    for (char* i = prefix; *i != '\0'; i++){
        out[len] = *i;
        len++;
    }

    for (char* i = filename; *i != '\0'; i++){
        out[len] = *i;
        len++;
    }

    for (char* i = suffix; *i != '\0'; i++){
        out[len] = *i;
        len++;
    }
    out[len] = '\0';
}

/*
    Returns the number of .flow files in the directory
    Assumes everything in packs is a flow file lmao

    TODO: Make only accept .flow files
*/
int count_packs(){
    DIR* dir = opendir("packs/");
    struct dirent *entry;

    int packs = 0;
    entry = readdir(dir);
    while(entry){
        int len = 0;
        for (char* i = entry->d_name; *i != '\0'; i++){
            len++;
        }
        if (len > 6) packs++;
        entry = readdir(dir);
    }
    return packs;
}

/*
    Returns a list of all pack names in the packs/
    directory
*/
void get_pack_names(char* list){
    DIR* dir = opendir("packs/");
    struct dirent *entry;

    int valid = 0;
    entry = readdir(dir);
    
    while(entry){
        if (entry == NULL) break;
        // Get length of string
        int len = 0;
        for (char* i = entry->d_name; *i != '\0'; i++){
            len++;
        }

        if (len < 6){
            entry = readdir(dir);
            continue;
        }

        // add to list
        for (int i = 0; i < len - 5; i++){
            list[32 * valid + i] = entry->d_name[i];
        }

        list[32 * valid + len + 1] = '\0';


        // Add entry to list
        valid++;
        entry = readdir(dir);
    }
    closedir(dir);
}

/*
    Given a levelpack and a file name, compresses and saves a .flow file 
    in the packs/directory
*/
void save_level_pack(level_pack_t* lp, char* filename){
    FILE* curr_file;

    char fullname[45]; // 32 limit + 13 chars for dir + extension
    add_fixes(filename, fullname);

    // Open the file in binary write mode
    curr_file = fopen(fullname, "wb");

    assert(curr_file != NULL);

    // Write level_pack_header
    level_pack_header_t* lph = malloc(sizeof(level_pack_header_t));
    lph->num_levels = lp->num_levels;
    fwrite(lph, sizeof(level_pack_header_t), 1, curr_file);
    free(lph);

    // Start writing boards
    for (int i = 0; i < lp->num_levels; i++){
        unsolved_board_t board = lp->levels[i];

        // Make & Write board header
        board_header_t* bh = malloc(sizeof(board_header_t));
        bh->size.width = board.width - 5;
        bh->size.height = board.height - 5;
        bh->n = board.n;
        fwrite(bh, sizeof(board_header_t), 1, curr_file);
        free(bh);

        int compressed_len = 2 * board.n + (board.n / 4) + (board.n % 4 != 0);

        uint8_t* positions = malloc(sizeof(uint8_t) * compressed_len);

        int curr_source = 0;
        int offset = 0;

        // Can be shifted to make a mask
        uint8_t MASK = (uint8_t) -1;

        // Compress positions into 9 bits and write out
        for (int byte = 0; byte < compressed_len; byte++){
            // Try to ignore endianness lmao
            // Convert current source to index
            
            // Only need to take from 1 position
            if (offset < 2){
                // Convert to index format
                uint16_t curr_val = board.positions[2 * curr_source] * 
                                    board.width + board.positions[2 * curr_source + 1];
                
                // Grab either first 8 or last 8 bits
                positions[byte] = (uint8_t)(curr_val >> (offset));

                // Moves to next source as necessary
                if (offset) curr_source++;
            }
            else{
                // Convert to index format
                uint16_t curr_val = board.positions[2 * curr_source] * 
                                    board.width + board.positions[2 * curr_source + 1];
                
                uint16_t next_val = board.positions[2 * curr_source + 2] * 
                                    board.width + board.positions[2 * curr_source + 3];
                
                uint8_t top_bits = ((uint8_t) (curr_val >> (offset))) & (MASK >> (offset - 1));
                uint8_t bot_bits = ((uint8_t) (next_val));
                
                positions[byte] = top_bits | (bot_bits << (9 - offset));

                curr_source++;
            }

            // update offset
            if (offset == 0) offset = 8;
            else offset -= 1;
        }

        fwrite(positions, sizeof(uint8_t), compressed_len, curr_file);
        free(positions);
    }
    fclose(curr_file);
}

/*
    Given a filename, loads the levelpack into lp
    lp should be allocated. Everything should be freed
    by the caller
*/
bool load_level_pack(level_pack_t* lp, char* filename){
    FILE* curr_file;

    char fullname[45]; // 32 limit + 13 chars for dir + extension
    add_fixes(filename, fullname);

    // Open the file in binary read mode
    curr_file = fopen(fullname, "rb");

    if (curr_file == NULL) return false;

    // Use the header to get the number of levels
    level_pack_header_t* lph = malloc(sizeof(level_pack_header_t));
    fread(lph, sizeof(level_pack_header_t), 1, curr_file);
    lp->num_levels = lph->num_levels;
    
    free(lph);

    // Initialize memory for all levels.
    lp->levels = malloc(sizeof(unsolved_board_t) * 256);
    
    // Start reading boards
    for (int i = 0; i < lp->num_levels; i++){
        // Read board header
        board_header_t* bh = malloc(sizeof(board_header_t));
        fread(bh, sizeof(board_header_t), 1, curr_file);

        lp->levels[i].width = bh->size.width + 5;
        lp->levels[i].height = bh->size.height + 5;
        lp->levels[i].n = bh->n;


        lp->levels[i].positions = malloc(sizeof(uint8_t) * 64);

        int board_n = bh->n;
        free(bh);

        int offset = 0;

        // Can be shifted to make a mask
        uint8_t MASK = (uint8_t) -1;

        // keep track of prev and curr byte
        uint8_t prev, curr;
        fread(&curr, sizeof(uint8_t), 1, curr_file);

        for (int byte = 0; byte < 2 * board_n; byte++){
            if (offset == 8){
                offset = 0;
                // At this point, we need to skip a byte ahead
                prev = curr;
                fread(&curr, sizeof(uint8_t), 1, curr_file);
            }
            
            prev = curr;
            fread(&curr, sizeof(uint8_t), 1, curr_file);

            assert(board_n <= 16);
            assert(byte <= 31);

            // reconstruct 16 bit number
            uint8_t bot_bits = (prev >> (offset));
            uint8_t top_bits = (curr & (MASK >> (7 - offset)));

            uint16_t index = ((uint16_t) bot_bits) | (((uint16_t) top_bits) << (8 - offset));

            assert(2 * byte < 64);
            assert(2 * byte + 1 < 64);

            // Save to DS
            lp->levels[i].positions[2 * byte] = index / lp->levels[i].width;
            lp->levels[i].positions[2 * byte + 1] = index % lp->levels[i].width;

            // Update offset
            offset++;
        }
    }
    fclose(curr_file);
    return true;
}
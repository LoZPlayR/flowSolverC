#ifndef LEVELPACK_H
#define LEVELPACK_H

#include "util.h"
#include <dirent.h>

/*
    Uncompressed Format
*/

typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t n;
    uint8_t* positions;
} unsolved_board_t;

typedef struct {
    uint8_t num_levels;
    unsolved_board_t* levels;
} level_pack_t;

/*
    Compressed Format
*/

// File:
// [levelpack_header][board_header1][board_1_positions][board2_header]
// <-----1 byte-----><---2 bytes---><-2*n+n/4+1 bytes-><---2 bytes--->

// Reading an writing should be SIMPLE :D

typedef struct __attribute__((packed)){
    uint8_t num_levels;
} level_pack_header_t;

typedef struct __attribute__((packed)) {
    uint8_t width : 4;
    uint8_t height: 4;
} board_size_t;

typedef struct __attribute__((packed)){
    board_size_t size;
    uint8_t n;
} board_header_t;

/*
    Methods
*/

// Count the number of total packs
int count_packs();

// Get the names of all the packs
void get_pack_names(char* list);

// Write a level to a given filename
void save_level_pack(level_pack_t *lp, char* filename);

// Load a levelpack from a given filename into a given levelpack
bool load_level_pack(level_pack_t *lp, char* filename);

#endif
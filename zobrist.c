
#include "zobrist.h"

// Initializes the zob array
static zob_t* init_zob(int height, int width, int n){
    zob_t* zob = malloc(sizeof(zob_t));
    
    zob->h = height;
    zob->w = width;
    zob->n = n;
    
    // I need one number for each color at each square
    zob->arr = malloc(sizeof(zob_key_t) * height * width * n);

    // Fill with random numbers for now
    // TODO: Make actually random
    for (int i = 0; i < height * width * n; i++){
        zob->arr[i] = i;
    }

    return zob;
}

// Toggles the color at loc
static zob_key_t update_zob(zob_t* zob, zob_key_t prev, int loc, int col){
    // Since I will end up doing mostly toggle on/toggle off at the same pos,
    // it makes more sense to do all colors next to each other
    // For a 5x5 n=3:
    // [r00, g00, b00, r01, g01, b01, ... r44, g44, b44]

    // I am doing a lil rep dependency here.
    // I could to a function to actually get the rowcol 
    // vs index etc.

    return prev ^ zob->arr[zob->n * loc + col];
}

// Destroys the zob array
static void destroy_zob(zob_t* zob){
    free(zob->arr);
    free(zob);
}
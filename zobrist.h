// I need one number for each color at each square

#ifndef ZOB_H
#define ZOB_H
#include "util.h"


typedef uint32_t zob_key_t;

// Some packing can be done here in the future
typedef struct {
    zob_key_t* arr;
    int h;
    int w;
    int n;
} zob_t;

static zob_t* init_zob(int height, int width, int n);

static zob_key_t update_zob(zob_t* zob, zob_key_t prev, int loc, int col);

static void destroy_zob(zob_t* zob);

#endif
#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stdbool.h>


// bitmap_t structure
typedef struct {
    uint8_t* data;    // Pointer to the bitmap data
    int width;        // Width in pixels
    int height;       // Height in pixels
} bitmap_t;

// Create a new bitmap with given width and height
bitmap_t* bitmap_create(int width, int height);

// Free the memory used by a bitmap
void bitmap_destroy(bitmap_t* bmp);

// Set a pixel (x, y) to value (0 or 1)
void bitmap_set(bitmap_t* bmp, int x, int y, bool value);

// Get the value of a pixel (x, y)
bool bitmap_get(const bitmap_t* bmp, int x, int y);

// Sets bitmap to 1 except boarder
void bitmap_initialize(bitmap_t* bmp);

// Prints out a bitmap
void bitmap_print(const bitmap_t* bmp);

// #include "board.h"

// // Set a pixel ind to value (0 or 1)
// void bitmap_set_ind(bitmap_t* bmp, key ind, bool value);

// // Set a pixel ind to value (0 or 1)
// void bitmap_get_ind(bitmap_t* bmp, key ind);

#endif // BITMAP_H
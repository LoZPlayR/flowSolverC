#include "bitmap.h"
#include <stdlib.h>
#include "util.h"

bitmap_t* bitmap_create(int width, int height) {
    if (DEBUG) {
        assert(width > 0 && height > 0);
    }

    bitmap_t* bmp = (bitmap_t*)malloc(sizeof(bitmap_t));
    if (!bmp) return NULL;

    bmp->width = width;
    bmp->height = height;

    int num_bits = width * height;
    int num_bytes = (num_bits + 7) / 8; // Round up to nearest byte

    bmp->data = (uint8_t*)calloc(num_bytes, sizeof(uint8_t));
    
    if (!bmp->data) {
        free(bmp);
        return NULL;
    }

    return bmp;
}

// Initialize bitmap
// Last bit of row i is equal to !(i % 2)
// Other border bits are 0
// All non-boarder bits are 1
void bitmap_initialize(bitmap_t* bmp){
    // TODO: Optimize
    for (int y = 0; y < bmp->height; y++){
        for (int x = 0; x < bmp->width; x++){
            // Last bit of row i is equal to !(i % 2)
            if (x == bmp->width - 1){
                bitmap_set(bmp, x, y, (y + 1) % 2);
            }
            // All other border bits are 0
            if (x == 0 || y == 0 || y == bmp->height - 1){
                bitmap_set(bmp, x, y, 0);
            } else if (x != bmp->width - 1){
                // Not on the border, set to 1
                bitmap_set(bmp, x, y, 1);
            }
        }
    }
}

void bitmap_destroy(bitmap_t* bmp) {
    if (bmp) {
        free(bmp->data);
        free(bmp);
    }
}

void bitmap_set(bitmap_t* bmp, int x, int y, bool value) {
    if (DEBUG) {
        assert(bmp != NULL);
        assert(x >= 0 && y >= 0 && x < bmp->width && y < bmp->height);
    }

    int bit_index = y * bmp->width + x;
    int byte_index = bit_index / 8;
    int bit_offset = bit_index % 8;

    if (value)
        bmp->data[byte_index] |= (1 << bit_offset);
    else
        bmp->data[byte_index] &= ~(1 << bit_offset);
}

bool bitmap_get(const bitmap_t* bmp, int x, int y) {
    if (!bmp || x < 0 || y < 0 || x >= bmp->width || y >= bmp->height) return false;

    int bit_index = y * bmp->width + x;
    int byte_index = bit_index / 8;
    int bit_offset = bit_index % 8;

    return (bmp->data[byte_index] >> bit_offset) & 1;
}

// bool bitmap_get_ind(const bitmap_t* bmp, key ind) {
//     if (!bmp || ind < 0 || key >= bmp->width * bmp->height) return false;

//     int bit_index = ind;
//     int byte_index = bit_index / 8;
//     int bit_offset = bit_index % 8;

//     return (bmp->data[byte_index] >> bit_offset) & 1;
// }

void bitmap_print(const bitmap_t* bmp){
    for (int y = 0; y < bmp->height; y ++){
        for (int x = 0; x < bmp->width; x++){
            bool a = bitmap_get(bmp, x, y);
            printf("%d ", a);
        }
        printf("\n");
    }
}
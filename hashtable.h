#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "util.h"

// Naive hashset implementation
// No need for removing, resizing, etc
// Gonna do O(n) for all functions, since n < 16.

// Methods:
// add(key, item): void
// get(key): item
// contains(key): bool

typedef int key;
typedef int item;

typedef struct {
    key* keys;      // Array of keys
    item* items;    // Array of items
    int maxSize;    // Max size of the hashtable
    int curSize;    // Current number of items in the hashtable
} hashtable_t;

// Create a hashtable of given size
hashtable_t* h_create(int size);

// Add a key-item pair to the hashtable
void h_add(hashtable_t* hash, key k, item i);

// Get the item given a key
item h_get(hashtable_t* hash, key k);

// Check if a key is in the hashtable
bool h_contains(hashtable_t* hash, key k);

// Destroy the hashtable
void h_destroy(hashtable_t* hash);

#endif
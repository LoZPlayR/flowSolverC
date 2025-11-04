#include "hashtable.h"

hashtable_t* h_create(int size){
    hashtable_t* hash = malloc(sizeof(hashtable_t));
    hash->curSize = 0;
    hash->maxSize = size;

    hash->keys = malloc(size * sizeof(key));
    hash->items = malloc(size * sizeof(item));
    
    return hash;
}

// Add an item to the hashtable
void h_add(hashtable_t* hash, key k, item i){
    if (DEBUG) {
        assert(hash->curSize < hash->maxSize);
    }
    hash->items[hash->curSize] = i;
    hash->keys[hash->curSize] = k;
    hash->curSize += 1;
}

// Check if key value pair is present in hash table
bool h_contains(hashtable_t* hash, key k) {
    for (int i = 0; i < hash->curSize; i++){
        if (hash->keys[i] == k){
            return true;
        }
    }
    return false;
}

// Get an item from the hash table
item h_get(hashtable_t* hash, key k){
    if (DEBUG) {
        assert(h_contains(hash, k));
    }

    for (int i = 0; i < hash->curSize; i++){
        if (hash->keys[i] == k){
            return hash->items[i];
        }
    }

    return 0;
}

// Free a hash table
void h_destroy(hashtable_t* hash){
    free(hash->items);
    free(hash->keys);
    free(hash);
}

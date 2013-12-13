#include "Cache.h"
#include <stdlib.h>

Line *createLine() {
    Line *line = malloc(sizeof(Line));
    line->validBit = 0;
    line->tag = 0;
    line->insertionAge = 0;
    line->usageAge = 0;
    return line;
}

void destroyLine(Line *line) {
    free(line);
}

Set *createSet(int numLines) {
    Set *set = malloc(sizeof(Set));
    set->numLines = numLines;
    set->lines = malloc(sizeof(Line *) * numLines);
    int i;
    for(i = 0; i < numLines; i++) {
        set->lines[i] = createLine();
    }
    return set;
}

void destroySet(Set *set) {
    int i;
    for(i = 0; i < set->numLines; i++) {
        destroyLine(set->lines[i]);
    }
    free(set);
}

Cache *createCache(int type, int size, int association, int blockSize, int numSets) {
    Cache *cache = malloc(sizeof(Cache));
    cache->type = type;
    cache->size = size;
    cache->association = association;
    cache->blockSize = blockSize;
    cache->numSets = numSets;
    cache->storage = malloc(sizeof(Set *) * numSets);
    int i;
    for(i = 0; i < numSets; i++) {
        cache->storage[i] = createSet(association);
    }
    return cache;
}

void destroyCache(Cache *cache) {
    int i;
    for(i = 0; i < cache->numSets; i++) {
        destroySet(cache->storage[i]);
    }
    free(cache);
}

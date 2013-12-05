#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim.h"

#define startingLineLength 8

int intArgs[7];
char *replaceAlg;
char *traceFile;
Cache *l1Cache, *l2Cache, *l3Cache;

int isPowerOfTwo(int check) {
    while((check % 2 == 0) && check > 1) {
        check /=2;
    }
    return (check == 1);
}

int validateParameters(char **argv) {
    int i;
    int shouldAbsorb = 1;
    int error = 0;
    for(i = 1; i < 16; i++) {
        char *currentStr = argv[i];
        if(strcmp("-l1size", currentStr) == 0) {
            if(strtol(argv[i + 1], NULL, 10)) {
                intArgs[0] = atoi(argv[i + 1]);
            } else {
                printf("ERROR: Invalid L1 Size\n");
                error = 1;
            }
            i++;
        } else if(strcmp("-l1assoc", currentStr) == 0) {
            char *nextStr = argv[i + 1];
            if(strcmp("direct", nextStr) == 0) {
                intArgs[1] = 1;
            } else if(strcmp("assoc", nextStr) == 0) {
                intArgs[1] = 0;
            } else if(strstr(nextStr, "assoc:") != NULL) {
                intArgs[1] = atoi(&nextStr[6]);
            } else {
                printf("ERROR: Invalid L1 Association Type\n");
                error = 1;
            }
            i++;
        } else if(strcmp("-l2size", currentStr) == 0) {
            if(strtol(argv[i + 1], NULL, 10)) {
                intArgs[2] = atoi(argv[i + 1]);
            } else {
                printf("ERROR: Invalid L2 Size\n");
                error = 1;
            }
            i++;
        } else if(strcmp("-l2assoc", currentStr) == 0) {
            char *nextStr = argv[i + 1];
            if(strcmp("direct", nextStr) == 0) {
                intArgs[3] = 1;
            } else if(strcmp("assoc", nextStr) == 0) {
                intArgs[3] = 0;
            } else if(strstr(nextStr, "assoc:") != NULL) {
                intArgs[3] = atoi(&nextStr[6]);
            } else {
                printf("ERROR: Invalid L2 Association Type\n");
                error = 1;
            }
            i++;
        } else if(strcmp("-l3size", currentStr) == 0) {
            if(strtol(argv[i + 1], NULL, 10)) {
                intArgs[4] = atoi(argv[i + 1]);
            } else {
                printf("ERROR: Invalid L3 Size\n");
                error = 1;
            }
            i++;
        } else if(strcmp("-l3assoc", currentStr) == 0) {
            char *nextStr = argv[i + 1];
            if(strcmp("direct", nextStr) == 0) {
                intArgs[5] = 1;
            } else if(strcmp("assoc", nextStr) == 0) {
                intArgs[5] = 0;
            } else if(strstr(nextStr, "assoc:") != NULL) {
                intArgs[5] = atoi(&nextStr[6]);
            } else {
                printf("ERROR: Invalid L3 Association Type\n");
                error = 1;
            }
            i++;
        } else if(strcmp("FIFO", currentStr) == 0 || strcmp("LRU", currentStr) == 0) {
            replaceAlg = currentStr;
        } else if(strtol(currentStr, NULL, 10)) {
            intArgs[7] = atoi(currentStr);
        } else {
            if(shouldAbsorb) {
                traceFile = currentStr;
                shouldAbsorb = 0;
            } else {
                printf("ERROR: Invalid Parameters\n");
                error = 1;
            }
        }
    }
    if(!isPowerOfTwo(intArgs[0]) || !isPowerOfTwo(intArgs[2]) || !isPowerOfTwo(intArgs[4]) || !isPowerOfTwo(intArgs[7]) ) {
        printf("ERROR: Numbers must be powers of two\n");
        error = 1;
    }
    if(intArgs[1] == 0) {
        int cacheSize = intArgs[0];
        int blockSize = intArgs[7];
        int assoc = cacheSize / blockSize;
        intArgs[1] = assoc;
    }
    if(intArgs[3] == 0) {
        int cacheSize = intArgs[2];
        int blockSize = intArgs[7];
        int assoc = cacheSize / blockSize;
        intArgs[3] = assoc;
    }
    if(intArgs[5] == 0) {
        int cacheSize = intArgs[4];
        int blockSize = intArgs[7];
        int assoc = cacheSize / blockSize;
        intArgs[5] = assoc;
    }
    if(!error) {
        return 1;
    } else {
        return 0;
    }
}

SmrtArr *getLines(FILE *file) {
    int byteData = 0;
    SmrtArr *allLines = createSmrtArr();

    while(byteData != EOF) {
        int innerCounter;
        char *currentLine = malloc(sizeof(char) * startingLineLength);
        int currentSize = startingLineLength;
        byteData = 0;
        for(innerCounter = 0; byteData != EOF && byteData != '\n'; innerCounter++) {
            byteData = fgetc(file);
            if(byteData != EOF && byteData != '\n') {
                currentLine[innerCounter] = (char)byteData;
            }
            if(innerCounter >= currentSize) {
                currentSize *= 2;
                currentLine = realloc(currentLine, sizeof(char) * currentSize);
            }
        }
        currentLine[innerCounter] = '\0';
        if(strcmp(currentLine, "") != 0 && strcmp(currentLine, "#eof") != 0) {
            insertElement(allLines, currentLine);
        }
    }
    return allLines;
}

Line *createLine() {
    Line *line = malloc(sizeof(Line));
    line->validBit = 0;
    line->tag = 0;
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

Cache *createCache(int size, int association, int blockSize, int numSets) {
    Cache *cache = malloc(sizeof(Cache));
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

int main(int argc, char **argv) {
    if(argc == 2) {
        if(strcmp("-h", argv[1]) == 0) {
            printf("Print help stuff\n");
        } else {
            printf("ERROR: Invalid Arguments\n");
        }
    } else if(argc == 16) {
        if(validateParameters(argv)) {
            FILE *file = fopen(traceFile, "r");
            if(file) {
                SmrtArr *lines = getLines(file);
                int size = intArgs[0];
                int assoc = intArgs[1];
                int blockSize = intArgs[7];
                int numSets = size / (assoc * blockSize);
                l1Cache = createCache(size, assoc, blockSize, numSets);
                printf("L1 Cache created with %d bytes of memory, %d sets, and a blocksize of %d.\n", l1Cache->size, l1Cache->numSets, l1Cache->blockSize);
                size = intArgs[2];
                assoc = intArgs[3];
                numSets = size / (assoc * blockSize);
                l2Cache = createCache(size, assoc, blockSize, numSets);
                printf("L2 Cache created with %d bytes of memory, %d sets, and a blocksize of %d.\n", l2Cache->size, l2Cache->numSets, l2Cache->blockSize);
                size = intArgs[4];
                assoc = intArgs[5];
                numSets = size / (assoc * blockSize);
                l3Cache = createCache(size, assoc, blockSize, numSets);
                printf("L3 Cache created with %d bytes of memory, %d sets, and a blocksize of %d.\n", l3Cache->size, l3Cache->numSets, l3Cache->blockSize);
            } else {
                printf("ERROR: File does not exist\n");
            }
        } else {
            return 1;
        }
    } else if(argc == 17) {
        if(strcmp("-h", argv[1]) == 0) {
            printf("Print help stuff\n");
        } else {
            printf("ERROR: Invalid Arguments\n");
        }
    } else {
        printf("ERROR: Invalid number of arguments\n");
    }
    return 0;
}

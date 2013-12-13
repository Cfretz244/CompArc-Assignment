#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "sim.h"

#define startingLineLength 8

int intArgs[8];
int l1Misses, l2Misses, l3Misses;
int l1ColdMisses, l2ColdMisses, l3ColdMisses;
int l1CapacityMisses, l1ConflictMisses;
int assocCacheMisses;
int l1Hits, l2Hits, l3Hits;
int memAccesses;
char *replaceAlg;
char *traceFile;
Cache *l1Cache, *l2Cache, *l3Cache;

int isPowerOfTwo(int check) {
    while((check % 2 == 0) && check > 1) {
        check /=2;
    }
    return (check == 1);
}

int whatPowerOfTwo(int check) {
    int i;
    for(i = 0; check % 2 == 0; i++) {
        check /= 2;
    }
    return i;
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
        } else if(strcmp("FIFO", currentStr) == 0 || strcmp("LRU", currentStr) == 0 || strcmp("lru", currentStr) == 0 || strcmp("fifo", currentStr) == 0) {
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
    if((intArgs[0] < intArgs[1] * intArgs[7]) || (intArgs[2] < intArgs[3] * intArgs[7]) || (intArgs[4] < intArgs[5] * intArgs[7])) {
        printf("ERROR: Check those numbers, they don't make sense\n");
        error = 1;
    }
    if(!error) {
        return 1;
    } else {
        return 0;
    }
}

SmrtArr *getLines(FILE *file) {
    int byteData = 0, shouldRun = 0, validData = 1;
    SmrtArr *allLines = createSmrtArr();

    while(byteData != EOF) {
        long long int currentLine = 0;
        byteData = 0;
        shouldRun = 0;
        validData = 1;
        while(byteData != EOF && byteData != '\n') {
            byteData = fgetc(file);
            if(byteData != EOF && byteData != '\n' && shouldRun) {
                char currentChar = (char)byteData;
                currentLine *= 16;
                if(currentChar >= 'a' && currentChar <= 'f') {
                    currentLine += currentChar - 87;
                } else if(currentChar >= '0' && currentChar <= '9') {
                    currentLine += currentChar - 48;
                } else if(currentChar != 'x') {
                    validData = 0;
                    break;
                }
            }
            if(!shouldRun) {
                shouldRun = 1;
            }
        }
        if(currentLine > 0 && validData) {
            insertElement(allLines, currentLine);
        }
    }
    return allLines;
}

long long int *bitHash(long long int currentElem) {
    long long int *hashes = malloc(sizeof(long long int) * 9);
    int l1BlockBits = whatPowerOfTwo(l1Cache->blockSize);
    int l2BlockBits = whatPowerOfTwo(l2Cache->blockSize);
    int l3BlockBits = whatPowerOfTwo(l3Cache->blockSize);
    int l1SetBits = whatPowerOfTwo(l1Cache->numSets);
    int l2SetBits = whatPowerOfTwo(l2Cache->numSets);
    int l3SetBits = whatPowerOfTwo(l3Cache->numSets);
    hashes[0] = (currentElem) & ~(~0 << (l1BlockBits));
    hashes[1] = (currentElem >> (l1BlockBits)) & ~(~0 << (l1SetBits));
    hashes[2] = (currentElem >> (l1BlockBits + l1SetBits));
    hashes[3] = (currentElem) & ~(~0 << (l2BlockBits));
    hashes[4] = (currentElem >> (l2BlockBits)) & ~(~0 << (l2SetBits));
    hashes[5] = (currentElem >> (l2BlockBits + l2SetBits));
    hashes[6] = (currentElem) & ~(~0 << (l3BlockBits));
    hashes[7] = (currentElem >> (l3BlockBits)) & ~(~0 << (l3SetBits));
    hashes[8] = (currentElem >> (l3BlockBits + l3SetBits));
    return hashes;
}

void updateLines(Line **lines, int lineCount, int index, int wasInserted) {
    int i;
    for(i = 0; i < lineCount; i++) {
        Line *currLine = lines[i];
        if(i != index && currLine->validBit) {
            currLine->usageAge++;
            if(wasInserted) {
                currLine->insertionAge++;
            }
        }
    }
}

int checkAndUpdateCache(Cache *cache, long long int blockOffset, long long int setOffset, long long int tag, long long int fullNum) {
    Set *currSet = cache->storage[setOffset];
    Line **lines = currSet->lines;
    int i, validCount = 0;
    for(i = 0; i < currSet->numLines; i++) {
        Line *currLine = lines[i];
        if(currLine->validBit) {
            validCount++;
            if(currLine->tag == tag) {
                //We have a hit
                currLine->usageAge = 0;
                return 1;
            }
        }
    }
    if(cache->type == 1) {
        l1Misses++;
    } else if(cache->type == 2) {
        l2Misses++;
    } else if(cache->type == 3) {
        l3Misses++;
    } else if(cache->type == 4) {
        assocCacheMisses++;
    }
    if(validCount == currSet->numLines) {
        //data needs to be evicted
        int index = -1;
        if(strcmp(replaceAlg, "FIFO") == 0) {
            int oldest = INT_MIN;
            for(i = 0; i < currSet->numLines; i++) {
                if(lines[i]->insertionAge > oldest) {
                    oldest = lines[i]->insertionAge;
                    index = i;
                }
            }
        } else {
            //Need to replace data used longest ago
            int leastUsed = INT_MIN;
            for(i = 0; i < currSet->numLines; i++) {
                if(lines[i]->usageAge > leastUsed) {
                    leastUsed = lines[i]->usageAge;
                    index = i;
                }
            }
        }
        lines[index]->tag = tag;
        lines[index]->insertionAge = 0;
        lines[index]->usageAge = 0;
        updateLines(lines, currSet->numLines, index, 1);
    } else {
        //need to insert at an unused location
        if(cache->type == 1) {
            l1ColdMisses++;
        } else if(cache->type == 2) {
            l2ColdMisses++;
        } else if(cache->type == 3) {
            l3ColdMisses++;
        }
        for(i = 0; i < currSet->numLines; i++) {
            Line *currLine = lines[i];
            if(!currLine->validBit) {
                currLine->validBit = 1;
                currLine->tag = tag;
                currLine->insertionAge = 0;
                currLine->usageAge = 0;
                updateLines(lines, currSet->numLines, i, 1);
                return 0;
            }
        }
    }
    return 0;
}

void insertionLoop(SmrtArr *arr) {
    int i;
    memAccesses = arr->elemsHeld;
    for(i = 0; i < arr->elemsHeld; i++) {
        int hit = 0;
        long long int *hashes = bitHash(arr->contents[i]);
        if(checkAndUpdateCache(l1Cache, hashes[0], hashes[1], hashes[2], arr->contents[i])) {
            l1Hits++;
            hit = 1;
        } else if(checkAndUpdateCache(l2Cache, hashes[3], hashes[4], hashes[5], arr->contents[i])) {
            l2Hits++;
            hit = 1;
        } else if(checkAndUpdateCache(l3Cache, hashes[6], hashes[7], hashes[8], arr->contents[i])) {
            l3Hits++;
            hit = 1;
        }
    }
}

long long int *secondaryBitHash(long long int currentElem, Cache *assocCache) {
    long long int *hashes = malloc(sizeof(long long int) * 3);
    int assocBlockBits = whatPowerOfTwo(assocCache->blockSize);
    int assocSetBits = whatPowerOfTwo(assocCache->numSets);
    hashes[0] = (currentElem) & ~(~0 << (assocBlockBits));
    hashes[1] = (currentElem >> (assocBlockBits)) & ~(~0 << (assocSetBits));
    hashes[2] = (currentElem >> (assocBlockBits + assocSetBits));
    return hashes;
}

void secondaryInsertionLoop(Cache *assocCache, SmrtArr *arr) {
    int i;
    for(i = 0; i < arr->elemsHeld; i++) {
        long long int *hashes = secondaryBitHash(arr->contents[i], assocCache);
        checkAndUpdateCache(assocCache, hashes[0], hashes[1], hashes[2], arr->contents[i]);
    }
}

void printHelpStuff() {
    printf("Usage: -l1size <l1size> -l1assoc <direct|assoc|assoc:n> -l2size <l2size> -l2assoc <direct|assoc|assoc:n> -l3size <l3size> -l3assoc <direct|assoc|assoc:n> <blocksize> <replacement algorithm> <trace file>\n");
}

int main(int argc, char **argv) {
    if(argc == 2) {
        if(strcmp("-h", argv[1]) == 0) {
            printHelpStuff();
        } else {
            printf("ERROR: Invalid Arguments\n");
        }
    } else if(argc == 16) {
        if(validateParameters(argv)) {
            FILE *file = fopen(traceFile, "r");
            if(file) {
                l1Misses = 0;
                l1ColdMisses = 0;
                l1CapacityMisses = 0;
                l1ConflictMisses = 0;
                l2Misses = 0;
                l3Misses = 0;
                l1Hits = 0;
                l2Hits = 0;
                l3Hits = 0;
                memAccesses = 0;
                SmrtArr *lines = getLines(file);
                int size = intArgs[0];
                int assoc = intArgs[1];
                int blockSize = intArgs[7];
                int numSets = size / (assoc * blockSize);
                l1Cache = createCache(1, size, assoc, blockSize, numSets);
                size = intArgs[2];
                assoc = intArgs[3];
                numSets = size / (assoc * blockSize);
                l2Cache = createCache(2, size, assoc, blockSize, numSets);
                size = intArgs[4];
                assoc = intArgs[5];
                numSets = size / (assoc * blockSize);
                l3Cache = createCache(3, size, assoc, blockSize, numSets);
                insertionLoop(lines);
                //This line makes me sad
                //I'd honestly prefer to keep track of the different misses as they happen than to rerun another simulation
                if(l1Cache->numSets != 1) {
                    int numLines = l1Cache->size / l1Cache->blockSize;
                    assocCacheMisses = 0;
                    Cache *assocCache = createCache(4, l1Cache->size, numLines, l1Cache->blockSize, 1);
                    secondaryInsertionLoop(assocCache, lines);
                    l1ConflictMisses = l1Misses - assocCacheMisses;
                    l1CapacityMisses = assocCacheMisses - l1ColdMisses;
                }
                printf("Memory Accesses: %d\n", memAccesses);
                printf("L1 Cache Hits: %d\n" ,l1Hits);
                printf("L1 Cache Miss: %d\n", l1Misses);
                printf("L2 Cache Hits: %d\n", l2Hits);
                printf("L2 Cache Miss: %d\n", l2Misses);
                printf("L3 Cache Hits: %d\n", l3Hits);
                printf("L3 Cache Miss: %d\n", l3Misses);
                printf("L1 Cold Misses: %d\n", l1ColdMisses);
                printf("L2 Cold Misses: %d\n", l2ColdMisses);
                printf("L3 Cold Misses: %d\n", l3ColdMisses);
                printf("L1 Conflict Misses: %d\n", l1ConflictMisses);
                printf("L1 Capacity Misses: %d\n", l1CapacityMisses);
            } else {
                printf("ERROR: File does not exist\n");
            }
        } else {
            return 1;
        }
    } else if(argc == 17) {
        if(strcmp("-h", argv[1]) == 0) {
            printHelpStuff();
        } else {
            printf("ERROR: Invalid Arguments\n");
        }
    } else {
        printf("ERROR: Invalid number of arguments\n");
    }
    return 0;
}

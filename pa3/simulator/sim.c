#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "sim.h"
#include "Cache.h"

#define startingLineLength 8

int intArgs[8];
int coldMiss, conflictMiss, capacityMiss;
int l1Hits, l2Hits, l3Hits;
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

int nextPowerOfTwo(long long int check) {
    int i;
    long long int powers = 1;
    for(i = 0; check > powers; i++) {
        powers *= 2;
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

int checkAndUpdateCache(Cache *cache, long long int blockOffset, long long int setOffset, long long int tag) {
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
                if(cache->type == 1) {
                    l1Hits++;
                } else if(cache->type == 2) {
                    l2Hits++;
                } else if(cache->type == 3) {
                    l3Hits++;
                }
                return 1;
            }
        }
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
    for(i = 0; i < arr->elemsHeld; i++) {
        long long int *hashes = bitHash(arr->contents[i]);
        if(checkAndUpdateCache(l1Cache, hashes[0], hashes[1], hashes[2])) {
            //l1Cache hit
        } else if(checkAndUpdateCache(l2Cache, hashes[3], hashes[4], hashes[5])) {
            //l2Cache hit
        } else if(checkAndUpdateCache(l3Cache, hashes[6], hashes[7], hashes[8])) {
            //l3Cache hit
        } else {
            //complete miss
        }
    }
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
            coldMiss = 0;
            capacityMiss = 0;
            conflictMiss = 0;
            FILE *file = fopen(traceFile, "r");
            if(file) {
                SmrtArr *lines = getLines(file);
                int size = intArgs[0];
                int assoc = intArgs[1];
                int blockSize = intArgs[7];
                int numSets = size / (assoc * blockSize);
                l1Cache = createCache(1, size, assoc, blockSize, numSets);
                printf("L1 Cache created with %d bytes of memory, %d sets, and a blocksize of %d.\n", l1Cache->size, l1Cache->numSets, l1Cache->blockSize);
                size = intArgs[2];
                assoc = intArgs[3];
                numSets = size / (assoc * blockSize);
                l2Cache = createCache(2, size, assoc, blockSize, numSets);
                printf("L2 Cache created with %d bytes of memory, %d sets, and a blocksize of %d.\n", l2Cache->size, l2Cache->numSets, l2Cache->blockSize);
                size = intArgs[4];
                assoc = intArgs[5];
                numSets = size / (assoc * blockSize);
                l3Cache = createCache(3, size, assoc, blockSize, numSets);
                printf("L3 Cache created with %d bytes of memory, %d sets, and a blocksize of %d.\n", l3Cache->size, l3Cache->numSets, l3Cache->blockSize);
                insertionLoop(lines);
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

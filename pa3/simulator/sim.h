#include "SmrtArr.h"
typedef struct Line {
    int validBit;
    int tag;
} Line;

typedef struct Set {
    int numLines;
    Line **lines;
} Set;

typedef struct Cache {
    int size;
    int association;
    int blockSize;
    int numSets;
    Set **storage;
} Cache;

//Creates a cache struct and initializes it with numOfSets sets
Cache *createCache(int size, int association, int blockSize, int numSets);

//Frees a cache struct
void destroyCache(Cache *cache);

//Creates a set struct
Set *createSet(int numLines);

//Destroys a set struct
void destroySet(Set *set);

//Creates a line struct
Line *createLine();

//Destroys a line struct
void destroyLine(Line *line);

//Very basic function that validates whether an integer is a power of two iteratively
int isPowerOfTwo(int check);

//Function takes care of initial processing of user entered parameters.
//Allows parameters to be entered in any order
int validateParameters(char **argv);

//Function handles reading data in from the trace files.
SmrtArr *getLines(FILE *file);

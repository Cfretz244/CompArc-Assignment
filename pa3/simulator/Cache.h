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

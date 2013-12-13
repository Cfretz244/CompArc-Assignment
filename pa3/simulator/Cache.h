//Struct represents a single line inside of a set, inside of a Cache
typedef struct Line {
    int insertionAge;
    int usageAge;
    int validBit;
    unsigned long long int tag;
} Line;

//Struct represents a single set inside of a Cache
typedef struct Set {
    int numLines;
    Line **lines;
} Set;

//Struct represents a complete cache, and all pertinent info
typedef struct Cache {
    int type;
    int size;
    int association;
    int blockSize;
    int numSets;
    Set **storage;
} Cache;

//Creates a cache struct and initializes it with numOfSets sets
Cache *createCache(int type, int size, int association, int blockSize, int numSets);

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

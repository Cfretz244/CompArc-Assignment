

typedef struct Line {
    int validBit;
    void *tag;
    void *data;
} Line;

typedef struct Set {
    int numLines;
    Line **lines;
} Set;

typedef struct Cache {
    int cacheSize;
    int setSize;
    int blockSize;
    int numOfSets;
    char *assoc;
    Set **storage;
} Cache;

int isPowerOfTwo(int check);
int validateParameters(char **argv);

#define startingArraySize 10

typedef struct SmrtArr {
    char **contents;
    int size;
    int elemsHeld;
} SmrtArr;

SmrtArr *createSmrtArr();

void insertElement(SmrtArr *arr, char *element);

void destroySmrtArr(SmrtArr *arr);

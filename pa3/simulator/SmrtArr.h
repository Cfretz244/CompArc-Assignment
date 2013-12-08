#define startingArraySize 10

typedef struct SmrtArr {
    long long int *contents;
    int size;
    int elemsHeld;
} SmrtArr;

SmrtArr *createSmrtArr();

void insertElement(SmrtArr *arr, long long int element);

void destroySmrtArr(SmrtArr *arr);

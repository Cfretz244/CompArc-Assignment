#define startingArraySize 10

//Simple struct that houses an array plus its current total size, and the number of
//elements in it.
typedef struct SmrtArr {
    unsigned long long int *contents;
    int size;
    int elemsHeld;
} SmrtArr;

//Functions handles the creation of a SmrtArr
SmrtArr *createSmrtArr();

//Function handles the insertion of elements into, and resizing of, a SmrtArr
void insertElement(SmrtArr *arr, unsigned long long int element);

//Although never actually called, or tested, I assume that this would successfully
//free a SmrtArr
void destroySmrtArr(SmrtArr *arr);

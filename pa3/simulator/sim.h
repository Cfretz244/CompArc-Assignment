#include "SmrtArr.h"
#include "Cache.h"

//Very basic function that validates whether an integer is a power of two iteratively
int isPowerOfTwo(int check);

//Very basic function that returns the what power of two a number is
//Works under the assumption that number has already been proven to be a power of two
int whatPowerOfTwo(int check);

//Function takes care of initial processing of user entered parameters.
//Allows parameters to be entered in any order
int validateParameters(char **argv);

//Function handles reading data in from the trace files.
SmrtArr *getLines(FILE *file);

//Function handles all bit shifting necessary to hash addresses into the cache
long long int *bitHash(long long int currentElem);

//Function keeps track of when addresses were inserted, and when they were last used
void updateLines(Line **lines, int lineCount, int index, int wasInserted);

//Function handles the majority of logic related to inserting an address into the cache
int checkAndUpdateCache(Cache *cache, long long int blockOffset, long long int setOffset, long long int tag, long long int fullNum);

//Function consists of a simple loop that calls checkAndUpdateCache on all addresses
void insertionLoop(SmrtArr *arr);

//Function does exactly the same thing as bitHash, but for the fully associated cache
//simulation that is run after the main simulation
long long int *secondaryBitHash(long long int currentElem, Cache *assocCache);

//Function accomplishes the same thing as insertionLoop, but for the fully associated
//cache simulation that is run after the main simulation
void secondaryInsertionLoop(Cache *assocCache, SmrtArr *arr);

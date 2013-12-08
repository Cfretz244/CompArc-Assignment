#include "SmrtArr.h"

//Very basic function that validates whether an integer is a power of two iteratively
int isPowerOfTwo(int check);

//Very basic function that returns the what power of two a number is
//Works under the assumption that number has already been proven to be a power of two
int whatPowerOfTwo(int check);

//Very basic function that returns the minimum number of bits required to store a
//number
int nextPowerOfTwo(long long int check);

//Function takes care of initial processing of user entered parameters.
//Allows parameters to be entered in any order
int validateParameters(char **argv);

//Function handles reading data in from the trace files.
SmrtArr *getLines(FILE *file);

//Function handles the cache inserts after validating the parameters and generating
//the caches
void insertionLoop(SmrtArr *arr);

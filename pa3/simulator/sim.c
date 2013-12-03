#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *arguments[9];

int isPowerOfTwo(int check) {
    while((check % 2 == 0) && check > 1) {
        check /=2;
    }
    return (check == 1);
}

int validateParameters(char **argv) {
    int i;
    int shouldAbsorb = 1;
    for(i = 1; i < 16; i++) {
        char *currentStr = argv[i];
        if(strcmp("-l1size", currentStr) == 0) {
            if(strtol(argv[i + 1], NULL, 10) != 0) {
                int tmp = atoi(argv[i + 1]);
                arguments[0] = &tmp;
                i++;
            } else {
                printf("ERROR: Invalid Parameters\n");
                return -1;
            }
        } else if(strcmp("-l1assoc", currentStr) == 0) {
            arguments[1] = argv[i + 1];
            i++;
        } else if(strcmp("-l2size", currentStr) == 0) {
            if(strtol(argv[i + 1], NULL, 10) != 0) {
                int tmp = atoi(argv[i + 1]);
                arguments[2] = &tmp;
                i++;
            } else {
                printf("ERROR: Invalid Parameters\n");
                return -1;
            }
        } else if(strcmp("-l2assoc", currentStr) == 0) {
            arguments[3] = argv[i + 1];
            i++;
        } else if(strcmp("-l3size", currentStr) == 0) {
            if(strtol(argv[i + 1], NULL, 10) != 0) {
                int tmp = atoi(argv[i + 1]);
                arguments[4] = &tmp;
                i++;
            } else {
                printf("ERROR: Invalid Parameters\n");
                return -1;
            }
        } else if(strcmp("-l3assoc", currentStr) == 0) {
            arguments[5] = argv[i + 1];
            i++;
        } else if(strcmp("FIFO", currentStr) == 0 || strcmp("LRU", currentStr) == 0) {
            arguments[6] = currentStr;
        } else if(strtol(currentStr, NULL, 10) != 0) {
            int tmp = atoi(currentStr);
            arguments[7] = &tmp;
        } else {
            if(shouldAbsorb) {
                arguments[8] = currentStr;
                shouldAbsorb = 0;
            } else {
                printf("ERROR: Invalid Parameters\n");
                return -1;
            }
        }
    }
    if(!isPowerOfTwo(*(int *)arguments[0]) || !isPowerOfTwo(*(int *)arguments[2]) || !isPowerOfTwo(*(int *)arguments[4]) || !isPowerOfTwo(*(int *)arguments[7]) ) {
        printf("ERROR: Numbers must be powers of two\n");
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if(argc == 2) {
        if(strcmp("-h", argv[1]) == 0) {
            printf("Print help stuff\n");
        } else {
            printf("ERROR: Invalid Arguments\n");
        }
    } else if(argc == 16) {
        if(validateParameters(argv) != -1) {
            printf("Validated\n");
        } else {
            printf("Failed\n");
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim.h"
#include "SmrtArr.h"

#define startingLineLength 8

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
    int error = 0;
    for(i = 1; i < 16; i++) {
        char *currentStr = argv[i];
        if(strcmp("-l1size", currentStr) == 0) {
            if(strtol(argv[i + 1], NULL, 10) != 0) {
                int tmp = atoi(argv[i + 1]);
                arguments[0] = &tmp;
            } else {
                printf("ERROR: Invalid L1 Size\n");
                error = 1;
            }
            i++;
        } else if(strcmp("-l1assoc", currentStr) == 0) {
            char *nextStr = argv[i + 1];
            if(strcmp("direct", nextStr) == 0 || strcmp("assoc", nextStr) == 0 || strcmp("assoc:n", nextStr) == 0) {
                arguments[1] = nextStr;
            } else {
                printf("ERROR: Invalid L1 Association Type\n");
                error = 1;
            }
            i++;
        } else if(strcmp("-l2size", currentStr) == 0) {
            if(strtol(argv[i + 1], NULL, 10) != 0) {
                int tmp = atoi(argv[i + 1]);
                arguments[2] = &tmp;
            } else {
                printf("ERROR: Invalid L2 Size\n");
                error = 1;
            }
            i++;
        } else if(strcmp("-l2assoc", currentStr) == 0) {
            char *nextStr = argv[i + 1];
            if(strcmp("direct", nextStr) == 0 || strcmp("assoc", nextStr) == 0 || strcmp("assoc:n", nextStr) == 0) {
                arguments[3] = nextStr;
            } else {
                printf("ERROR: Invalid L2 Association Type\n");
                error = 1;
            }
            i++;
        } else if(strcmp("-l3size", currentStr) == 0) {
            if(strtol(argv[i + 1], NULL, 10) != 0) {
                int tmp = atoi(argv[i + 1]);
                arguments[4] = &tmp;
            } else {
                printf("ERROR: Invalid L3 Size\n");
                error = 1;
            }
            i++;
        } else if(strcmp("-l3assoc", currentStr) == 0) {
            char *nextStr = argv[i + 1];
            if(strcmp("direct", nextStr) == 0 || strcmp("assoc", nextStr) == 0 || strcmp("assoc:n", nextStr) == 0) {
                arguments[5] = nextStr;
            } else {
                printf("ERROR: Invalid L3 Association Type\n");
                error = 1;
            }
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
                error = 1;
            }
        }
    }
    if(!isPowerOfTwo(*(int *)arguments[0]) || !isPowerOfTwo(*(int *)arguments[2]) || !isPowerOfTwo(*(int *)arguments[4]) || !isPowerOfTwo(*(int *)arguments[7]) ) {
        printf("ERROR: Numbers must be powers of two\n");
        error = 1;
    }
    if(!error) {
        return 1;
    } else {
        return 0;
    }
}

SmrtArr* getLines(FILE *file) {
    int byteData = 0;
    SmrtArr *allLines = createSmrtArr();

    while(byteData != EOF) {
        int innerCounter;
        char *currentLine = malloc(sizeof(char) * startingLineLength);
        int currentSize = startingLineLength;
        byteData = 0;
        for(innerCounter = 0; byteData != EOF && byteData != '\n'; innerCounter++) {
            byteData = fgetc(file);
            if(byteData != EOF && byteData != '\n') {
                currentLine[innerCounter] = (char)byteData;
            }
            if(innerCounter >= currentSize) {
                currentSize *= 2;
                currentLine = realloc(currentLine, sizeof(char) * currentSize);
            }
        }
        currentLine[innerCounter] = '\0';
        if(strcmp(currentLine, "") != 0 && strcmp(currentLine, "#eof") != 0) {
            insertElement(allLines, currentLine);
        }
    }
    return allLines;
}

int main(int argc, char **argv) {
    if(argc == 2) {
        if(strcmp("-h", argv[1]) == 0) {
            printf("Print help stuff\n");
        } else {
            printf("ERROR: Invalid Arguments\n");
        }
    } else if(argc == 16) {
        if(validateParameters(argv)) {
            printf("Validated\n");
            FILE *file = fopen((char *)arguments[8], "r");
            if(file) {
                printf("File Opened\n");
                SmrtArr *lines = getLines(file);
                printf("File read\n");
                int i;
                for(i = 0; i < lines->elemsHeld; i++) {
                    printf("%s\n", lines->contents[i]);
                }
            } else {
                printf("ERROR: File does not exist\n");
            }
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

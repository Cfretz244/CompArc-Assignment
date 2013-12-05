#include <stdio.h>
#include <stdlib.h>
#include "SmrtArr.h"

SmrtArr *createSmrtArr() {
    SmrtArr *arr = malloc(sizeof(SmrtArr));
    arr->contents = malloc(sizeof(char *) * startingArraySize);
    arr->elemsHeld = 0;
    arr->size = 10;
    int i;
    for(i = 0; i < arr->size; i++) {
        arr->contents[i] = NULL;
    }
    return arr;
}

void resizeSmrtArr(SmrtArr *arr) {
    arr->size *= 2;
    arr->contents = realloc(arr->contents, sizeof(char *) * arr->size);
    int i;
    for(i = arr->elemsHeld; i < arr->size; i++) {
        arr->contents[i] = NULL;
    }
}

void insertElement(SmrtArr *arr, char *element) {
    if(arr->elemsHeld == arr->size) {
        resizeSmrtArr(arr);
    }
    arr->contents[arr->elemsHeld] = element;
    arr->elemsHeld++;
}

void destroySmrtArr(SmrtArr *arr) {
    int i;
    for(i = 0; i < arr->size; i++) {
        free(arr->contents[i]);
    }
    free(arr->contents);
    free(arr);
}

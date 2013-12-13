#include <stdio.h>
#include <stdlib.h>
#include "SmrtArr.h"

SmrtArr *createSmrtArr() {
    SmrtArr *arr = malloc(sizeof(SmrtArr));
    arr->contents = malloc(sizeof(unsigned long long int) * startingArraySize);
    arr->elemsHeld = 0;
    arr->size = 10;
    int i;
    for(i = 0; i < arr->size; i++) {
        arr->contents[i] = 0;
    }
    return arr;
}

void resizeSmrtArr(SmrtArr *arr) {
    arr->size *= 2;
    arr->contents = realloc(arr->contents, sizeof(unsigned long long int) * arr->size);
    int i;
    for(i = arr->elemsHeld; i < arr->size; i++) {
        arr->contents[i] = 0;
    }
}

void insertElement(SmrtArr *arr, unsigned long long int element) {
    if(arr->elemsHeld == arr->size) {
        resizeSmrtArr(arr);
    }
    arr->contents[arr->elemsHeld] = element;
    arr->elemsHeld++;
}

void destroySmrtArr(SmrtArr *arr) {
    free(arr->contents);
    free(arr);
}

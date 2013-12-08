#include <stdio.h>
int main(void) {
    int test = 2129;
    int msb = 7;
    int lsb = 4;
    int result = (test >> lsb) & ~(~0 << (msb-lsb+1));
    printf("Someting");
}

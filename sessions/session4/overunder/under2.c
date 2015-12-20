#include <stdio.h>

int main() {
    unsigned char guys = 3;
    int i;
    printf("Onion is trying to steal my G.U.Y.s!\n");
    printf("I have %d now.\n", guys);
    for (i = 0; i < 5; i++) {
        steal_guy(&guys);
        printf("Oh no, Onion stole one! I have %d left!\n", guys);
    }
    printf("What! I have more G.U.Y.s than I started with!\n");
}

void steal_guy(unsigned char* guy) {
    *guy = *guy - 1;
}

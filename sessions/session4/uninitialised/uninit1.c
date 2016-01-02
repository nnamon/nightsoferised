#include <stdio.h>

int main() {
    eat_burrito();
    everything_burrito();
    eat_burrito();
}

void everything_burrito() {
    char everything[500];
    memset(everything, 0x41, 500);
}

void eat_burrito() {
    char burrito[500];
    puts("Never know what you're gonna get!");
    printf("Yum: %x %x %x %x %x\n", burrito[200], burrito[201], burrito[202],
            burrito[203], burrito[204]);
}

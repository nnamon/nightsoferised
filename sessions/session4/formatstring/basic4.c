#include <stdio.h>

int main(int argc, char** argv) {
    meeseeks();
}

void meeseeks() {
    int wish1 = 0x41414141;
    int wish2 = 0x42424242;
    int wish3 = 0x43434343;
    int wish4 = 0x44444444;
    printf("Last golf run: %3$x\n", wish1, wish2, wish3, wish4);
}

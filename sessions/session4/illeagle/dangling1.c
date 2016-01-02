#include <stdlib.h>
#include <stdio.h>

void assign(int **pointy) {
    int workers[100] = {0};
    int the_boss = 0x41414141;
    *pointy = &the_boss;
}

void slackoff(int dowork) {
    int boredom[100] =  {0};
    int slack = 0xdeadbeef;
    printf("0x%x sounds boring. Ignore!\n", dowork);
    printf("I will slack off for 0x%x seconds!\n", slack);
}

int main() {
    int *ptr;
    assign(&ptr);
    printf("Pointer: address 0x%x and value 0x%x\n", ptr, *ptr);
    slackoff(0xcafebabe);
    printf("Slack Pointer: address 0x%x and value 0x%x\n", ptr, *ptr);
    return 0;
}

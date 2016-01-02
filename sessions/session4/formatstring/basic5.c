#include <stdio.h>

char token[26];

int main() {
    strncpy(token, "yum-yum-squanch-squaunchy", 26);
    printf("Token address: 0x%x\n", &token);
    squanch();
}

void squanch() {
    fputs("Please give me something to squanch: ", stdout);
    char squeak[501] = {0};
    scanf("%500s", squeak);
    printf(squeak);
}

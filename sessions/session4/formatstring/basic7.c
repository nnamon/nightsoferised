#include <stdio.h>

int security_code;

int main() {
    security_code = 0;
    printf("Security code address: 0x%x\n", &security_code);
    go_customs();
    printf("\nSecurity code: %d\n", security_code);
    if (check_code()) {
        puts("That isn't right... go to jail!");
    }
    else {
        puts("All clear!");
    }
}

int check_code() {
    if (security_code == 42) {
        return 0;
    }
    return 1;
}

void go_customs() {
    fputs("Passport number: ", stdout);
    char passport[501] = {0};
    scanf("%500s", passport);
    printf(passport);
}

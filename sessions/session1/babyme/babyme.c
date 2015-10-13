int main() {
    char *potato = "This is the potato password!";
    char tocheck[21];
    scanf("%20s", tocheck);
    if (check(potato, tocheck)) {
        printf("You win!\n");
    }
    else {
        printf("You lose!\n");
    }
}

int check(char* correct, char* tocheck) {
    int i;
    for (i = 0; i < 6; i++) {
        if (correct[12+i] != tocheck[i]) {
            return 0;
        }
    }
    return 1;
}

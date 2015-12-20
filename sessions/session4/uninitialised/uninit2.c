#include <stdio.h>

struct Adventurer {
    char name[40];
    int level;
};

struct Adventurer finn;

int main() {
    finn.level = 0;
    strncpy(finn.name, "Finn", 5);
    printf("%s is an adventurer! He is level %u!\n", finn.name, finn.level);
    printf("Finn lives at 0x%x (it's a treehouse.)\n", &finn);
    puts("He needs training but only Billy *guitar solo* can train him.");
    printf("Can you tell us where he is? ");
    billyspad();
    findbilly();
    checkfinn();
}


void billyspad() {
    char everything[500];
    memset(everything, 0, 500);
    scanf("%499s", everything);
}

void findbilly() {
    char search[500];
    puts("Thanks! But we couldn't find Billy at: ");
    printf(search);
}

void checkfinn() {
    puts("\nNever mind though! If he is level 101, he will become super" \
            "mathematical and awesome!");
    printf("Level: %u\n", finn.level);
    if (finn.level != 101) {
        printf("Aww cabbage... %s isn't level 101!\n", finn.name);
    }
    else {
        puts("Woooo, he is totally RAD!");
    }
}

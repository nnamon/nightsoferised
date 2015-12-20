#include <stdio.h>

int main() {
    signed char zombies = 123;
    int i;
    printf("Pearl: \"There are too many of them! %d to be exact!\"\n", zombies);
    puts("Steven: \"I have an idea! Instead of killing them, let's make more!\"");
    puts("Amethyst: \"Wait, what? You're nuts!\"");
    for (i = 0; i < 7; i++) {
        make_zombies(&zombies);
        printf("Random Stranger: \"Ouch, I got bit! Grrgh ugh %d zombies.\n",
                zombies);
    }
    printf("Steven: \"See, there are %d zombies now! And the opposite of " \
            "zombies are cute cats! Yay cute cats! :3\"\n", zombies);
}

void make_zombies(signed char* zombies) {
    *zombies = *zombies + 1;
}

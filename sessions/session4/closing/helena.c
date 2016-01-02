#include <stdio.h>
#include <stdlib.h>

struct streamer {
    char name[40];
    char cmd[10];
};

int main() {
    struct streamer *helena = (struct streamer*) malloc(sizeof(struct streamer));
    strcpy(helena->name, "Helena");
    strcpy(helena->cmd, "id");
    printf("I am %s, and my command is %s.\n", helena->name, helena->cmd);
    free(helena);

    struct streamer *guest = (struct streamer*) malloc(sizeof(struct streamer));
    printf("Welcome guest streamer! My command is the best, you have it too!\n");
    strcpy(guest->cmd, helena->cmd);
    printf("What is your name: ");
    gets(guest->name);
    printf("Hello, %s!\n", guest->name);

    printf("I'm going to execute my command now, because I'm cool.\n");
    system(helena->cmd);
    return 0;
}

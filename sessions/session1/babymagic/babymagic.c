int main(int argc, char ** argv) {
    int boomstring[33] = {17, 58, 44, 12, 11, 56, 17, 13, 23, 0, 50, 18, 48, 38, 3, 59, 11, 31, 0, 51, 51, 0, 25, 4, 7, 48, 38, 56, 26, 45, 21, 5, 4};
    char * potatostring = "The_potato_world_welcomes_you_yay";
    int i;
    char * boom = argv[1];
    for (i = 0; i < 33; i++) {
        if ((boom[i] ^ potatostring[i]) != boomstring[i]) {
            printf("Wrong!\n");
            return 1;
        }
    }
    printf("Correct!\n");
    return 0;
}

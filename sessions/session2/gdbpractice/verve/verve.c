char* bitter() {
    char *b = "bitter";
    return b;
}

char* sweet() {
    char *b = "sweet";
    return b;
}

char* symphony() {
    char *b = "symphony";
    return b;
}

int main(int argc, char ** argv) {
    char * message = "No!";
    if (strcmp(argv[1], bitter()) == 0) {
        if (strcmp(argv[2], sweet() == 0)) {
            if (strcmp(argv[3], symphony()) == 0) {
                message = "Yes!";
            }
        }
    }
    printf(message);
}


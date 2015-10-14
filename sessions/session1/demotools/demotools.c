int main(int argc, char ** argv) {
    printf(argv[1]);
    say_hello();
}

void say_hello() {
    char * ihateeveryone = "Goaway!\n";
    char * iloveveryone = "Helloo!\n";
    if (strcmp(ihateeveryone, iloveveryone) == 0) {
        printf(ihateeveryone);
    }
    printf(iloveveryone);
}

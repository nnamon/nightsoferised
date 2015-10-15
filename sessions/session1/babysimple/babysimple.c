int main(int argc, char ** argv) {
    if (argc < 2) {
        printf("Usage: ./babysimple <guess>\n");
        return 1;
    }

    int password = strtoul(argv[1], 0, 10);
    if (password == 0xdeadbeef) {
        printf("Correct!\n");
    }
    else {
        printf("Wrong!\n");
    }
}

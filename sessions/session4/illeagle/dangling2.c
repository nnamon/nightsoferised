#include <stdlib.h>

int main() {
    char *ptr = malloc(50);
    memset(ptr, 0x41, 50);
    ptr[99] = 0;
    printf("Contents of ptr: %s\n", ptr);
    free(ptr);
    printf("We freed ptr\n");

    printf("Doing some completely unrelated thing.\n");
    char *unrelated = malloc(50);
    memset(unrelated, 0x42, 50);
    unrelated[99] = 0;

    printf("Contents of ptr: %s\n", ptr);
    return 0;
}

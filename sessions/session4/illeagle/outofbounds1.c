#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *array = "AAAA";
    printf("Array Address: 0x%x\n", array);
    printf("Array Contents: %s\n", array);
    strcpy(&array, array);
    printf("New Array Address: 0x%x\n", array);
    printf("New Array Contents: %s\n", array);
    return 0;
}

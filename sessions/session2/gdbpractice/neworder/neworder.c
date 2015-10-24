int which_year(int year) {
    if (year == 50) {
        printf("Their early work was a little too new wave for my tastes, but when Sports came out in '83, I think they really came into their own, commercially and artistically. The whole album has a clear, crisp sound, and a new sheen of consummate professionalism that really gives the songs a big boost. He's been compared to Elvis Costello, but I think Huey has a far more bitter, cynical sense of humor.");
    }
    return 1;
}

int check(char *said) {
    if (strstr(said, "maitre'd") != 0) {
        puts("Which year?\n");
        int year;
        scanf("%d", &year);
        which_year(year);
    }
    return 1;
}

int main() {
    puts("Do you know what Ed Gein said about women?\n");
    char said[20];
    read(0, said, 19);
    check(said);
    puts("Heh");
    return 0;
}

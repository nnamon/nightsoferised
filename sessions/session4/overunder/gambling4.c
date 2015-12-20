#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    srand(time(NULL));
    unsigned int money = 40;
    int i;
    printf("Welcome to the Tiger Millionaire Casino!\n");
    printf("Since this is your first time, we'll give you a free $50!\n");
    for (i = 0; i < 5; i++) {
        play(&money);
        printf("You have $%u. Checking if you have enough...\n", money);
        check(&money);
    }
    printf("I'm sorry, you're out of tries. You lose!\n");
}

void play(unsigned int *money) {
    int bet = 0;
    printf("Place your bets: ");
    scanf("%d", &bet);
    if (bet > 10 || bet < 0) {
        printf("Sorry, you can only bet between 0 and 10\n");
        return;
    }
    int toguess = rand();
    int guess = 0;
    printf("Make the guess: ");
    scanf("%d", &guess);
    if (guess == toguess) {
        printf("You got it right!\n");
        *money = *money + bet;
    }
    else {
        printf("You got it wrong!\n");
        *money = *money - bet;
    }
    printf("The answer was: %d.\n", toguess);
}

void check(unsigned int *money) {
    if (*money > 5000) {
        printf("You win!\n");
        exit(0);
    }
}

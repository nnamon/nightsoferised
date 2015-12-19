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

[//]: # (Links)
[1]: ./formatstring/basic1.c
[2]: ./formatstring/basic2.c
[3]: ./formatstring/basic3.c
[4]: ./formatstring/basic4.c


[//]: # (Images)
[basic2stack]: ./images/basic2stack.png
[basic3stack]: ./images/basic3stack.png

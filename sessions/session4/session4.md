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



[//]: # (Links)
[1]: ./formatstring/basic1.c
[2]: ./formatstring/basic2.c

[//]: # (Images)
[basic2stack]: ./basic2stack.png

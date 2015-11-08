Session 3 - Night of Decompildation
==========================

In this session, we're going to look at reversing byte code of
interpreted/virtual machine based programming languages.

Objectives
----------

1. Java Reversing Set Up
2. Java Crackme
3. Python Reversing Set Up
4. Python Crackme


1. Java Reversing Set Up
------------------------

First, we need to install the Java runtime and development kit in our vagrant
environment.

```console
vagrant@erised:~$ sudo apt-get install openjdk-7-jre-headless openjdk-7-jdk
Reading package lists... Done
Building dependency tree
Reading state information... Done
...
Processing triggers for ca-certificates (20141019ubuntu0.14.04.1) ...
Updating certificates in /etc/ssl/certs... 0 added, 0 removed; done.
Running hooks in /etc/ca-certificates/update.d....
done.
done.
```

2. Java Crackme
---------------

We'll work on the Java crackme in the bears directory.

```console
vagrant@erised:~/.../sessions/session3$ cd bears/
vagrant@erised:~/.../session3/bears$ ls -la
total 16
drwxrwxr-x 3 vagrant vagrant 4096 Nov  7 14:48 .
drwxrwxr-x 4 vagrant vagrant 4096 Nov  7 15:14 ..
-rw-rw-r-- 1 vagrant vagrant 1311 Nov  7 14:48 Bears.class
drwxrwxr-x 2 vagrant vagrant 4096 Nov  7 14:48 source
```

If we cat Bears.class, we won't be able to retrieve any information on the logic
of program since it's in Java bytecode format.

```console
vagrant@erised:~/.../session3/bears$ cat Bears.class | xxd
0000000: cafe babe 0000 0033 0045 0a00 1500 2603  .......3.E....&.
0000010: 0001 4b4a 0500 0000 0000 d59d fe07 0027  ..KJ...........'
0000020: 0900 2800 290a 0005 002a 0a00 0500 2b07  ..(.)....*....+.
0000030: 002c 0a00 0900 260a 0009 002d 0900 2800  .,....&....-..(.
0000040: 2e08 002f 0a00 3000 310a 0009 0032 0800  .../..0.1....2..
0000050: 330a 0009 0034 0800 3508 0036 0800 3707  3....4..5..6..7.
0000060: 0038 0100 063c 696e 6974 3e01 0003 2829  .8...<init>...()
0000070: 5601 0004 436f 6465 0100 0f4c 696e 654e  V...Code...LineN
0000080: 756d 6265 7254 6162 6c65 0100 0668 6f74  umberTable...hot
0000090: 646f 6701 0004 2849 295a 0100 0d53 7461  dog...(I)Z...Sta
00000a0: 636b 4d61 7054 6162 6c65 0100 0870 6172  ckMapTable...par
00000b0: 7479 7061 7401 000a 7472 6565 7472 756e  typat...treetrun
00000c0: 6b73 0100 046d 6169 6e01 0016 285b 4c6a  ks...main...([Lj
00000d0: 6176 612f 6c61 6e67 2f53 7472 696e 673b  ava/lang/String;
... undecipherable stuff ...
```

Of course, if we use strings on the file, we can get an idea of what the program
does like with binaries.

```console
vagrant@erised:~/.../session3/bears$ strings Bears.class
<init>
Code
LineNumberTable
hotdog
(I)Z
StackMapTable
partypat
treetrunks
main
([Ljava/lang/String;)V
SourceFile
Bears.java
java/util/Scanner
Bears
'You have passed the first test of DOOM!
,You have passed the second test of POTATOES!
)You have passed the third test of EPICAL!
Mathematical! You win!
Aww, you suck! Cheque please!
java/lang/Object
[Ljava/lang/String;
java/lang/System
Ljava/io/InputStream;
(Ljava/io/InputStream;)V
nextInt
Ljava/io/PrintStream;
java/io/PrintStream
println
(Ljava/lang/String;)V
```

But it is still not very useful. To help us look at what might be going on under
the hood, we can use the JDK utility javap.

```console
vagrant@erised:~/.../session3/bears$ javap Bears.class
Compiled from "Bears.java"
public class Bears {
  public Bears();
  public boolean hotdog(int);
  public boolean partypat(int);
  public boolean treetrunks(int);
  public static void main(java.lang.String[]);
}
```

Let's improve this by looking at the diassembled bytecode.

```console
vagrant@erised:~/.../session3/bears$ javap -c Bears.class
Compiled from "Bears.java"
public class Bears {
  public Bears();
    Code:
       0: aload_0
       1: invokespecial #1                  // Method java/lang/Object."<init>":()V
       4: return

  public boolean hotdog(int);
    Code:
       0: iload_1
       1: bipush        42
       3: irem
       4: ifne          9
       7: iconst_1
       8: ireturn
       9: iconst_0
      10: ireturn

  public boolean partypat(int);
    Code:
       0: iconst_0
       1: istore_2
       2: iconst_0
       3: istore_3
       4: iload_3
       5: iconst_3
       6: if_icmpge     21
       9: iload_2
      10: iload_3
      11: imul
      12: iload_1
      13: iadd
      14: istore_2
      15: iinc          3, 1
      18: goto          4
      21: iload_2
      22: ldc           #2                  // int 84810
      24: if_icmpne     29
      27: iconst_1
      28: ireturn
      29: iconst_0
      30: ireturn

  public boolean treetrunks(int);
    Code:
       0: iload_1
       1: iload_1
       2: imul
       3: iconst_2
       4: imul
       5: iload_1
       6: iadd
       7: sipush        4919
      10: iadd
      11: i2l
      12: lstore_2
      13: ldc2_w        #3                  // long 13999614l
      16: lstore        4
      18: lload_2
      19: lload         4
      21: lcmp
      22: ifne          27
      25: iconst_1
      26: ireturn
      27: iconst_0
      28: ireturn

  public static void main(java.lang.String[]);
    Code:
       0: new           #5                  // class java/util/Scanner
       3: dup
       4: getstatic     #6                  // Field java/lang/System.in:Ljava/io/InputStream;
       7: invokespecial #7                  // Method java/util/Scanner."<init>":(Ljava/io/InputStream;)V
      10: astore_1
      11: aload_1
      12: invokevirtual #8                  // Method java/util/Scanner.nextInt:()I
      15: istore_2
      16: aload_1
      17: invokevirtual #8                  // Method java/util/Scanner.nextInt:()I
      20: istore_3
      21: aload_1
      22: invokevirtual #8                  // Method java/util/Scanner.nextInt:()I
      25: istore        4
      27: new           #9                  // class Bears
      30: dup
      31: invokespecial #10                 // Method "<init>":()V
      34: astore        5
      36: aload         5
      38: iload_2
      39: invokevirtual #11                 // Method hotdog:(I)Z
      42: ifeq          99
      45: getstatic     #12                 // Field java/lang/System.out:Ljava/io/PrintStream;
      48: ldc           #13                 // String You have passed the first test of DOOM!
      50: invokevirtual #14                 // Method java/io/PrintStream.println:(Ljava/lang/String;)V
      53: aload         5
      55: iload_3
      56: invokevirtual #15                 // Method partypat:(I)Z
      59: ifeq          107
      62: getstatic     #12                 // Field java/lang/System.out:Ljava/io/PrintStream;
      65: ldc           #16                 // String You have passed the second test of POTATOES!
      67: invokevirtual #14                 // Method java/io/PrintStream.println:(Ljava/lang/String;)V
      70: aload         5
      72: iload         4
      74: invokevirtual #17                 // Method treetrunks:(I)Z
      77: ifeq          107
      80: getstatic     #12                 // Field java/lang/System.out:Ljava/io/PrintStream;
      83: ldc           #18                 // String You have passed the third test of EPICAL!
      85: invokevirtual #14                 // Method java/io/PrintStream.println:(Ljava/lang/String;)V
      88: getstatic     #12                 // Field java/lang/System.out:Ljava/io/PrintStream;
      91: ldc           #19                 // String Mathematical! You win!
      93: invokevirtual #14                 // Method java/io/PrintStream.println:(Ljava/lang/String;)V
      96: goto          107
      99: getstatic     #12                 // Field java/lang/System.out:Ljava/io/PrintStream;
     102: ldc           #20                 // String Aww, you suck! Cheque please!
     104: invokevirtual #14                 // Method java/io/PrintStream.println:(Ljava/lang/String;)V
     107: return
}
```

If we took a couple of minutes to decode the instructions, we probably could
reverse the entire program. This is enough to give us a rough idea of what the
program does.

However, there's a better way of reversing a Java application: decompilation. We
can use [this website](http://www.javadecompilers.com/) to decompile the Java
class file.

We get the following Java source:

```java
import java.util.Scanner;

//
// Decompiled by Procyon v0.5.29
//

public class Bears
{
    public boolean hotdog(final int n) {
        return n % 42 == 0;
    }

    public boolean partypat(final int n) {
        int n2 = 0;
        for (int i = 0; i < 3; ++i) {
            n2 = n2 * i + n;
        }
        return n2 == 84810;
    }

    public boolean treetrunks(final int n) {
        return n * n * 2 + n + 4919 == 13999614L;
    }

    public static void main(final String[] array) {
        final Scanner scanner = new Scanner(System.in);
        final int nextInt = scanner.nextInt();
        final int nextInt2 = scanner.nextInt();
        final int nextInt3 = scanner.nextInt();
        final Bears bears = new Bears();
        if (bears.hotdog(nextInt)) {
            System.out.println("You have passed the first test of DOOM!");
            if (bears.partypat(nextInt2)) {
                System.out.println("You have passed the second test of POTATOES!");
                if (bears.treetrunks(nextInt3)) {
                    System.out.println("You have passed the third test of EPICAL!");
                    System.out.println("Mathematical! You win!");
                }
            }
        }
        else {
            System.out.println("Aww, you suck! Cheque please!");
        }
    }
}
```

The program takes three integers in stdin and puts them through checks.

There are three checks involved:
1. hotdog
2. partypat
3. treetrunks

### Running the Program

```console
vagrant@erised:~/.../session3/bears$ java Bears
1
2
3
Aww, you suck! Cheque please!
```

### Passing hotdog

```java
    public boolean hotdog(final int n) {
        return n % 42 == 0;
    }
```

The hotdog check simply checks if the number is divisible by 42. Let's just pick
a number divisible by 42... like 42!

First number: 42.

### Passing partypat

```java
    public boolean partypat(final int n) {
        int n2 = 0;
        for (int i = 0; i < 3; ++i) {
            n2 = n2 * i + n;
        }
        return n2 == 84810;
    }
```

To pass this check, we need to find a number that equals 84810 when passed
through this function. We could do this by analysing the equation generated by
the function, but we solve this naively by writing a short python script to
brute force the number.

```python
In [1]: def solve(x):
   ...:     total = 0
   ...:     for i in range(3):
   ...:         total = total*i + x
   ...:     return total
   ...:

In [2]: for i in range(84810):
   ...:     if solve(i) == 84810:
   ...:         print i
   ...:
16962
```

Second number: 16962.

### Passing treetrunks

```java
    public boolean treetrunks(final int n) {
        return n * n * 2 + n + 4919 == 13999614L;
    }
```

Now, this function requires the solution to the following quadratic equation:

2n^2 + n + 4919 = 13999614

We can use [Wolfram Alpha](http://www.wolframalpha.com/input/?i=2n%5E2+%2B+n+%2B+4919+%3D+13999614)
to solve for the roots of the equation. There are two solutions: n = -5291/2 and
n = 2645. We disregard the negative solution.

Third number: 2645.

### Solution

Finally, we can solve the crackme.

```console
vagrant@erised:~/.../session3/bears$ java Bears
42
16962
2645
You have passed the first test of DOOM!
You have passed the second test of POTATOES!
You have passed the third test of EPICAL!
Mathematical! You win!
```

3. Python Reversing Set Up
--------------------------

First, we'll need to install pip.

```console
vagrant@erised:~$ sudo apt-get install python-pip
Reading package lists... Done
Building dependency tree
Reading state information... Done
The following extra packages will be installed:
  build-essential dpkg-dev g++ g++-4.8 libalgorithm-diff-perl
  libalgorithm-diff-xs-perl libalgorithm-merge-perl libdpkg-perl
  libfile-fcntllock-perl libstdc++-4.8-dev python-chardet-whl python-colorama
  python-colorama-whl python-distlib python-distlib-whl python-html5lib
  python-html5lib-whl python-pip-whl python-requests-whl python-setuptools
  python-setuptools-whl python-six-whl python-urllib3-whl python-wheel
  python3-pkg-resources
Suggested packages:
  debian-keyring g++-multilib g++-4.8-multilib gcc-4.8-doc libstdc++6-4.8-dbg
  libstdc++-4.8-doc python-genshi python-lxml python3-setuptools
...
```

Next, we will use pip to install uncompyle2.

```console
vagrant@erised:~$ sudo pip install uncompyle2
Downloading/unpacking uncompyle2
  Downloading uncompyle2-1.1.tar.gz (351kB): 351kB downloaded
  Running setup.py (path:/tmp/pip_build_root/uncompyle2/setup.py) egg_info for package uncompyle2

    warning: no files found matching 'MANIFEST'
    warning: no files found matching 'ANNOUNCE'
    warning: no files found matching 'CHANGES'
    warning: no files found matching 'TODO'
    warning: no files found matching 'uncompyle'
    warning: no previously-included files matching '*~' found anywhere in distribution
    warning: no previously-included files matching '.*~' found anywhere in distribution
Installing collected packages: uncompyle2
  Running setup.py install for uncompyle2
    changing mode of build/scripts-2.7/uncompyle2 from 644 to 755

    warning: no files found matching 'MANIFEST'
    warning: no files found matching 'ANNOUNCE'
    warning: no files found matching 'CHANGES'
    warning: no files found matching 'TODO'
    warning: no files found matching 'uncompyle'
    warning: no previously-included files matching '*~' found anywhere in distribution
    warning: no previously-included files matching '.*~' found anywhere in distribution
    changing mode of /usr/local/bin/uncompyle2 to 755
Successfully installed uncompyle2
Cleaning up...
vagrant@erised:~$
```

4. Python Crackme
-----------------

We'll work on the Python crackme in the morty directory and as with the Java
crackme, we notice that the morty.pyc is in bytecode format.

```console
vagrant@erised:~/.../sessions/session3$ cd morty/
vagrant@erised:~/.../session3/morty$ cat morty.pyc | xxd
0000000: 03f3 0d0a d60c 3e56 6300 0000 0000 0000  ......>Vc.......
0000010: 0002 0000 0040 0000 0073 3800 0000 6400  .....@...s8...d.
0000020: 0064 0100 6c00 005a 0000 6402 0084 0000  .d..l..Z..d.....
0000030: 5a01 0064 0300 8400 005a 0200 6503 0064  Z..d.....Z..e..d
0000040: 0400 6b02 0072 3400 6502 0083 0000 016e  ..k..r4.e......n
0000050: 0000 6401 0053 2805 0000 0069 ffff ffff  ..d..S(....i....
0000060: 4e63 0100 0000 0400 0000 0400 0000 4300  Nc............C.
0000070: 0000 73ab 0000 007c 0000 6a00 0064 0100  ..s....|..j..d..
0000080: 8301 007d 0100 7c01 0064 0200 197d 0200  ...}..|..d...}..
0000090: 7c02 006a 0100 8300 007c 0200 6b03 0072  |..j.....|..k..r
00000a0: 2f00 7402 0053 7403 0074 0400 7c01 0064  /.t..St..t..|..d
00000b0: 0300 196a 0000 6404 0083 0100 8302 007d  ...j..d........}
00000c0: 0300 7c03 0064 0200 197c 0300 6403 0019  ..|..d...|..d...
00000d0: 177c 0300 6405 0019 6b03 0072 6b00 7402  .|..d...k..rk.t.
00000e0: 0053 7c03 0064 0600 197c 0300 6407 0019  .S|..d...|..d...
00000f0: 6407 0014 1764 0800 167c 0300 6409 0019  d....d...|..d...
0000100: 6b03 0072 9300 7402 0053 7c01 0064 0500  k..r..t..S|..d..
0000110: 1964 0a00 6b03 0072 a700 7402 0053 7405  .d..k..r..t..St.
0000120: 0053 280b 0000 004e 7301 0000 000a 6900  .S(....Ns.....i.
0000130: 0000 0069 0100 0000 7401 0000 002d 6902  ...i....t....-i.
0000140: 0000 0069 0300 0000 6904 0000 0069 0c00  ...i....i....i..
0000150: 0000 6905 0000 0073 0d00 0000 4d61 7263  ..i....s....Marc
0000160: 656c 696e 6520 496e 6328 0600 0000 7405  eline Inc(....t.
... undicipherable stuff ...
```

Now, let's try running it.

```console
vagrant@erised:~/.../session3/morty$ python morty.pyc
Product not registed. Go away!
```

We probably need to reverse it further. Let's take a look at a method to get at
the Python bytecode.

The 'dis' module disassembles CPython bytecode.

```python
vagrant@erised:~/.../session3/morty$ python
Python 2.7.6 (default, Jun 22 2015, 17:58:13)
[GCC 4.8.2] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> import dis
>>> import morty
>>> dis.dis(morty)
Disassembly of main:
 18           0 LOAD_GLOBAL              0 (os)
              3 LOAD_ATTR                1 (path)
              6 LOAD_ATTR                2 (isfile)
              9 LOAD_CONST               1 ('license.key')
             12 CALL_FUNCTION            1
             15 POP_JUMP_IF_TRUE        33

 19          18 LOAD_CONST               2 ('Product not registed. Go away!')
             21 PRINT_ITEM
             22 PRINT_NEWLINE

 20          23 LOAD_GLOBAL              3 (exit)
             26 CALL_FUNCTION            0
             29 POP_TOP
             30 JUMP_FORWARD             0 (to 33)

 22     >>   33 LOAD_GLOBAL              4 (open)
             36 LOAD_CONST               1 ('license.key')
             39 CALL_FUNCTION            1
             42 SETUP_WITH              44 (to 89)
             45 STORE_FAST               0 (license)

 23          48 LOAD_FAST                0 (license)
             51 LOAD_ATTR                5 (read)
             54 CALL_FUNCTION            0
             57 STORE_FAST               1 (license_key)

 24          60 LOAD_GLOBAL              6 (validate)
             63 LOAD_FAST                1 (license_key)
             66 CALL_FUNCTION            1
             69 POP_JUMP_IF_FALSE       80

 25          72 LOAD_CONST               3 ('Awesome, you get to be a vampire! Registered!')
             75 PRINT_ITEM
             76 PRINT_NEWLINE
             77 JUMP_FORWARD             5 (to 85)

 27     >>   80 LOAD_CONST               4 ('Your guts get eaten!')
             83 PRINT_ITEM
             84 PRINT_NEWLINE
        >>   85 POP_BLOCK
             86 LOAD_CONST               0 (None)
        >>   89 WITH_CLEANUP
             90 END_FINALLY
             91 LOAD_CONST               0 (None)
             94 RETURN_VALUE

Disassembly of validate:
  4           0 LOAD_FAST                0 (key)
              3 LOAD_ATTR                0 (split)
              6 LOAD_CONST               1 ('\n')
              9 CALL_FUNCTION            1
             12 STORE_FAST               1 (lines)

  5          15 LOAD_FAST                1 (lines)
             18 LOAD_CONST               2 (0)
             21 BINARY_SUBSCR
             22 STORE_FAST               2 (name)

  6          25 LOAD_FAST                2 (name)
             28 LOAD_ATTR                1 (upper)
             31 CALL_FUNCTION            0
             34 LOAD_FAST                2 (name)
             37 COMPARE_OP               3 (!=)
             40 POP_JUMP_IF_FALSE       47

  7          43 LOAD_GLOBAL              2 (False)
             46 RETURN_VALUE

  8     >>   47 LOAD_GLOBAL              3 (map)
             50 LOAD_GLOBAL              4 (int)
             53 LOAD_FAST                1 (lines)
             56 LOAD_CONST               3 (1)
             59 BINARY_SUBSCR
             60 LOAD_ATTR                0 (split)
             63 LOAD_CONST               4 ('-')
             66 CALL_FUNCTION            1
             69 CALL_FUNCTION            2
             72 STORE_FAST               3 (digs)

  9          75 LOAD_FAST                3 (digs)
             78 LOAD_CONST               2 (0)
             81 BINARY_SUBSCR
             82 LOAD_FAST                3 (digs)
             85 LOAD_CONST               3 (1)
             88 BINARY_SUBSCR
             89 BINARY_ADD
             90 LOAD_FAST                3 (digs)
             93 LOAD_CONST               5 (2)
             96 BINARY_SUBSCR
             97 COMPARE_OP               3 (!=)
            100 POP_JUMP_IF_FALSE      107

 10         103 LOAD_GLOBAL              2 (False)
            106 RETURN_VALUE

 11     >>  107 LOAD_FAST                3 (digs)
            110 LOAD_CONST               6 (3)
            113 BINARY_SUBSCR
            114 LOAD_FAST                3 (digs)
            117 LOAD_CONST               7 (4)
            120 BINARY_SUBSCR
            121 LOAD_CONST               7 (4)
            124 BINARY_MULTIPLY
            125 BINARY_ADD
            126 LOAD_CONST               8 (12)
            129 BINARY_MODULO
            130 LOAD_FAST                3 (digs)
            133 LOAD_CONST               9 (5)
            136 BINARY_SUBSCR
            137 COMPARE_OP               3 (!=)
            140 POP_JUMP_IF_FALSE      147

 12         143 LOAD_GLOBAL              2 (False)
            146 RETURN_VALUE

 13     >>  147 LOAD_FAST                1 (lines)
            150 LOAD_CONST               5 (2)
            153 BINARY_SUBSCR
            154 LOAD_CONST              10 ('Marceline Inc')
            157 COMPARE_OP               3 (!=)
            160 POP_JUMP_IF_FALSE      167

 14         163 LOAD_GLOBAL              2 (False)
            166 RETURN_VALUE

 15     >>  167 LOAD_GLOBAL              5 (True)
            170 RETURN_VALUE
```

This is pretty great but we can also decompile Python bytecode into almost
perfect Python source code using uncompyle2.

```python
vagrant@erised:~/.../session3/morty$ uncompyle2 morty.pyc
# 2015.11.08 09:33:17 UTC
#Embedded file name: ./morty.py
import os

def validate(key):
    lines = key.split('\n')
    name = lines[0]
    if name.upper() != name:
        return False
    digs = map(int, lines[1].split('-'))
    if digs[0] + digs[1] != digs[2]:
        return False
    if (digs[3] + digs[4] * 4) % 12 != digs[5]:
        return False
    if lines[2] != 'Marceline Inc':
        return False
    return True


def main():
    if not os.path.isfile('license.key'):
        print 'Product not registed. Go away!'
        exit()
    with open('license.key') as license:
        license_key = license.read()
        if validate(license_key):
            print 'Awesome, you get to be a vampire! Registered!'
        else:
            print 'Your guts get eaten!'


if __name__ == '__main__':
    main()
+++ okay decompyling morty.pyc
# decompiled 1 files: 1 okay, 0 failed, 0 verify failed
# 2015.11.08 09:33:18 UTC
```

What this program does is check if the file 'license.key' exists and then
validate the license.key file. The license.key file is validated if it passes
the following constraints:

1. The first line contains a name in upper case.
2. The second line contains numbers separated by a space meeting the criteria.
3. The third line contains the string 'Marceline Inc'.

This is an example for a valid license.key file:

```
MASSIVE POTATOES
10-11-21-5-8-1
Marceline Inc
```

Let's run the program again with this sample license.key.

```
vagrant@erised:~/.../session3/morty$ python morty.pyc
Awesome, you get to be a vampire! Registered!
```

And we are done!

5. Bonus Java Content: Javasnoop!
---------------------------------

So, Ryan's presented a really cool new tool I didn't know about. It's called
Javasnoop and it can be [downloaded here](http://www.aspectsecurity.com/tools/javasnoop).

The tool lets you hook onto individual functions within the class file and
modify return values on the fly and more!

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
vagrant@erised:~/.../session3/bears$ cat Bears.class
����3E
...
undecipherable stuff
...
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

We'll work on the Python crackme in the morty directory.

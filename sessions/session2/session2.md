Session 1 - Baby Steps
======================

In this session, we'll keep it light and just practice some GDB. We'll also
install some cool scripts for GDB that make debugging less painful.

Objectives
----------
1. Install PEDA
2. GDB Primer
3. Simple reversing practice

1. Installing PEDA
------------------

Installing PEDA is really easy. Spawn up the vagrant box and run the following
commands:

```
vagrant@erised:~$ git clone https://github.com/longld/peda.git ~/peda
Cloning into '/home/vagrant/peda'...
remote: Counting objects: 276, done.
Receiving objects: 100% (276/276), 211.52 KiB | 150.00 KiB/s, done.
remote: Total 276 (delta 0), reused 0 (delta 0), pack-reused 276
Resolving deltas: 100% (171/171), done.
Checking connectivity... done.
vagrant@erised:~$ echo "source ~/peda/peda.py" >> ~/.gdbinit
vagrant@erised:~$
```

Now, let's check that PEDA has been installed.

```
vagrant@erised:~$ gdb
GNU gdb (Ubuntu 7.7-0ubuntu3) 7.7
Copyright (C) 2014 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word".
gdb-peda$
```

See that prompt there? That's how you see that peda's running. If you run the
command 'peda' you should get a pretty long help message describing the options
available with the new plugin.

```
gdb-peda$ peda
PEDA - Python Exploit Development Assistance for GDB
For latest update, check peda project page: https://github.com/longld/peda/
List of "peda" subcommands, type the subcommand to invoke it:
aslr -- Show/set ASLR setting of GDB
asmsearch -- Search for ASM instructions in memory
assemble -- On the fly assemble and execute instructions using NASM
checksec -- Check for various security options of binary
cmpmem -- Compare content of a memory region with a file
context -- Display various information of current execution context
... snip ...
waitfor -- Try to attach to new forked process; mimic "attach -waitfor"
xinfo -- Display detail information of address/registers
xormem -- XOR a memory region with a key
xprint -- Extra support to GDB's print command
xrefs -- Search for all call/data access references to a function/variable
xuntil -- Continue execution until an address or function

Type "help" followed by subcommand for full documentation.
gdb-peda$
```

Now, let's leave the special peda commands aside for now, and focus on the
essential GDB commands.

There are couple of things we want to do with GDB:

1. Running a program
2. Setting a breakpoint within a program
3. Inspecting the registers
4. Inspecting the memory
5. Inquiring about the function call stack
6. Looking up file and process metadata

### 1.1 Running a program

We will only look at single threaded binaries this session. We can cover
multithreaded applications like fork servers in another session.

Typically, you would wish to invoke gdb with the program as an argument. Let's
take Verve as an example. It's in sessions/session2/gdbpractice/verve.

```
vagrant@erised:~/.../gdbpractice/verve$ gdb ./verve
GNU gdb (Ubuntu 7.7-0ubuntu3) 7.7
Copyright (C) 2014 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./verve...done.
gdb-peda$
```

Now that we have loaded the binary, we can run it using 'run'.

```
gdb-peda$ run
Starting program:
/home/vagrant/nightsoferised/sessions/session2/gdbpractice/verve/verve
No!
[Inferior 1 (process 2352) exited with code 0143]
Warning: not running or target is remote
gdb-peda$
```

Since we're focus on just getting familiar with GDB, let's cheat a little and
take a peek at the source code in verve.c. Now, if you look at the start of
main, you should see the following snippet:

```
if (argc != 4) {
        puts(message);
        exit(99);
    }
```

Now, it's asking for 3 arguments. So we can do that in GDB by running "run arg1
arg2 arg3".

```
gdb-peda$ r abc bcd efg
Starting program:
/home/vagrant/nightsoferised/sessions/session2/gdbpractice/verve/verve abc bcd
efg
No![Inferior 1 (process 2367) exited with code 03]
Warning: not running or target is remote
gdb-peda$
```

So, that's simple enough. What if we want to debug an existing process? Quit gdb
using 'quit' and run neworder in sessions/session2/gdbpractice/neworder/. In
another terminal, check for the process id of the neworder process.

In terminal 1:

```
vagrant@erised:~/.../gdbpractice/neworder$ ./neworder
Do you know what Ed Gein said about women?


```

In terminal 2:

```
vagrant@erised:~/.../sessions/session2$ ps aux | grep neworder
vagrant   2513  0.0  0.0   2024   280 pts/0    S+   13:03   0:00 ./neworder
vagrant   2623  0.0  0.1  10460   940 pts/2    S+   13:04   0:00 grep
--color=auto neworder
vagrant@erised:~/.../sessions/session2$
```

In this case, my process id is 2513. Adjust for the results on your own
machines. Returning to terminal 2, we use GDB to attach to the process with
ptrace.

```
vagrant@erised:~/.../sessions/session2$ sudo gdb -p 2513
GNU gdb (Ubuntu 7.7-0ubuntu3) 7.7
...
Attaching to process 2513
Reading symbols from /home/vagrant/nightsoferised/sessions/session2/gdbpractice/neworder/neworder...done.
Reading symbols from /lib/i386-linux-gnu/libc.so.6...(no debugging symbols found)...done.
Loaded symbols for /lib/i386-linux-gnu/libc.so.6
Reading symbols from /lib/ld-linux.so.2...(no debugging symbols found)...done.
Loaded symbols for /lib/ld-linux.so.2
[----------------------------------registers-----------------------------------]
EAX: 0xfffffe00
EBX: 0x0
ECX: 0xffb748f8 --> 0xffb749bc --> 0xffb76889 ("XDG_SESSION_ID=2")
EDX: 0x13
ESI: 0x0
EDI: 0x0
EBP: 0xffb74918 --> 0x0
ESP: 0xffb748c8 --> 0xffb74918 --> 0x0
EIP: 0xf77ad440 (<__kernel_vsyscall+16>:        pop    ebp)
EFLAGS: 0x296 (carry PARITY ADJUST zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
   0xf77ad43c <__kernel_vsyscall+12>:   nop
   0xf77ad43d <__kernel_vsyscall+13>:   nop
   0xf77ad43e <__kernel_vsyscall+14>:   int    0x80
=> 0xf77ad440 <__kernel_vsyscall+16>:   pop    ebp
   0xf77ad441 <__kernel_vsyscall+17>:   pop    edx
   0xf77ad442 <__kernel_vsyscall+18>:   pop    ecx
   0xf77ad443 <__kernel_vsyscall+19>:   ret
   0xf77ad444:  add    BYTE PTR [esi],ch
[------------------------------------stack-------------------------------------]
0000| 0xffb748c8 --> 0xffb74918 --> 0x0
0004| 0xffb748cc --> 0x13
0008| 0xffb748d0 --> 0xffb748f8 --> 0xffb749bc --> 0xffb76889 ("XDG_SESSION_ID=2")
0012| 0xffb748d4 --> 0xf76d0bf3 (<read+35>:     pop    ebx)
0016| 0xffb748d8 --> 0xf77a0000 --> 0x1a9da8
0020| 0xffb748dc --> 0x80485d7 (<main+61>:      lea    eax,[esp+0x18])
0024| 0xffb748e0 --> 0x0
0028| 0xffb748e4 --> 0xffb748f8 --> 0xffb749bc --> 0xffb76889 ("XDG_SESSION_ID=2")
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
0xf77ad440 in __kernel_vsyscall ()
gdb-peda$
```

Notice it drops you within kernel_vsyscall. Also, notice the fancy peda
interface we'll get to play with soon enough. To allow the program to continue
execution we simply issue "continue" to GDB.

```
gdb-peda$ c
Continuing.

```

Returning to terminal 1, we can interact with the process as per normal.

```
vagrant@erised:~/.../gdbpractice/neworder$ ./neworder
Do you know what Ed Gein said about women?

stuff
Heh
vagrant@erised:~/.../gdbpractice/neworder$
```

### 1.2 Setting breakpoints


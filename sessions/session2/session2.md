Session 2 - GDB Digs Bugs!
==========================

In this session, we'll keep it light and just practice some GDB. We'll also
install some cool scripts for GDB that make debugging less painful.

Objectives
----------
1. Install PEDA
2. GDB Primer

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

2. GDB Primer
-------------

Now, let's leave the special peda commands aside for now, and focus on the
essential GDB commands.

There are couple of things we want to do with GDB:

1. Running a program
2. Setting a breakpoint within a program
3. Disassembling code
4. Stepping through code
5. Inspecting the registers
6. Inspecting the memory
7. Inquiring about the function call stack
8. Looking up file and process metadata

### Running a program

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

### Setting breakpoints within the program

It's great and all that we can run and attach to programs but we would like to
do something more useful. Breakpoints are the bread and butter of manual dynamic
analysis. They allow us to pause execution at a particular point in a program to
inspect and manipulate the register and memory states.

Note that programmers would probably use breaks by line numbers in their source
files but as reverse engineers, we do not have this luxury. So we'll mostly deal
in function symbols and addresses.

Let's take a look at verve again. This time, we will load the binary, set a
breakpoint on the main function, and run it. Note that we can say there is a
main function because it exists as a symbol:

```
vagrant@erised:~/.../gdbpractice/verve$ nm verve
...
080484ad T bitter
...
080484e3 T main
080484bf T sweet
080484d1 T symphony
...
vagrant@erised:~/.../gdbpractice/verve$
```

Running our gdb:

```
vagrant@erised:~/.../gdbpractice/verve$ gdb verve
gdb-peda$ break main
Breakpoint 1 at 0x80484ec: file verve.c, line 17.
gdb-peda$ r
Starting program: /home/vagrant/nightsoferised/sessions/session2/gdbpractice/verve/verve
[----------------------------------registers-----------------------------------]
EAX: 0x1
EBX: 0xf7fce000 --> 0x1a9da8
ECX: 0x7504f908
EDX: 0xffffd6a4 --> 0xf7fce000 --> 0x1a9da8
ESI: 0x0
EDI: 0x0
EBP: 0xffffd678 --> 0x0
ESP: 0xffffd650 --> 0x1
EIP: 0x80484ec (<main+9>:       mov    DWORD PTR [esp+0x1c],0x8048626)
EFLAGS: 0x286 (carry PARITY adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
   0x80484e4 <main+1>:  mov    ebp,esp
   0x80484e6 <main+3>:  and    esp,0xfffffff0
   0x80484e9 <main+6>:  sub    esp,0x20
=> 0x80484ec <main+9>:  mov    DWORD PTR [esp+0x1c],0x8048626
   0x80484f4 <main+17>: cmp    DWORD PTR [ebp+0x8],0x4
   0x80484f8 <main+21>: je     0x8048512 <main+47>
   0x80484fa <main+23>: mov    eax,DWORD PTR [esp+0x1c]
   0x80484fe <main+27>: mov    DWORD PTR [esp],eax
[------------------------------------stack-------------------------------------]
0000| 0xffffd650 --> 0x1
0004| 0xffffd654 --> 0xffffd714 --> 0xffffd83a ("/home/vagrant/nightsoferised/sessions/session2/gdbpractice/verve/verve")
0008| 0xffffd658 --> 0xffffd71c --> 0xffffd881 ("XDG_SESSION_ID=2")
0012| 0xffffd65c --> 0xf7e5742d (<__cxa_atexit+29>:     test   eax,eax)
0016| 0xffffd660 --> 0xf7fce3c4 --> 0xf7fcf1e0 --> 0x0
0020| 0xffffd664 --> 0xf7ffd000 --> 0x20f34
0024| 0xffffd668 --> 0x804858b (<__libc_csu_init+11>:   add    ebx,0x1a75)
0028| 0xffffd66c --> 0xf7fce000 --> 0x1a9da8
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value

Breakpoint 1, main (argc=0x1, argv=0xffffd714) at verve.c:17
17          char * message = "No!";
gdb-peda$
```

Observe that gdb is making mappings to the source file. This is not by
accident. It is specifically because the binary was compiled with the debugging
symbols.

Note that we can also break specifically on addresses. So let's maybe take
the address of main and try to break on that.

```
vagrant@erised:~/.../gdbpractice/verve$ gdb verve
gdb-peda$ break *0x80484e3
Breakpoint 1 at 0x80484e3: file verve.c, line 16.
gdb-peda$ r
Starting program: /home/vagrant/nightsoferised/sessions/session2/gdbpractice/verve/verve
[----------------------------------registers-----------------------------------]
EAX: 0x1
EBX: 0xf7fce000 --> 0x1a9da8
ECX: 0xced08ef1
EDX: 0xffffd6a4 --> 0xf7fce000 --> 0x1a9da8
ESI: 0x0
EDI: 0x0
EBP: 0x0
ESP: 0xffffd67c --> 0xf7e3da83 (<__libc_start_main+243>:        mov    DWORD PTR [esp],eax)
EIP: 0x80484e3 (<main>: push   ebp)
EFLAGS: 0x246 (carry PARITY adjust ZERO sign trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
   0x80484de <symphony+13>:     mov    eax,DWORD PTR [ebp-0x4]
   0x80484e1 <symphony+16>:     leave
   0x80484e2 <symphony+17>:     ret
=> 0x80484e3 <main>:    push   ebp
   0x80484e4 <main+1>:  mov    ebp,esp
   0x80484e6 <main+3>:  and    esp,0xfffffff0
   0x80484e9 <main+6>:  sub    esp,0x20
   0x80484ec <main+9>:  mov    DWORD PTR [esp+0x1c],0x8048626
[------------------------------------stack-------------------------------------]
0000| 0xffffd67c --> 0xf7e3da83 (<__libc_start_main+243>:       mov    DWORD PTR [esp],eax)
0004| 0xffffd680 --> 0x1
0008| 0xffffd684 --> 0xffffd714 --> 0xffffd83a ("/home/vagrant/nightsoferised/sessions/session2/gdbpractice/verve/verve")
0012| 0xffffd688 --> 0xffffd71c --> 0xffffd881 ("XDG_SESSION_ID=2")
0016| 0xffffd68c --> 0xf7feacea (add    ebx,0x12316)
0020| 0xffffd690 --> 0x1
0024| 0xffffd694 --> 0xffffd714 --> 0xffffd83a ("/home/vagrant/nightsoferised/sessions/session2/gdbpractice/verve/verve")
0028| 0xffffd698 --> 0xffffd6b4 --> 0xf6c90ae1
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value

Breakpoint 1, main (argc=0x1, argv=0xffffd714) at verve.c:16
16      int main(int argc, char ** argv) {
gdb-peda$
```

If you issue 'breakpoint' with no arguments, you will set a breakpoint on the
current instruction pointer.

You should probably be aware that your GDB looks quite drastically different
from the last session. With a lot more information being presented after a
breakpoint hits.

### Disassembling

To disassemble at the current instruction pointer, you can simply issue
'disassemble'.

To disassemble another function, you can use the address as the argument or
specify the symbolic name.

Let's try disassembling the bitter function.

```
gdb-peda$ disas bitter
Dump of assembler code for function bitter:
   0x080484ad <+0>:     push   ebp
   0x080484ae <+1>:     mov    ebp,esp
   0x080484b0 <+3>:     sub    esp,0x10
   0x080484b3 <+6>:     mov    DWORD PTR [ebp-0x4],0x8048610
   0x080484ba <+13>:    mov    eax,DWORD PTR [ebp-0x4]
   0x080484bd <+16>:    leave
   0x080484be <+17>:    ret
End of assembler dump.
gdb-peda$ disas 0x080484ad
Dump of assembler code for function bitter:
   0x080484ad <+0>:     push   ebp
   0x080484ae <+1>:     mov    ebp,esp
   0x080484b0 <+3>:     sub    esp,0x10
   0x080484b3 <+6>:     mov    DWORD PTR [ebp-0x4],0x8048610
   0x080484ba <+13>:    mov    eax,DWORD PTR [ebp-0x4]
   0x080484bd <+16>:    leave
   0x080484be <+17>:    ret
End of assembler dump.
gdb-peda$
```

### Stepping through the code

Of course, it would be pretty useful for us if we can take the program step by
step and watching how the internal states change. So we can achieve this by
stepping through the instructions. To do this, you can use 'stepi'.

Try stepping until you reach instruction 0x8048501.


### Inspecting the registers

Assuming you're halted at 0x8048501, we can use 'info reg' to view the contents
of the registers at that particular moment of execution.

```
gdb-peda$ info reg
eax            0x8048626    0x8048626
ecx            0x1d7f6388   0x1d7f6388
edx            0xffffd6a4   0xffffd6a4
ebx            0xf7fce000   0xf7fce000
esp            0xffffd650   0xffffd650
ebp            0xffffd678   0xffffd678
esi            0x0  0x0
edi            0x0  0x0
eip            0x8048501    0x8048501 <main+30>
eflags         0x293    [ CF AF SF IF ]
cs             0x23 0x23
ss             0x2b 0x2b
ds             0x2b 0x2b
es             0x2b 0x2b
fs             0x0  0x0
gs             0x63 0x63
gdb-peda$
```

However, we have something better with peda installed. Issue the command
'context register'.

```
gdb-peda$ context register
[----------------------------------registers-----------------------------------]
EAX: 0x8048626 --> 0x216f4e ('No!')
EBX: 0xf7fce000 --> 0x1a9da8
ECX: 0x1d7f6388
EDX: 0xffffd6a4 --> 0xf7fce000 --> 0x1a9da8
ESI: 0x0
EDI: 0x0
EBP: 0xffffd678 --> 0x0
ESP: 0xffffd650 --> 0x8048626 --> 0x216f4e ('No!')
EIP: 0x8048501 (<main+30>:  call   0x8048370 <puts@plt>)
EFLAGS: 0x293 (CARRY parity ADJUST zero SIGN trap INTERRUPT direction overflow)
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
gdb-peda$
```

That's a lot more readable and with pointer resolutions for easier analysis. We
even get automated hex to string conversions!

What if we want to just see what's in one register though? We can use 'print'.

```
gdb-peda$ print $eax
$1 = 0x8048626
gdb-peda$
```

### Inspecting the memory



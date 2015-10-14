Session 1 - Baby Steps
======================

This first session will be a short one. Just to introduce the environment we
will be working in and the bread-and-butter tools we will be starting out with.
Now, we will be only working with x86-64 binaries in this session and a little
technical knowledge is assumed.


Objectives
----------
0. Clone the Erised repo
1. Basic Tools Set Up
2. Introduction to Basic Tools
3. Quick Assembly Primer
4. Simple Crackme Warmup

Cloning the Erised Repo
-----------------------

In your virtualised environment you should have set up in Session 0, clone the
Erised repository to your home directory. We'll have to update it every session
with the new content in the repository.

```
vagrant@erised:~$ git clone
https://github.com/nnamon/nightsoferised.git
Cloning into 'nightsoferised'...
remote: Counting objects: 33, done.
remote: Compressing objects: 100% (21/21), done.
remote: Total 33 (delta 4), reused 32 (delta 3), pack-reused 0
Unpacking objects: 100% (33/33), done.
Checking connectivity... done.
```

Basic Tools Set Up
------------------
We will be restricting ourselves to very simple tools for this session. So
there's actually nothing we need to set up. However, for completeness sake,
we'll check we have the following tools ready:

1. nm
2. ltrace
3. strace
4. gdb
5. objdump
6. file
7. strings

Introduction to Basic Tools
---------------------------

Note we have restricted ourselves to the most minimal of tools. No fancy
decompilation tools or IR lifters yet :)

Let's go through what each thing does.

### nm

nm is a tool from GNU binutils that lists the symbols from an object or binary
file.

We're not going to mess too much with the flags, so let's just use it vanilla on
our demotools program.

Hint: It's located in sessions/session1/demotools/.

```
vagrant@erised:~$ cd nightsoferised/sessions/session1/demotools/
vagrant@erised:~/.../session1/demotools$ nm demotools
0000000000601048 B __bss_start
0000000000601048 b completed.6973
0000000000601038 D __data_start
0000000000601038 W data_start
00000000004004c0 t deregister_tm_clones
0000000000400530 t __do_global_dtors_aux
0000000000600e18 t __do_global_dtors_aux_fini_array_entry
0000000000601040 D __dso_handle
0000000000600e28 d _DYNAMIC
0000000000601048 D _edata
0000000000601050 B _end
0000000000400684 T _fini
0000000000400550 t frame_dummy
0000000000600e10 t __frame_dummy_init_array_entry
00000000004007f8 r __FRAME_END__
0000000000601000 d _GLOBAL_OFFSET_TABLE_
                 w __gmon_start__
0000000000400418 T _init
0000000000600e18 t __init_array_end
0000000000600e10 t __init_array_start
0000000000400690 R _IO_stdin_used
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
0000000000600e20 d __JCR_END__
0000000000600e20 d __JCR_LIST__
                 w _Jv_RegisterClasses
0000000000400680 T __libc_csu_fini
0000000000400610 T __libc_csu_init
                 U __libc_start_main@@GLIBC_2.2.5
000000000040057d T main
                 U printf@@GLIBC_2.2.5
00000000004004f0 t register_tm_clones
00000000004005b0 T say_hello
0000000000400490 T _start
                 U strcmp@@GLIBC_2.2.5
0000000000601048 D __TMC_END__
```

The letters mean some pretty important things. D for example, means that that
symbols resides with the data section, T, the text or code section, R means
read-only data, etc. Check the manual page for a full description of all the
flags.

Now, these symbols aren't always included in the binaries (usually they are
stripped in CTFs) but when they are, they're really useful for reversing.


### ltrace

Now, ltrace is a really cool GNU binutils tool that lets you trace library cools
during the program's execution.

'nuff said.

```
vagrant@erised:~/.../session1/demotools $  ltrace ./demotools
__libc_start_main(0x40057d, 1, 0x7fffd0a999d8, 0x400610 <unfinished ...>
printf(nil)                                                                                  = -1
strcmp("Goaway!\n", "Helloo!\n")                                                             = -1
printf("Helloo!\n"Helloo!
)                                                                          = 8
+++ exited (status 8) +++
```

### strace

Likewise, strace lets you trace system calls and signals during the program's
execution.

```
vagrant@erised:~/.../session1/demotools$ strace ./demotools
execve("./demotools", ["./demotools"], [/* 20 vars */]) = 0
brk(0)                                  = 0x104c000
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f04942d4000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
open("/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=28039, ...}) = 0
mmap(NULL, 28039, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f04942cd000
close(3)                                = 0
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
open("/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\320\37\2\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0755, st_size=1840928, ...}) = 0
mmap(NULL, 3949248, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f0493cef000
mprotect(0x7f0493eaa000, 2093056, PROT_NONE) = 0
mmap(0x7f04940a9000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1ba000) = 0x7f04940a9000
mmap(0x7f04940af000, 17088, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7f04940af000
close(3)                                = 0
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f04942cc000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f04942ca000
arch_prctl(ARCH_SET_FS, 0x7f04942ca740) = 0
mprotect(0x7f04940a9000, 16384, PROT_READ) = 0
mprotect(0x600000, 4096, PROT_READ)     = 0
mprotect(0x7f04942d6000, 4096, PROT_READ) = 0
munmap(0x7f04942cd000, 28039)           = 0
fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(136, 0), ...}) = 0
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f04942d3000
write(1, "Helloo!\n", 8Helloo!
)                = 8
exit_group(8)                           = ?
+++ exited with 8 +++
```


### gdb

Well, this is a little too large to cover in one section but let's just
demonstrate how to run a binary.

```
vagrant@erised:~/.../session1/demotools$ gdb demotools
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
Reading symbols from demotools...(no debugging symbols found)...done.
(gdb) r
Starting program: /home/vagrant/nightsoferised/sessions/session1/demotools/demotools
Helloo!
[Inferior 1 (process 1356) exited with code 010]
(gdb) Quit
```


### objdump

objdump is actually the swiss army knife of extracting information from a
binary. You can do a whole lot of things with it, but for now, let's just do a
disassembly.

```
vagrant@erised:~/.../session1/demotools$ objdump -d demotools

demotools:     file format elf64-x86-64


Disassembly of section .init:

0000000000400418 <_init>:
  400418:       48 83 ec 08             sub    $0x8,%rsp
  40041c:       48 8b 05 d5 0b 20 00    mov    0x200bd5(%rip),%rax        # 600ff8 <_DYNAMIC+0x1d0>
  400423:       48 85 c0                test   %rax,%rax
  400426:       74 05                   je     40042d <_init+0x15>
  400428:       e8 53 00 00 00          callq  400480 <__gmon_start__@plt>
  40042d:       48 83 c4 08             add    $0x8,%rsp
  400431:       c3                      retq

... snip ...

000000000040057d <main>:
  40057d:       55                      push   %rbp
  40057e:       48 89 e5                mov    %rsp,%rbp
  400581:       48 83 ec 10             sub    $0x10,%rsp
  400585:       89 7d fc                mov    %edi,-0x4(%rbp)
  400588:       48 89 75 f0             mov    %rsi,-0x10(%rbp)
  40058c:       48 8b 45 f0             mov    -0x10(%rbp),%rax
  400590:       48 83 c0 08             add    $0x8,%rax
  400594:       48 8b 00                mov    (%rax),%rax
  400597:       48 89 c7                mov    %rax,%rdi
  40059a:       b8 00 00 00 00          mov    $0x0,%eax
  40059f:       e8 ac fe ff ff          callq  400450 <printf@plt>
  4005a4:       b8 00 00 00 00          mov    $0x0,%eax
  4005a9:       e8 02 00 00 00          callq  4005b0 <say_hello>
  4005ae:       c9                      leaveq
  4005af:       c3                      retq

00000000004005b0 <say_hello>:
  4005b0:       55                      push   %rbp
  4005b1:       48 89 e5                mov    %rsp,%rbp
  4005b4:       48 83 ec 10             sub    $0x10,%rsp
  4005b8:       48 c7 45 f0 94 06 40    movq   $0x400694,-0x10(%rbp)
  4005bf:       00
  4005c0:       48 c7 45 f8 9d 06 40    movq   $0x40069d,-0x8(%rbp)
  4005c7:       00
  4005c8:       48 8b 55 f8             mov    -0x8(%rbp),%rdx
  4005cc:       48 8b 45 f0             mov    -0x10(%rbp),%rax
  4005d0:       48 89 d6                mov    %rdx,%rsi
  4005d3:       48 89 c7                mov    %rax,%rdi
  4005d6:       e8 95 fe ff ff          callq  400470 <strcmp@plt>
  4005db:       85 c0                   test   %eax,%eax
  4005dd:       75 11                   jne    4005f0 <say_hello+0x40>
  4005df:       48 8b 45 f0             mov    -0x10(%rbp),%rax
  4005e3:       48 89 c7                mov    %rax,%rdi
  4005e6:       b8 00 00 00 00          mov    $0x0,%eax
  4005eb:       e8 60 fe ff ff          callq  400450 <printf@plt>
  4005f0:       48 8b 45 f8             mov    -0x8(%rbp),%rax
  4005f4:       48 89 c7                mov    %rax,%rdi
  4005f7:       b8 00 00 00 00          mov    $0x0,%eax
  4005fc:       e8 4f fe ff ff          callq  400450 <printf@plt>
  400601:       c9                      leaveq
  400602:       c3                      retq
  400603:       66 2e 0f 1f 84 00 00    nopw   %cs:0x0(%rax,%rax,1)
  40060a:       00 00 00
  40060d:       0f 1f 00                nopl   (%rax)

... snip ...
```

### file

file is something I use for everything from crypto to forensics to reversing.
It's the most amazing tool in the world. Seriously.

```
vagrant@erised:~/.../session1/demotools$ file demotools
demotools: ELF 64-bit LSB  executable, x86-64, version 1 (SYSV), dynamically
linked (uses shared libs), for GNU/Linux 2.6.24,
BuildID[sha1]=1b01e4e6f5d0a64f26fae1dd36f100fbb20f54dc, not stripped
```

### strings

Another basic tool that you should always use to get a better idea of what you
are looking at. Don't run it vanilla on [untrusted binaries
though](http://lcamtuf.blogspot.sg/2014/10/psa-dont-run-strings-on-untrusted-files.html).

```
vagrant@erised:~/.../session1/demotools$ strings demotools
/lib64/ld-linux-x86-64.so.2
libc.so.6
printf
strcmp
GLIBC_2.2.5
UH-H
UH-H
Goaway!
Helloo!
;*3$"
GCC: (Ubuntu 4.8.4-2ubuntu1~14.04) 4.8.4
GCC: (Ubuntu 4.8.2-19ubuntu1) 4.8.2
... snip ...
text
.fini
.rodata
.eh_frame_hdr
.eh_frame
.init_array
.fini_array
.jcr
.dynamic
.got
.got.plt
.data
.bss
... snip ...
data_start
_edata
_fini
printf@@GLIBC_2.2.5
strcmp@@GLIBC_2.2.5
_start
main
say_hello
... snip ...
```

Quick Assembly Primer
---------------------

Now, obviously, assembly, especially x86-64 assembly is not something you can
get used to in an hour. So we'll just cover superficially what you need to be
able to read for this session.

Consider this fragment:

```
000000000040057d <main>:
  40057d:       55                      push   %rbp
  40057e:       48 89 e5                mov    %rsp,%rbp
  400581:       48 83 ec 10             sub    $0x10,%rsp
  400585:       89 7d fc                mov    %edi,-0x4(%rbp)
  400588:       48 89 75 f0             mov    %rsi,-0x10(%rbp)
  40058c:       48 8b 45 f0             mov    -0x10(%rbp),%rax
  400590:       48 83 c0 08             add    $0x8,%rax
  400594:       48 8b 00                mov    (%rax),%rax
  400597:       48 89 c7                mov    %rax,%rdi
  40059a:       b8 00 00 00 00          mov    $0x0,%eax
  40059f:       e8 ac fe ff ff          callq  400450 <printf@plt>
  4005a4:       b8 00 00 00 00          mov    $0x0,%eax
  4005a9:       e8 02 00 00 00          callq  4005b0 <say_hello>
  4005ae:       c9                      leaveq
  4005af:       c3                      retq
```

This is in AT&T or GAS syntax which means that most instructions take the form

```
mnenomic source, destination
```

Also, mnenomics tend to have operation suffixes based on the size of the operand
that is being manipulated.

There are sigils involved as well, registers are prefixed with % and constant
numbers are prefixed with $.

Now, contrast the above with something in Intel syntax:

```
Dump of assembler code for function main:
   0x000000000040057d <+0>:     push   rbp
   0x000000000040057e <+1>:     mov    rbp,rsp
   0x0000000000400581 <+4>:     sub    rsp,0x10
   0x0000000000400585 <+8>:     mov    DWORD PTR [rbp-0x4],edi
   0x0000000000400588 <+11>:    mov    QWORD PTR [rbp-0x10],rsi
   0x000000000040058c <+15>:    mov    rax,QWORD PTR [rbp-0x10]
   0x0000000000400590 <+19>:    add    rax,0x8
   0x0000000000400594 <+23>:    mov    rax,QWORD PTR [rax]
   0x0000000000400597 <+26>:    mov    rdi,rax
   0x000000000040059a <+29>:    mov    eax,0x0
   0x000000000040059f <+34>:    call   0x400450 <printf@plt>
   0x00000000004005a4 <+39>:    mov    eax,0x0
   0x00000000004005a9 <+44>:    call   0x4005b0 <say_hello>
   0x00000000004005ae <+49>:    leave
   0x00000000004005af <+50>:    ret
```

Now, the order of the source and destination operands is flipped compared to
AT&T syntax. Also, square brackets.

Important operations to know are:

```
40057d: 55                      push   %rbp
40066a: 5b                      pop    %rbx
```

Push operations manipulate data on the stack. The stack frame is usually
handled through storing addresses in the %rbp and %rsp registers. Every time a
push happens, the data is pushed into the addressed at %rsp and %rsp is
incremented (the size depends on the architecture). Pop operations are the
reverse. Data from memory is put into a register and the %rsp is decremented.

```
400588: 48 89 75 f0             mov    %rsi,-0x10(%rbp)
```

Move operations are fairly common. They generally move data from register to
register, register to memory, or memory to register. Offsets from the addresses
stored in registers are also allowed.

```
400581: 48 83 ec 10             sub    $0x10,%rsp
40042d: 48 83 c4 08             add    $0x8,%rsp
400634: 31 db                   xor    %ebx,%ebx
400499: 48 83 e4 f0             and    $0xfffffffffffffff0,%rsp
```

Arithmetic operations are also fairly common.

```
4005fc: e8 4f fe ff ff          callq  400450 <printf@plt>
4005ae: c9                      leaveq
4005af: c3                      retq
```

These instructions are the main ways a program handles function calls and
returning from those function calls. We won't talk about the call stack this
session but we will in future sessions.

```
400530: 80 3d 11 0b 20 00 00    cmpb   $0x0,0x200b11(%rip)
400537: 75 11                   jne    40054a <__do_global_dtors_aux+0x1a>
```

Branching instructions are also very important as they determine which path the
program execution takes. These work because comparison (and some arithmetic)
instructions set certain flags in the processor. These flags are used to check
whether to take a jump or not.

```
400527: ff e2                   jmpq   *%rdx
```

Of course, there are also non-conditional branching instructions.


Simple Crackme Warmup
---------------------

Now that's out of the way, let's play around with two simple crackme exercises.


### Babysimple

Let's try and run the binary.

```
vagrant@erised:~$ cd nightsoferised/sessions/session1/babysimple/
vagrant@erised:~/.../session1/babysimple$ ./babysimple
Usage: ./babysimple <guess>
vagrant@erised:~/.../session1/babysimple$ ./babysimple asd
Wrong!
vagrant@erised:~/.../session1/babysimple$ ./babysimple 123
Wrong!
```

Okay, let's do our secret file magic!

```
vagrant@erised:~/.../session1/babysimple$ file babysimple
babysimple: ELF 32-bit LSB  executable, Intel 80386, version 1 (SYSV),
dynamically linked (uses shared libs), for GNU/Linux 2.6.32,
BuildID[sha1]=0ce4c7e422ebc8dec9d03374d5a3263e154982fa, not stripped
```

It's a simple unstripped 64 bit elf binary. Now for some strings magic!

```
vagrant@erised:~/.../session1/babysimple$ strings babysimple
/lib/ld-linux.so.2
libc.so.6
_IO_stdin_used
puts
strtol
__libc_start_main
__gmon_start__
GLIBC_2.0
PTRh
QVh+
UWVS
t$,U
[^_]
Usage: ./babysimple <guess>
Correct!
Wrong!
;*2$"(
GCC: (GNU) 5.2.0
.symtab
.strtab
.shstrtab
.interp
... snip ...
```

Looks like our objective is to get the program to output 'Correct!'

Now, if we objdump the file to disassemble it, we can see that there's only
the main function that's worth looking at:

```
vagrant@erised:~/.../session1/babysimple$ objdump -d babysimple
... snip ...
0804842b <main>:
 804842b:       8d 4c 24 04             lea    0x4(%esp),%ecx
 804842f:       83 e4 f0                and    $0xfffffff0,%esp
 8048432:       ff 71 fc                pushl  -0x4(%ecx)
 8048435:       55                      push   %ebp
 8048436:       89 e5                   mov    %esp,%ebp
 8048438:       51                      push   %ecx
 8048439:       83 ec 14                sub    $0x14,%esp
 804843c:       89 c8                   mov    %ecx,%eax
 804843e:       83 38 01                cmpl   $0x1,(%eax)
 8048441:       7f 17                   jg     804845a <main+0x2f>
 8048443:       83 ec 0c                sub    $0xc,%esp
 8048446:       68 30 85 04 08          push   $0x8048530
 804844b:       e8 a0 fe ff ff          call   80482f0 <puts@plt>
 8048450:       83 c4 10                add    $0x10,%esp
 8048453:       b8 01 00 00 00          mov    $0x1,%eax
 8048458:       eb 4b                   jmp    80484a5 <main+0x7a>
 804845a:       8b 40 04                mov    0x4(%eax),%eax
 804845d:       83 c0 04                add    $0x4,%eax
 8048460:       8b 00                   mov    (%eax),%eax
 8048462:       83 ec 04                sub    $0x4,%esp
 8048465:       6a 0a                   push   $0xa
 8048467:       6a 00                   push   $0x0
 8048469:       50                      push   %eax
 804846a:       e8 b1 fe ff ff          call   8048320 <strtoul@plt>
 804846f:       83 c4 10                add    $0x10,%esp
 8048472:       89 45 f4                mov    %eax,-0xc(%ebp)
 8048475:       81 7d f4 ef be ad de    cmpl   $0xdeadbeef,-0xc(%ebp)
 804847c:       75 12                   jne    8048490 <main+0x65>
 804847e:       83 ec 0c                sub    $0xc,%esp
 8048481:       68 4c 85 04 08          push   $0x804854c
 8048486:       e8 65 fe ff ff          call   80482f0 <puts@plt>
 804848b:       83 c4 10                add    $0x10,%esp
 804848e:       eb 10                   jmp    80484a0 <main+0x75>
 8048490:       83 ec 0c                sub    $0xc,%esp
 8048493:       68 55 85 04 08          push   $0x8048555
 8048498:       e8 53 fe ff ff          call   80482f0 <puts@plt>
 804849d:       83 c4 10                add    $0x10,%esp
 80484a0:       b8 00 00 00 00          mov    $0x0,%eax
 80484a5:       8b 4d fc                mov    -0x4(%ebp),%ecx
 80484a8:       c9                      leave
 80484a9:       8d 61 fc                lea    -0x4(%ecx),%esp
 80484ac:       c3                      ret
 80484ad:       66 90                   xchg   %ax,%ax
 80484af:       90                      nop
... snip ...
```

Now, can we make sense of what's happening here? First, we identify
the bit that checks whether there are any arguments present:

```
 804843c:       89 c8                   mov    %ecx,%eax
 804843e:       83 38 01                cmpl   $0x1,(%eax)
 8048441:       7f 17                   jg     804845a <main+0x2f>
 8048443:       83 ec 0c                sub    $0xc,%esp
 8048446:       68 30 85 04 08          push   $0x8048530
 804844b:       e8 a0 fe ff ff          call   80482f0 <puts@plt>
```

Now, we can guess that whatever is in %eax here is probably the number
of arguments to the C program (or the argc). How do we know? Let's
verify by starting gdb and checking. Let's break on 0x804843e.

```
vagrant@erised:~/.../session1/babysimple$ gdb babysimple
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
Reading symbols from babysimple...(no debugging symbols found)...done.
(gdb) br *0x804843e
Breakpoint 1 at 0x804843e
(gdb) r
Starting program: /home/vagrant/nightsoferised/sessions/session1/babysimple/babysimple

Breakpoint 1, 0x0804843e in main ()
(gdb) x/xw $eax
0xffffd690:     0x00000001
```

Well, a value of one is just what you'd expect from giving a program no arguments.
Let's try to give it some arguments this time.

```
(gdb) r 123
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/vagrant/nightsoferised/sessions/session1/babysimple/babysimple 123

Breakpoint 1, 0x0804843e in main ()
(gdb) x/xw $eax
0xffffd690:     0x00000002
(gdb) r 123 123
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/vagrant/nightsoferised/sessions/session1/babysimple/babysimple 123 123

Breakpoint 1, 0x0804843e in main ()
(gdb) x/xw $eax
0xffffd670:     0x00000003
```

Our hunch was right. So it is the number of arguments. Now, what does it do with
this information?

```
 804843e:       83 38 01                cmpl   $0x1,(%eax)
 8048441:       7f 17                   jg     804845a <main+0x2f>
```

It checks if it's greater than it of course! So there must be at least one argument.
Indeed, if there isn't it prints something.

```
 8048443:       83 ec 0c                sub    $0xc,%esp
 8048446:       68 30 85 04 08          push   $0x8048530
 804844b:       e8 a0 fe ff ff          call   80482f0 <puts@plt>
```

We can figure out what it prints in gdb.

```
(gdb) x/s 0x8048530
0x8048530:      "Usage: ./babysimple <guess>"
```

So it's just a usage string. Let's move on.

```
 804845a:       8b 40 04                mov    0x4(%eax),%eax
 804845d:       83 c0 04                add    $0x4,%eax
 8048460:       8b 00                   mov    (%eax),%eax
 8048462:       83 ec 04                sub    $0x4,%esp
 8048465:       6a 0a                   push   $0xa
 8048467:       6a 00                   push   $0x0
 8048469:       50                      push   %eax
 804846a:       e8 b1 fe ff ff          call   8048320 <strtoul@plt>
```

This is where it jumps to if there are arguments to the program.
What is it doing? It's taking the one word after the address in
%eax. And recall that previously, the address that %eax contained
contains the argc. One word after that is probably the address of
argv. And then, it takes the address one word after that. This is
probably our first argument. That is argv[1].

After that it calls strtoul on it.

How can we verify this? Let's be lazy and use ltrace since a library
call is involved.

```
vagrant@erised:~/.../session1/babysimple$ ltrace ./babysimple 123
__libc_start_main(0x804842b, 2, 0xff9cd154, 0x80484b0 <unfinished ...>
strtol(0xff9ce8bb, 0, 10, 0x80484fb)                                                         = 123
puts("Wrong!"Wrong!
)                                                                               = 7
+++ exited (status 0) +++
```

Yep, it's calling strtol on our supplied argument. So what does it do
after converting our argument?

```
 8048475:       81 7d f4 ef be ad de    cmpl   $0xdeadbeef,-0xc(%ebp)
 804847c:       75 12                   jne    8048490 <main+0x65>
```

Looks like it compares the result to 0xdeadbeef. So does mean we can
just supply 0xdeadbeef? :D

```
vagrant@erised:~/.../session1/babysimple$ ./babysimple 0xdeadbeef
Wrong!
```

Haha, no. If you recall the call to strtoul was this:

```
strtoul(0xff9ce8bb, 0, 10, 0x80484fb)
```

Now, if we check the manual entry for strtoul:

```
 long int strtoul(const char *nptr, char **endptr, int base);
```

Now, the base is the radix of the numerical representation it parses.
So we can easily see that the base is 10. Let's convert our proposed
check number to decimal.

```
vagrant@erised:~/.../session1/babysimple$ python -c 'print 0xdeadbeef'
3735928559
```

Now, let's use this to solve our crackme

```
vagrant@erised:~/.../session1/babysimple$ ./babysimple 3735928559
Correct!
```

Solved!


### Babyme

<I'll do this with you during the session>
<but I'll fill this out in full later>

### Babymagic

This one I'll leave as an exercise to you :)
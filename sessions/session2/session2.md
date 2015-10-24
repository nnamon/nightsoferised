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

I. Running a program
II. Setting a breakpoint within a program
III. Inspecting the registers
IV. Inspecting the memory
V. Inquiring about the function call stack
VI. Looking up file and process metadata



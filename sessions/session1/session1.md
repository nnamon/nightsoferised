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


Quick Assembly Primer
---------------------


Simple Crackme Warmup
---------------------


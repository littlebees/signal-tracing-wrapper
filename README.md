# A small wrapper to trace who kills

## Motivation
I want to know who kill my daemon.

However, the linux kernel I am using is quite old, so I can't use ebpf to trace.

And I wrote this small wrapper to trace.

## How to use

1. edit `handler.cpp` to decide to trace which signals
2. edit `utils.h` to change where to place the real daemon executable
3. `make`
4. `./wrapper <daemon location> <args>...`
5. Now, the daemon location is this wrapper, and the real executable is moved to the folder you specified in utils.h.
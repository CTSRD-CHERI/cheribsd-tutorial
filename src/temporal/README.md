# Heap temporal memory safety

CheriBSD 23.11 incorporates support for userlevel heap temporal memory
safety based on a load-barrier extension to
[Cornucopia](https://www.cl.cam.ac.uk/research/security/ctsrd/pdfs/2020oakland-cornucopia.pdf),
which is inspired by garbage-collection techniques.
This feature involves a collaboration between the kernel (which provides
asynchronous capability revocation with VM acceleration) and the userlevel
heap allocator (which quarantines freed memory until revocation of any
pointers to it) to ensure that memory cannot be reallocated until there are
no outstanding valid capabilities lasting from its previous allocation.

This activity involves source code in the `temporal` subdirectory:

```
% cd temporal
```

## Checking whether temporal safety is globally enabled

Use the `sysctl(8)` command to inspect the value of the
`security.cheri.runtime_revocation_default` system MIB entry:

```
% sysctl security.cheri.runtime_revocation_default
```

This sysctl sets the default policy for revocation used by processes on
startup.

## Controlling revocation by binary and process

*XXXRW: This section to be written.*

## Exercising a use-after-free bug

Compile the program `use-after-free.c`:

```
% cc -Wall -g -o use-after-free use-after-free.c
```

Run the program to see what happens when a use-after-free bug is exercised:

```
% ./use-after-free
```

Why doesn't the program crash?

## Synchronous revocation

Revocation normally occurs asynchronously, with a memory quarantine preventing
memory reuse until revocation of any pointers to that memory.
Uncomment this line in `use-after-free.c` to trigger a synchronous revocation
before the use-after-free memory access:

```
/* malloc_revoke(). */
```

Recompile and re-run the program, this time under GDB, and observe its
behavior.
Which line faults, and why?

## Monitoring revocation in processes

Use the `procstat cheri -v` command to inspect the CHERI memory-safety state
of a target process.
For example:

```
# procstat cheri -v 923 1012
  PID COMM                C QUAR  RSTATE                              EPOCH
  923 seatd               P  yes    none                                  0
 1012 Xorg                P  yes    none                               0xd2
```

Both processes in this example use a pure-capability process environment, have
quarantining enabled, and are not currently revoking.
seatd has never needed to perform a revocation pass, as it remains in epoch 0,
whereas X.org has a non-zero epoch and has performed multiple passes.

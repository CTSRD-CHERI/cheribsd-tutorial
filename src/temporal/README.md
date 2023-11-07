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
The userlevel memory allocator and the kernel revoker share an `epoch counter`
that counts the number of completed atomic revocation sweeps.
Memory added to quarantine in one epoch cannot be removed from quarantine
until at least one complete epoch has passed -- i.e., the epoch counter has
been increased by 2.
More information on temporal memory safety support can be found in the
[mrs(3) man page](https://man.cheribsd.org/cgi-bin/man.cgi/dev/mrs):

```
man mrs
```

This activity involves source code in the `temporal` subdirectory:

```
cd ~/tutorial/src/temporal
```

## Checking whether temporal safety is globally enabled

Use the `sysctl(8)` command to inspect the value of the
`security.cheri.runtime_revocation_default` system MIB entry:

```
sysctl security.cheri.runtime_revocation_default
```

This sysctl sets the default policy for revocation used by processes on
startup.
We recommend setting this in `/boot/loader.conf`, which is processed by the
boot loader before any user processes start.

## Controlling revocation by binary or process


You can forcefully enable or disable revocations for a specific binary or
process
with
[elfctl(1)](https://man.cheribsd.org/cgi-bin/man.cgi/dev/elfctl)
or
[proccontrol(1)](https://man.cheribsd.org/cgi-bin/man.cgi/dev/proccontrol)
and ignore the default policy:

```
elfctl -e <+cherirevoke or +nocherirevoke> <binary>
```

```
proccontrol -m cherirevoke -s <enable or disable> <program with arguments>
```

You can read more on these commands in the
[mrs(3) man page](https://man.cheribsd.org/cgi-bin/man.cgi/dev/mrs).


## Exercising a use-after-free bug

Compile the program `use-after-free.c`:

```
cc -Wall -g -o use-after-free use-after-free.c
```

Run the program to see what happens when a use-after-free bug is exercised:

```
./use-after-free
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

 * Which line in the program faults, and why?

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

Modify `helloworld.c` to await user input before and after the call to
`cheri_revoke()` using the POSIX `gets(3)` API; run `helloworld`.
In a second login session, use the `ps(1)` command to obtain the process ID,
and then use `procstat cheri` to inspects its protection state on either side
of the revocation call.

 * What is EPOCH before and after calling `cheri_revoke()` -- and why?

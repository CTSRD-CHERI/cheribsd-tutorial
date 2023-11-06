# Benchmark ABI

The Benchmark ABI makes minor changes to pure-capability code generation to
address a limitation in Morello's branch prediction when PCC bounds may
change.
This limitation penalizes the performance of function calls and returns where
PCC bounds are required shortly after the jump, leading to instruction stalls.
Benchmark ABI code uses integer jumps rather than capability jumps, and
Benchmark ABI processes use a PCC with bounds covering the full address space.
Further details regarding Morello performance, and the Benchmark ABI, may be
found in [Early performance results from the prototype Morello
microarchitecture](https://ctsrd-cheri.github.io/morello-early-performance-results/cover/).

This activity involves source code in the `benchmark` subdirectory:

```
cd ~/tutorial/src/benchmark
```

## Compiling code for the Benchmark ABI

Compile `helloworld.c` as a CheriABI binary:

```
cc -Wall -g -o helloworld-cheriabi helloworld.c
```

Compile `helloworld.c` as a Benchmark ABI binary:

```
cc -Wall -g -mabi=purecap-benchmark -o helloworld-benchmark helloworld.c
```

## Identifying Benchmark ABI binaries

Use the `file(1)` command to identify the two binaries:

```
file helloworld-cheriabi
```
```
file helloworld-benchmark
```

Inspect ELF binary headers with `readelf` (from the base system) or
`llvm-readelf` (from LLVM for Morello):

```
readelf -n helloworld-cheriabi
```
```
readelf -n helloworld-benchmark
```
```
llvm-readelf -n helloworld-cheriabi
```
```
llvm-readelf -n helloworld-benchmark
```

## Running Benchmark ABI binaries

Run both binaries from the command line:

```
./helloworld-cheriabi
```
```
./helloworld-benchmark
```

## The Benchmark ABI package manager

Using `pkg64cb`, which manages a complete set of third-party software packages
compiled for the Benchmark ABI, list the currently installed Benchmark ABI
package:

```
pkg64cb info
```

Now install the Benchmark ABI compilation of `bash`:

```
sudo pkg64cb install bash
```

You can read more on useful package manager commands in the
[Getting Started with CheriBSD guide](https://www.cheribsd.org/getting-started/23.11/packages/commands.html).

## Identifying Benchmark ABI processes

In one terminal window, run the Benchmark ABI version of the `bash` shell with

```
/usr/local64cb/bin/bash
```
and execute in the bash shell session
```
echo $$
```
to print the process's PID.
Note that PID down.

In a second terminal window, run
```
procstat -a
```
to list the ABIs for all running processes.

What is shown for your Benchmark ABI `bash` process with the PID you noted down,
and how does this differ from other processes you see?

## Disassembling Benchmark ABI binaries

Use `objdump` to disassemble the `main()` functions in both binaries:

```
objdump -dj .text ./helloworld-cheriabi
```
```
objdump -dj .text ./helloworld-benchmark
```

What differences exist between the two functions, and why?

## Debugging Benchmark ABI binaries

Use GDB to analyse CPU register contents just before and after returning from
the `main()` functions in both binaries.

First, run `gdb`:

```
gdb ./helloworld-cheriabi
```

Once you enter a GDB session,

1. Disassemble the `main()` function:
   ```
   disassemble main
   ```

1. Record the offset of the `RET` instruction at the end of disassembly, e.g.
   `56` in
   ```
   0x00000000001107ec <+56>:    ret     c30
   ```

1. Set a breakpoint for the `RET` instruction:
   ```
   break *main + N
   ```
   where `N` is the offset you recorded, e.g.
   ```
   break *main + 56
   ```

1. Run the program:
   ```
   run
   ```

1. Disassemble the current function to make sure the process was suspended at
   the `RET` instruction:
   ```
   disassemble
   ```

1. Display the `C30` and `PCC` registers:
   ```
   info register c30 pcc
   ```

1. Execute the `RET` instruction:
   ```
   stepi
   ```

1. Display the `PCC` register again:
   ```
   info register pcc
   ```

Repeat the GDB session for the Benchmark ABI binary:
```
gdb ./helloworld-benchmark
```

What differences exist between the two functions, and why?

## Benchmarking with the Benchmark ABI

Compile the source code for two short C programs, `benchmark-atoi.c` and
`benchmark-sha256.c`:

```
cc -Wall -g -o benchmark-atoi-cheriabi benchmark-atoi.c -lmd
```
```
cc -Wall -g -mabi=purecap-benchmark -o benchmark-atoi-benchmarkabi benchmark-atoi.c -lmd
```
```
cc -Wall -g -o benchmark-sha256-cheriabi benchmark-sha256.c -lmd
```
```
cc -Wall -g -mabi=purecap-benchmark -o benchmark-sha256-benchmarkabi benchmark-sha256.c -lmd
```

Using the UNIX `time(1)` command, run each of the resulting binaries:
```
time ./benchmark-atoi-cheriabi
```
```
time ./benchmark-atoi-benchmarkabi
```
```
time ./benchmark-sha256-cheriabi
```
```
time ./benchmark-sha256-benchmarkabi
```

How long does execution take for the CheriABI vs Benchmark ABI compilations of
each program?
Review the source code for the two workloads; why do they perform the way that
they do?

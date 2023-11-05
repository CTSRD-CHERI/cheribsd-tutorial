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
% cd benchmark
```

## Compiling code for the Benchmark ABI

Compile `helloworld.c` as a CheriABI binary:

```
% cc -Wall -g -o helloworld-cheriabi helloworld.c
```

Compile `helloworld.c` as a Benchmark ABI binary:

```
% cc -Wall -g -mabi=purecap-benchmark -o helloworld-benchmark helloworld.c
```

## Identifying Benchmark ABI binaries

Use the `file(1)` command to identify the two binaries:

```
% file helloworld-cheriabi
% file helloworld-benchmark
```

## Running Benchmark ABI binaries

Run both binaries from the command line:

```
% ./helloworld-cheriabi
% ./helloworld-benchmark
```
## The Benchmark ABI package manager

Using `pkg64cb`, which manages a complete set of third-party software packages
compiled for the Benchmark ABI, list the currently installed Benchmark ABI
package.
Now install the Benchmark ABI compilation of `bash`.

## Identifying Benchmark ABI processes

In one terminal window, run the Benchmark ABI version of the `bash` shell:

```
% /usr/local64cb/bin/bash
```

Run `echo $$` to print the process's PID, and note this down.
Then run `procstat -a` to list the ABIs for all running processes.
What is shown for your Benchmark ABI `bash` process, and how does this differ
from other processes you see?

## Disassembling Benchmark ABI binaries

Use GDB to disassemble the `main()` functions in both binaries.
What differences exist between the two functions, and why?

## Benchmarking with the Benchmark ABI

Compile the source code for two short C programs, `benchmark-atoi.c` and
`benchmark-sha256.c`:

```
% cc -Wall -g -o benchmark-atoi-cheriabi benchmark-atoi.c -lmd
% cc -Wall -g -mabi=purecap-benchmark -o benchmark-atoi-benchmarkabi benchmark-atoi.c -lmd
% cc -Wall -g -o benchmark-sha256-cheriabi benchmark-sha256.c -lmd
% cc -Wall -g -mabi=purecap-benchmark -o benchmark-sha256-benchmarkabi benchmark-sha256.c -lmd
```

Using the UNIX `time(1)` command, run each of the resulting binaries: How
long does execution take for the CheriABI vs Benchmark ABI compilations of
each program?
Review the source code for the two workloads; why do they perform the way that
they do?

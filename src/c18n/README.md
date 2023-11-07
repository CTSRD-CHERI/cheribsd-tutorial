# Library compartmentalization

CheriBSD's library compartmentalization feature (c18n) executes each dynamic
library in a compartmentalization-enabled process in its own protection
domain.
The non-default c18n-enabled run-time linker grants libraries only
capabilities to resources (global variables, APIs) declared in their ELF
linkage.

The adversary model for these compartments is one of trusted code, but
untrustworthy execution: A library such as `libpng` or `libjpeg` is trusted
until it begins dynamic execution -- and has potentially been exposed to
maligious data.
With library compartmentalization, an adversary who achieves arbitrary code
execution within the library at run time will be able to reach only the
resources (and further attack surfaces) declared statically through its
linkage.
The programmer must then harden that linkage, and any involved APIs, to make
them suitable for adversarial engagement -- but the foundation of isolation,
controlled access, and controlled domain transition is in place by default.

In addition to a modified run-time linker, modest changes have been made to
the aarch64c calling convention to avoid assumptions such as implicit stack
sharing between callers and callees across library boundaries when passing
variadic argument lists.
This modified ABI is now used by all CheriABI binaries in CheriBSD, and so
off-the-shelf aarch64c binaries and libraries can be used with library
compartmentalization without recompilation to a modified ABI.
More information on library compartmentalization can be found in the c18n(3)
man page:

```
man c18n
```

## Compiling applications for library compartmentalization

To compile a main application to use library compartmentalization, add the
following flags to compilation of the program binary:

```
-Wl,--dynamic-linker=/libexec/ld-elf-c18n.so.1
```

For example, compile our `helloworld.c` example using:

```
cc -Wall -g -o helloworld helloworld.c -Wl,--dynamic-linker=/libexec/ld-elf-c18n.so.1
```

You can confirm whether a binary uses the c18n run-time linker by inspecting
its `INTERP` field using the `readelf -l` command:

```
readelf -l helloworld
```

## Tracing compartment-boundary crossings

The BSD ktrace(1) command is able to trace compartment-boundary crossings.
To enable this feature, set the `LD_C18N_UTRACE_COMPARTMENT` environmental
variable, which will cause the c18n run-time linker to emit records using
the utrace(2) system call.
Run the program under ktrace with the `-tu` argument to capture only those
records (and not a full system-call trace):

```
ktrace -tu ./helloworld
```

The resulting `ktrace.out` file can be viewed using the kdump(1) command:

```
kdump
```

## Exercise

A classic motivation for software compartmentalization is to separate less
trustworthy I/O routines (which are more easily subject to compromise) from
keying material.
We have constructed a simple application consisting of three C files:

 * `passwordcheck.c` contains the global variable 'password' and the function
   `passwordcheck()`.
 * `io.c` contains the API `getpassword()`.
 * `main.c` calls `getpassword()` followed by `passwordcheck()`, and if it
   succeeds, will print the global variable `secret`.

Compile the three C files as a cheriabi binary, `check.cheriabi`:

```
cc -Wall -g -o check.cheriabi main.c io.c passwordcheck.c
```

Run the program and enter the password `password123` to print the secret.

Next, run the program and enter the password `password123456789`, which will
crash due to a capability bounds violation.
While CHERI memory safety is able to catch this specific vulnerability, it is
relatively easy to imagine non-memory-safety vulnerabilities in I/O handling,
which could lead to arbitrary code execution.

To understand the implications of the vulnerability, we can use `objdump` to
see what data will be visible to a compromisdd main program.
Run `objdump --full-contents` to hexdump the full program binary, whose
`.text` and `.code` sections include those available to the program at run
time:

```
objdump --full-contents check.cheriabi
```

To mitigate these vulnerabilities, we will recompile the program and place the
I/O routine in its own library:

```
cc -Wall -shared -g -o libio.so io.c
cc -Wall -g -o check.c18n main.c passwordcheck.c -Wl,--dynamic-linker=/libexec/ld-elf-c18n.so.1 -L. -lio
```

Now run the program:

```
export LD_C18N_LIBRARY_PATH=.
./check.c18n
```

The program will print its PID, which you can then use as an argument to
`chericat` to dump its capability state (replace `PID` with the printed
value):

```
./chericat -d check.c18n.db -p PID
./chericat -d check.c18n.db -c libio.so
```

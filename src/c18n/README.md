# Library compartmentalization

CheriBSD's library compartmentalization feature (c18n) executes each dynamic
library within a compartmentalization-enabled process in its own protection
domain.
The non-default c18n-enabled run-time linker grants libraries capabilities
only to resources (global variables, APIs) declared in their ELF linkage.
Function calls that cross domain boundaries are interposed on by
domain-crossing shims implemented by the run-time linker.

The adversary model for these compartments is one of trusted code but
untrustworthy execution: a library such as `libpng` or `libjpeg` is trusted
until it begins dynamic execution -- and has potentially been exposed to
maligious data.
With library compartmentalization, an adversary who achieves arbitrary code
execution within the library at run time will be able to reach only the
resources (and further attack surfaces) declared statically through its
linkage.
The programmer must then harden that linkage, and any involved APIs, to make
them suitable for adversarial engagement -- but the foundation of isolation,
controlled access, and controlled domain transition is provided by the c18n
implementation.

In addition to a modified run-time linker, modest changes have been made to
the aarch64c calling convention to avoid assumptions such as implicit stack
sharing between callers and callees across library boundaries when passing
variadic argument lists.
This modified ABI is now used by all CheriABI binaries in CheriBSD, and so
off-the-shelf aarch64c binaries and libraries can be used with library
compartmentalization without recompilation to the modified ABI.
More information on library compartmentalization can be found in the
[c18n(3) man page](https://man.cheribsd.org/cgi-bin/man.cgi/dev/c18n):

```
man c18n
```

This activity involves source code in the `temporal` subdirectory:

```
cd ~/tutorial/src/c18n
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
env LD_C18N_UTRACE_COMPARTMENT=1 ktrace -tu ./helloworld
```

The resulting `ktrace.out` file can be viewed using the kdump(1) command:

```
kdump
```

## Exercise

A classic motivation for software compartmentalization is to separate less
trustworthy I/O-processing routines (which are more easily subject to
compromise) from keying material.
We have constructed a simple application consisting of three C files:

 * `passwordcheck.c` contains the global variable 'the_password' and the
   function `passwordcheck()` that checks the offered password against the
   defined password.
 * `io.c` contains the API `readpassword()`, which uses the legacy C API
   `fscanf()`.
 * `main.c` calls `readpassword()` followed by `passwordcheck()`, and if it
   succeeds, will print the global variable `secret`.

Compile the three C files as a CheriABI binary, `check.cheriabi`:

```
cc -Wall -g -o check.cheriabi main.c io.c passwordcheck.c
```

Run the program and enter the password `password123` to print the secret.

Next, run the program and enter the password `password123456789`, which will
crash due to a capability bounds violation.

 * Why does the program crash?
While CHERI memory safety is able to catch this specific vulnerability, it is
relatively easy to imagine non-memory-safety vulnerabilities in I/O handling,
which could lead to arbitrary code execution.

 * What is an example of an I/O data processing vulnerability that CHERI
   memory safety would not mitigate?

To understand the implications of such vulnerability, we can use `objdump` to
see what data will be visible to a compromised main program.
Run `objdump --full-contents` to hexdump the full program binary, whose
`.text` and `.code` sections include those available to the program at run
time:

```
objdump --full-contents check.cheriabi
```

This includes the hard-coded password in the password-checking routine.

To mitigate these vulnerabilities, we will recompile the program and place the
I/O routine in its own library -- and hence its own compartment when using the
c18n run-time linker:

```
cc -Wall -shared -g -o libio.so io.c
```
```
cc -Wall -g -o check.c18n main.c passwordcheck.c -Wl,--dynamic-linker=/libexec/ld-elf-c18n.so.1 -L. -lio
```

Now run the program:

```
env LD_C18N_LIBRARY_PATH=. ./check.c18n
```

The program will print its PID, which you can then use as an argument to
`chericat` to dump its capability state (replace `PID` with the printed
value):

```
chericat -f check.c18n.db -p PID
```
```
chericat -f check.c18n.db -c libio.so
```

* What capabilities can the attacker reach?
* How do they differ from those available to the attacker in the
  uncompartmentalized case?

It is important to understand, however, that simply isolating running code is
almost always insufficient to achieve robust sandboxing.
The competent adversary will now consider further rights and attack surfaces
to explore in search of further vulnerabilities.
While this increased work factor of finding additional vulnerabilities is an
important part of compartmentalization, internal software APIs are rarely well
suited to be security boundaries without performing additional hardening.
With this in mind:

 * Inspecting the source code, ouput from `objdump`, and output from
   `chericat`, assess the robustness of this compartmentalization: How might a
   highly competent adversary try to escape the sandbox?
 * What larger software architectural steps may be required to allow library
   compartmentalization to be used more robustly for this kind of use case?

Library compartmentalization has the potential to significantly improve
software integrity and confidentiality properties in the presence of a strong
adversary.
However, it is also limited by the abstraction being around the current
library operational model.

 * What are the implications of library compartmentalization on availability?

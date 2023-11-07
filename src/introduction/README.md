# Introduction

During the workshop, you should be given an ssh(1) command and a password that
you can use to log in to your dedicated work environment.
If you do not have access to the work environment, please talk to us during the
workshop.

The environment is one of
[FreeBSD jails](https://docs.freebsd.org/en/books/handbook/jails/)
on a Morello box running a recent
[CheriBSD dev branch snapshot](https://download.cheribsd.org/snapshots/dev/arm64/aarch64c/2023-11-01/)
that includes features that will be available in CheriBSD 23.11.
The running kernel is the default kernel shipped with CheriBSD releases -
the hybrid `GENERIC-MORELLO` kernel with `INVARIANTS` and `WITNESS` features
enabled.

There are several pre-installed packages in your jail to save you time with
preparing your environment:

* CheriABI `git`, `nano`, `tmux` and `chericat`

* Hybrid ABI `llvm-base`, `gdb-cheri` and `vim`

Once you are logged in, you can switch to the `root` user with `sudo su -l` and
install any package you want with `pkg64c`, `pkg64` or `pkg64cb` package
managers, as explained in the
[Third-party packages](https://www.cheribsd.org/getting-started/23.11/packages/)
section of the Getting Started with CheriBSD guide.
You can also install your SSH key in `~/.ssh/authorized_keys` not to be required
to type in a user password to connect.

The source code of this document with example exercises are stored in the
`~/tutorial` directory of your jail.

You can use your jail after the workshop to experiment with CheriBSD in your
free time. We will remove your access after one week, on November 15.

Log in to your jail now and follow next chapters describing new features in
CheriBSD 23.11:

 * [The Benchmark ABI](../benchmark/)
 * [Heap temporal memory safety](../temporal/)
 * [Library compartmentalization](../c18n/)

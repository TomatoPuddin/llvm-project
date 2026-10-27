# Fork Overview

This fork is based on [LLVM 23.1.4](https://github.com/llvm/llvm-project/releases/tag/llvmorg-23.1.4).

> [!CAUTION]
> **MICROSOFT ABI CHANGED. REBUILD REQUIRED.**

The `23.1.4` branch contains these changes:

- **⚠️Microsoft ABI**: nested empty-subobject layouts, `msvc::no_unique_address`, and `empty_bases` inheritance.
- Windows DLL builds: API exports, PCH reuse, vtable emission, and shared LLVM linkage for `llvm-exegesis`.
- Windows toolchains: MSVC sysroot support from [PR #96417](https://github.com/llvm/llvm-project/pull/96417), with legacy `msstl` compatibility.
- Windows runtimes: libc++ new-handler support without MSVC STL linkage, CRT-dependent header visibility, compiler-rt header and TLS fixes, and PDB installation.
- Mach-O split LTO: weak alias linkage and flags from [PR #219534](https://github.com/llvm/llvm-project/pull/219534), with ARM64 and x86_64 regression tests.
- Runtime optimization: optional whole-program vtables for runtime LTO sources, with consistent C and C++ split-unit settings.

- Runtime builds: resolve compiler-rt builtins from in-tree targets before searching the installed toolchain.
- ARM builtins: avoid duplicate reverse-subtraction symbols ([PR #230739](https://github.com/llvm/llvm-project/pull/230739)).
- libc++ CFI: suppress checks for a valid cast in `assume_aligned` ([PR #220296](https://github.com/llvm/llvm-project/pull/220296)).
- libc++ shared arrays: detect allocation-size overflow in `make_shared<T[]>` and `allocate_shared<T[]>` ([PR #223735](https://github.com/llvm/llvm-project/pull/223735)).

The runtime backports come from the
[Android toolchain patch set](https://android.googlesource.com/toolchain/llvm_android/+/a309e8d8406000c78e46520a104c4cf5a098d223/patches/PATCHES.json).
The `[ndk]` commit prefix records the patch source.

To build the Android runtimes, use the [`23.1.4-android` branch](https://github.com/TomatoPuddin/llvm-project/tree/23.1.4-android).


---

# The LLVM Compiler Infrastructure

[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/llvm/llvm-project/badge)](https://securityscorecards.dev/viewer/?uri=github.com/llvm/llvm-project)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/8273/badge)](https://www.bestpractices.dev/projects/8273)
[![libc++](https://github.com/llvm/llvm-project/actions/workflows/libcxx-build-and-test.yaml/badge.svg?branch=main&event=schedule)](https://github.com/llvm/llvm-project/actions/workflows/libcxx-build-and-test.yaml?query=event%3Aschedule)

Welcome to the LLVM project!

This repository contains the source code for LLVM, a toolkit for the
construction of highly optimized compilers, optimizers, and run-time
environments.

The LLVM project has multiple components. The core of the project is
itself called "LLVM". This contains all of the tools, libraries, and header
files needed to process intermediate representations and convert them into
object files. Tools include an assembler, disassembler, bitcode analyzer, and
bitcode optimizer.

C-like languages use the [Clang](https://clang.llvm.org/) frontend. This
component compiles C, C++, Objective-C, and Objective-C++ code into LLVM bitcode
-- and from there into object files, using LLVM.

Other components include:
the [libc++ C++ standard library](https://libcxx.llvm.org),
the [LLD linker](https://lld.llvm.org), and more.

## Getting the Source Code and Building LLVM

Consult the
[Getting Started with LLVM](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm)
page for information on building and running LLVM.

For information on how to contribute to the LLVM project, please take a look at
the [Contributing to LLVM](https://llvm.org/docs/Contributing.html) guide.

## Getting in touch

Join the [LLVM Discourse forums](https://discourse.llvm.org/), [Discord
chat](https://discord.gg/xS7Z362),
[LLVM Office Hours](https://llvm.org/docs/GettingInvolved.html#office-hours) or
[Regular sync-ups](https://llvm.org/docs/GettingInvolved.html#online-sync-ups).

The LLVM project has adopted a [code of conduct](https://llvm.org/docs/CodeOfConduct.html) for
participants to all modes of communication within the project.

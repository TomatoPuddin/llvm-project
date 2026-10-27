; REQUIRES: x86, aarch64
; RUN: rm -rf %t; split-file %s %t
; RUN: llc -mtriple=x86_64-apple-macosx11.0.0 -filetype=obj %t/weak.ll -o %t/weak-x86.o
; RUN: llc -mtriple=arm64-apple-macosx11.0.0 -filetype=obj %t/weak.ll -o %t/weak-arm.o
; RUN: llc -mtriple=x86_64-apple-macosx11.0.0 -filetype=obj %t/strong.ll -o %t/strong-x86.o
; RUN: llc -mtriple=arm64-apple-macosx11.0.0 -filetype=obj %t/strong.ll -o %t/strong-arm.o
; RUN: cp %t/weak-x86.o %t/weak-x86-copy.o
; RUN: cp %t/weak-arm.o %t/weak-arm-copy.o
; RUN: %lld -dylib %t/weak-x86.o %t/weak-x86-copy.o -o %t/coalesced-x86
; RUN: llvm-nm -m %t/coalesced-x86 | FileCheck %s --check-prefix=WEAK
; RUN: %lld -arch arm64 -dylib %t/weak-arm.o %t/weak-arm-copy.o -o %t/coalesced-arm
; RUN: llvm-nm -m %t/coalesced-arm | FileCheck %s --check-prefix=WEAK
; RUN: %lld -dylib %t/weak-x86.o %t/strong-x86.o -o %t/override-x86
; RUN: llvm-nm -m %t/override-x86 | FileCheck %s --check-prefix=STRONG
; RUN: %lld -arch arm64 -dylib %t/strong-arm.o %t/weak-arm.o -o %t/override-arm
; RUN: llvm-nm -m %t/override-arm | FileCheck %s --check-prefix=STRONG

; Weak aliases must coalesce and allow a strong definition to override them.
; A private aliasee must not make its alias local or strong.
; WEAK: weak external _weak_alias
; STRONG: {{^[0-9a-f]+ .*\) external _weak_alias$}}

;--- weak.ll
@storage = private global i32 42
@weak_alias = weak_odr alias i32, ptr @storage

;--- strong.ll
@weak_alias = global i32 17

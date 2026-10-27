; REQUIRES: x86, aarch64
; RUN: rm -rf %t; mkdir -p %t
; RUN: opt -mtriple=x86_64-apple-macosx11.0.0 --thinlto-bc --thinlto-split-lto-unit %s -o %t/x86.o
; RUN: opt -mtriple=arm64-apple-macosx11.0.0 --thinlto-bc --thinlto-split-lto-unit %s -o %t/arm.o
; RUN: llvm-modextract -n 1 %t/x86.o -o %t/regular.bc
; RUN: llvm-dis %t/regular.bc -o - | FileCheck %s --check-prefix=SPLIT
; RUN: %lld -dylib %t/x86.o -o %t/x86.dylib
; RUN: llvm-nm -m %t/x86.dylib | FileCheck %s
; RUN: %lld -arch arm64 -dylib -dead_strip %t/arm.o -o %t/arm.dylib
; RUN: llvm-nm -m %t/arm.dylib | FileCheck %s

; LowerTypeTests replaces the regular LTO partition's vtable with an alias to
; private storage. The alias must remain a weak definition that the ThinLTO
; partition can reference, including when dead stripping is enabled.
; SPLIT: @vtable = weak_odr
; CHECK: external _check
; CHECK: external _get_vtable
; CHECK: weak external _vtable

@vtable = weak_odr constant [1 x ptr] [ptr @method], !type !0

define i32 @method() {
  ret i32 42
}

define ptr @get_vtable() {
  ret ptr @vtable
}

define i1 @check(ptr %p) {
  %ok = call i1 @llvm.type.test(ptr %p, metadata !"typeid")
  ret i1 %ok
}

declare i1 @llvm.type.test(ptr, metadata)
!0 = !{i64 0, !"typeid"}

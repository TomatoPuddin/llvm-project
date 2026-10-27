; RUN: llc -mtriple=aarch64-apple-macosx13.0.0 %s -o - | FileCheck %s
; RUN: llc -mtriple=x86_64-apple-macosx13.0.0 %s -o - | FileCheck %s
; REQUIRES: x86-registered-target

@foo = internal global i32 0
@external_alias = alias i32, ptr @foo
@internal_alias = internal alias i32, ptr @foo
@weak_alias = weak alias i32, ptr @foo
@hidden_weak_alias = weak hidden alias i32, ptr @foo
@linkonce_alias = linkonce alias i32, ptr @foo
@auto_hide_alias = linkonce_odr unnamed_addr alias i32, ptr @foo

; CHECK:      .globl _external_alias
; CHECK-NEXT: _external_alias = _foo
; CHECK:      _internal_alias = _foo
; CHECK:      .globl _weak_alias
; CHECK-NEXT: .weak_definition _weak_alias
; CHECK-NEXT: _weak_alias = _foo
; CHECK:      .globl _hidden_weak_alias
; CHECK-NEXT: .weak_definition _hidden_weak_alias
; CHECK-NEXT: .private_extern _hidden_weak_alias
; CHECK-NEXT: _hidden_weak_alias = _foo
; CHECK:      .globl _linkonce_alias
; CHECK-NEXT: .weak_definition _linkonce_alias
; CHECK-NEXT: _linkonce_alias = _foo
; CHECK:      .globl _auto_hide_alias
; CHECK-NEXT: .weak_def_can_be_hidden _auto_hide_alias
; CHECK-NEXT: _auto_hide_alias = _foo

@storage = private global [2 x i32] [i32 42, i32 43]
@private_storage_alias = weak_odr alias i32, ptr @storage
@offset_alias = weak_odr alias i32, getelementptr ([2 x i32], ptr @storage, i64 0, i64 1)
@chain_alias = weak_odr alias i32, ptr @private_storage_alias

; CHECK: .globl _private_storage_alias
; CHECK-NEXT: .weak_definition _private_storage_alias
; CHECK-NEXT: _private_storage_alias = l_storage
; CHECK: .globl _offset_alias
; CHECK-NEXT: .weak_definition _offset_alias
; CHECK-NEXT: .alt_entry _offset_alias
; CHECK-NEXT: _offset_alias = l_storage+4
; CHECK: .globl _chain_alias
; CHECK-NEXT: .weak_definition _chain_alias
; CHECK-NEXT: _chain_alias = _private_storage_alias

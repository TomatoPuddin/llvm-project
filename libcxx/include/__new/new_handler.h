//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___NEW_NEW_HANDLER_H
#define _LIBCPP___NEW_NEW_HANDLER_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

#if defined(_LIBCPP_ABI_VCRUNTIME)
#  include <new.h>
#endif
_LIBCPP_BEGIN_UNVERSIONED_NAMESPACE_STD
_LIBCPP_BEGIN_EXPLICIT_ABI_ANNOTATIONS

typedef void (*new_handler)();
// UCRT's declaration has no DLL annotation; libc++ provides this symbol.
_LIBCPP_DIAGNOSTIC_PUSH
_LIBCPP_CLANG_DIAGNOSTIC_IGNORED("-Wdll-attribute-on-redeclaration")
_LIBCPP_EXPORTED_FROM_ABI new_handler set_new_handler(new_handler) _NOEXCEPT;
_LIBCPP_DIAGNOSTIC_POP
_LIBCPP_EXPORTED_FROM_ABI new_handler get_new_handler() _NOEXCEPT;

_LIBCPP_END_EXPLICIT_ABI_ANNOTATIONS
_LIBCPP_END_UNVERSIONED_NAMESPACE_STD

#endif // _LIBCPP___NEW_NEW_HANDLER_H

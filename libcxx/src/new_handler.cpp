//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <new>

#include "include/atomic_support.h"

#if defined(_LIBCPP_ABI_VCRUNTIME) && _LIBCPP_HAS_THREADS
#  include <__thread/support.h>
#endif

#if defined(_LIBCPP_ABI_MICROSOFT)
#  define _LIBPCPP_DEFINE_NEW_HANDLER
#elif defined(LIBCXX_BUILDING_LIBCXXABI)
// nothing to do, we use the one from libc++abi
#elif defined(LIBCXXRT)
#  define _LIBPCPP_DEFINE_NEW_HANDLER
#elif defined(__GLIBCXX__)
// nothing to do, we use the one from libstdc++/libsupc++
#else
#  define _LIBPCPP_DEFINE_NEW_HANDLER
#endif

#if defined(_LIBPCPP_DEFINE_NEW_HANDLER)

namespace std { // purposefully not versioned

static constinit std::new_handler __new_handler = nullptr;

#if defined(_LIBCPP_ABI_VCRUNTIME)
// UCRT allocation retries use an int(size_t) callback instead of void().
static int __cdecl __new_handler_adapter(size_t) {
  if (new_handler handler = get_new_handler()) {
    handler();
    return 1;
  }
  return 0;
}

#  if _LIBCPP_HAS_THREADS
static constinit __libcpp_mutex_t __new_handler_mutex = _LIBCPP_MUTEX_INITIALIZER;
#  endif

new_handler set_new_handler(new_handler handler) noexcept {
  // Serialize both updates so concurrent setters cannot leave UCRT disabled
  // while the C++ handler is non-null. Never hold this lock while invoking it.
#  if _LIBCPP_HAS_THREADS
  __libcpp_mutex_lock(&__new_handler_mutex);
#  endif
  new_handler previous = __libcpp_atomic_exchange(&__new_handler, handler);
  _set_new_handler(handler ? __new_handler_adapter : nullptr);
#  if _LIBCPP_HAS_THREADS
  __libcpp_mutex_unlock(&__new_handler_mutex);
#  endif
  return previous;
}
#else
new_handler set_new_handler(new_handler handler) noexcept { return __libcpp_atomic_exchange(&__new_handler, handler); }
#endif

new_handler get_new_handler() noexcept { return __libcpp_atomic_load(&__new_handler); }

} // namespace std

#endif // _LIBPCPP_DEFINE_NEW_HANDLER

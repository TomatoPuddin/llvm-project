//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: no-exceptions
// UNSUPPORTED: sanitizer-new-delete
// UNSUPPORTED: c++03

// A registered handler must participate in the allocator's failure path.

#include <cassert>
#include <cstddef>
#include <limits>
#include <new>

static int calls;
struct allocation_failure {};

static void throw_on_retry() {
  if (++calls == 2)
    throw allocation_failure();
}

static void disable_handler() {
  ++calls;
  assert(std::set_new_handler(0) == disable_handler);
}

int main(int, char**) {
  // Keep the impossible request opaque to allocation optimizations.
  volatile std::size_t size = std::numeric_limits<std::size_t>::max();
  std::new_handler original = std::set_new_handler(throw_on_retry);
  try {
    ::operator delete(::operator new(size));
    assert(false);
  } catch (const allocation_failure&) {
    assert(calls == 2);
  }

  calls = 0;
  assert(::operator new(size, std::nothrow) == 0);
  assert(calls == 2);

  calls = 0;
  std::set_new_handler(disable_handler);
  try {
    ::operator delete(::operator new(size));
    assert(false);
  } catch (const std::bad_alloc&) {
    assert(calls == 1);
    assert(std::get_new_handler() == 0);
  }

  std::set_new_handler(original);
  return 0;
}

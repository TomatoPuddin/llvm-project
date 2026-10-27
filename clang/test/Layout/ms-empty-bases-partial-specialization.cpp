// RUN: %clang_cc1 -std=c++20 -fms-extensions -fsyntax-only -triple x86_64-windows-msvc -verify %s
// RUN: %clang_cc1 -std=c++20 -fms-extensions -fsyntax-only -triple i686-windows-msvc -verify %s
// expected-no-diagnostics

struct Empty {};
struct OtherEmpty {};
template <class T> struct Derived : T { char value; };

template <class...> struct __declspec(empty_bases) Accessor {};
template <class A, class B> struct Accessor<A, B> : A, B {};
static_assert(sizeof(Derived<Accessor<Empty, OtherEmpty>>) == 1);

// A full specialization has an independent layout policy.
template <> struct Accessor<int> : Empty, OtherEmpty {};
static_assert(sizeof(Derived<Accessor<int>>) == 2);

template <class...> struct Unannotated {};
template <class A, class B> struct Unannotated<A, B> : A, B {};
static_assert(sizeof(Derived<Unannotated<Empty, OtherEmpty>>) == 2);

template <class...> struct __declspec(empty_bases) ForwardDeclared;
template <class A, class B> struct ForwardDeclared<A, B>;
template <class A, class B> struct ForwardDeclared<A, B> : A, B {};
static_assert(sizeof(Derived<ForwardDeclared<Empty, OtherEmpty>>) == 1);

template <class T> struct Outer {
  template <class...> struct __declspec(empty_bases) Member {};
  template <class A, class B> struct Member<A, B> : A, B {};
};
static_assert(sizeof(Derived<Outer<int>::Member<Empty, OtherEmpty>>) == 1);

// Proxy stores a composite accessor in a no_unique_address wrapper base.
template <class... Bases>
struct __declspec(empty_bases) Composite : Bases... {};
struct Empty2 {};
struct Empty3 {};
using CompositeAccessor =
    Composite<Accessor<Empty, OtherEmpty>, Accessor<Empty2, Empty3>>;
struct Wrapper {
  [[msvc::no_unique_address]] CompositeAccessor value;
};
struct Proxy : Wrapper { void *storage[3]; };
static_assert(sizeof(Proxy) == 3 * sizeof(void *));

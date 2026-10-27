// RUN: %clang_cc1 -std=c++20 -fms-extensions -Wno-invalid-offsetof -fsyntax-only -triple x86_64-windows-msvc -verify %s
// RUN: %clang_cc1 -std=c++20 -fms-extensions -Wno-invalid-offsetof -fsyntax-only -triple i686-windows-msvc -verify %s
// RUN: %clang_cc1 -std=c++20 -fms-extensions -Wno-invalid-offsetof -fsyntax-only -triple aarch64-windows-msvc -verify %s
// expected-no-diagnostics

// Match MSVC's distinction between complete objects and zero-sized subobjects.
// In particular, semantic emptiness alone does not imply a zero-sized base.
struct Empty {};
struct OtherEmpty {};
struct Wrapper { [[msvc::no_unique_address]] Empty value; };
struct Nested { [[msvc::no_unique_address]] Wrapper value; };
struct Distinct {
  [[msvc::no_unique_address]] Empty first;
  [[msvc::no_unique_address]] OtherEmpty second;
};
struct NonTrivial {
  NonTrivial();
  ~NonTrivial();
  [[msvc::no_unique_address]] Wrapper value;
};
struct Inherited : Wrapper {};
struct __declspec(empty_bases) Multiple : Wrapper, OtherEmpty {};

template <class T> struct Derived : T { char value; };
template <class T> struct Leading {
  [[msvc::no_unique_address]] T empty;
  int value;
};
template <class T> struct Trailing {
  int value;
  [[msvc::no_unique_address]] T empty;
};

template <class T> constexpr bool checkEmptyWrapper() {
  static_assert(sizeof(T) == 1);
  static_assert(sizeof(Derived<T>) == 1);
  static_assert(sizeof(Leading<T>) == sizeof(int));
  static_assert(sizeof(Trailing<T>) == sizeof(int));
  static_assert(__builtin_offsetof(Derived<T>, value) == 0);
  static_assert(__builtin_offsetof(Leading<T>, value) == 0);
  static_assert(__builtin_offsetof(Trailing<T>, empty) == 0);
  static_assert(__is_empty(T));
  return true;
}

static_assert(checkEmptyWrapper<Wrapper>());
static_assert(checkEmptyWrapper<Nested>());
static_assert(checkEmptyWrapper<Distinct>());
static_assert(checkEmptyWrapper<NonTrivial>());
static_assert(checkEmptyWrapper<Inherited>());
static_assert(checkEmptyWrapper<Multiple>());

struct Repeated {
  [[msvc::no_unique_address]] Wrapper first, second;
};
static_assert(sizeof(Repeated) == 2);
static_assert(__builtin_offsetof(Repeated, second) == 1);
static_assert(sizeof(Derived<Repeated>) == 3);
static_assert(sizeof(Leading<Repeated>) == 8);
static_assert(sizeof(Trailing<Repeated>) == 8);

struct alignas(8) AlignedEmpty {};
struct AlignedMember { [[msvc::no_unique_address]] AlignedEmpty value; };
struct ExplicitlyAlignedMember {
  alignas(8) [[msvc::no_unique_address]] Empty value;
};
struct alignas(8) AlignedWrapper {
  [[msvc::no_unique_address]] Empty value;
};
static_assert(sizeof(Derived<AlignedMember>) == 16);
static_assert(sizeof(Leading<AlignedMember>) == 16);
static_assert(sizeof(Trailing<AlignedMember>) == 16);
static_assert(sizeof(Derived<ExplicitlyAlignedMember>) == 16);
static_assert(sizeof(Derived<AlignedWrapper>) == 8);
static_assert(sizeof(Leading<AlignedWrapper>) == 8);
static_assert(sizeof(Trailing<AlignedWrapper>) == 8);

struct Bitfield {
  int : 0;
  [[msvc::no_unique_address]] Empty value;
};
struct BaseAndMember : Empty {
  [[msvc::no_unique_address]] OtherEmpty value;
};
static_assert(sizeof(Derived<Bitfield>) == 2);
static_assert(sizeof(Leading<Bitfield>) == 8);
static_assert(sizeof(Trailing<Bitfield>) == 8);
static_assert(sizeof(Derived<BaseAndMember>) == 2);
static_assert(sizeof(Leading<BaseAndMember>) == 8);
static_assert(sizeof(Trailing<BaseAndMember>) == 8);

struct Polymorphic {
  virtual void f();
  [[msvc::no_unique_address]] Wrapper empty;
};
static_assert(sizeof(Polymorphic) == 2 * sizeof(void *));

struct SameTypeAsBase : Empty {
  [[msvc::no_unique_address]] Empty empty;
};
struct SameTypeAsBaseMember : Wrapper {
  [[msvc::no_unique_address]] Empty empty;
  char value;
};
static_assert(sizeof(SameTypeAsBase) == 2);
static_assert(__builtin_offsetof(SameTypeAsBase, empty) == 1);
static_assert(sizeof(SameTypeAsBaseMember) == 2);
static_assert(__builtin_offsetof(SameTypeAsBaseMember, empty) == 1);
static_assert(__builtin_offsetof(SameTypeAsBaseMember, value) == 0);

// MSVC retains its legacy overlap for ordinary members, including in records
// that also contain no_unique_address members.
struct OrdinaryMember : Empty {
  Empty empty;
  [[msvc::no_unique_address]] OtherEmpty other;
};
struct OrdinaryMemberOfWrapper : Wrapper { Empty empty; };
static_assert(sizeof(OrdinaryMember) == 1);
static_assert(sizeof(OrdinaryMemberOfWrapper) == 1);
static_assert(__builtin_offsetof(OrdinaryMember, empty) == 0);
static_assert(__builtin_offsetof(OrdinaryMemberOfWrapper, empty) == 0);

#pragma pack(push, 1)
struct Packed {
  [[msvc::no_unique_address]] Nested empty;
  int value;
};
#pragma pack(pop)
static_assert(sizeof(Packed) == 4);
static_assert(alignof(Packed) == 1);
static_assert(__builtin_offsetof(Packed, value) == 0);

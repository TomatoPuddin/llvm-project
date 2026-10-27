// RUN: %clang_cc1 -triple x86_64-windows-msvc -std=c++20 -emit-llvm -O1 -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple i686-windows-msvc -std=c++20 -emit-llvm -O1 -o - %s | FileCheck %s

struct Empty {};
struct Wrapper { [[msvc::no_unique_address]] Empty empty; };
struct Nested { [[msvc::no_unique_address]] Wrapper empty; };
struct Derived : Nested { int value; };
struct Holder {
  int value;
  [[msvc::no_unique_address]] Nested empty;
};

// CHECK-DAG: @derived ={{.*}} global %struct.Derived { i32 42 }
// CHECK-DAG: @holder ={{.*}} global %struct.Holder { i32 43 }
extern "C" {
Derived derived = {{}, 42};
Holder holder = {43, {}};
}

// Copying an empty subobject must not overwrite the scalar at the same address.
// CHECK-LABEL: define {{.*}}i32 @copy_member(
// CHECK: store i32 42, ptr %{{[^,]+}}, align 4
// CHECK-NEXT: ret i32 42
extern "C" int copy_member(Holder *dest, const Holder *src) {
  dest->value = 42;
  dest->empty = src->empty;
  return dest->value;
}

// CHECK-LABEL: define {{.*}}i32 @copy_base(
// CHECK: store i32 42, ptr %{{[^,]+}}, align 4
// CHECK-NEXT: ret i32 42
extern "C" int copy_base(Derived *dest, const Derived *src) {
  dest->value = 42;
  static_cast<Nested &>(*dest) = *src;
  return dest->value;
}

// CHECK-LABEL: define {{.*}}void @copy_holder(
// CHECK: [[VALUE:%.*]] = load i32, ptr %{{[^,]+}}, align 4
// CHECK-NEXT: store i32 [[VALUE]], ptr %{{[^,]+}}, align 4
// CHECK-NEXT: ret void
extern "C" void copy_holder(Holder *dest, const Holder *src) {
  *dest = *src;
}

struct NonTrivial {
  NonTrivial();
  NonTrivial(const NonTrivial &);
  ~NonTrivial();
};
struct NonTrivialWrapper { [[msvc::no_unique_address]] NonTrivial empty; };
struct WithSideEffects {
  int value;
  [[msvc::no_unique_address]] NonTrivialWrapper empty;
};
static_assert(sizeof(WithSideEffects) == sizeof(int));

void *operator new(decltype(sizeof(0)), void *p) noexcept { return p; }

// Overlap must preserve constructor and destructor side effects.
// CHECK-LABEL: define {{.*}}void @construct(
// CHECK: store i32 42, ptr [[OBJECT:%[^,]+]], align 4
// CHECK: call {{.*}}ptr @"??0NonTrivial@@{{.*}}(ptr {{.*}}[[OBJECT]])
// CHECK: ret void
extern "C" void construct(WithSideEffects *p) {
  ::new (p) WithSideEffects{42, {}};
}

// CHECK-LABEL: define {{.*}}void @destroy(
// CHECK-NOT: getelementptr
// CHECK: call {{.*}}void @"??1NonTrivial@@
// CHECK: ret void
extern "C" void destroy(WithSideEffects *p) { p->~WithSideEffects(); }

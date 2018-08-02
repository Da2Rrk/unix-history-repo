// RUN: %clang_cc1 -fsyntax-only -verify %s -triple spir-unknown-unknown

kernel void half_arg(half x) { } // expected-error{{declaring function parameter of type 'half' is not allowed; did you forget * ?}}

#pragma OPENCL EXTENSION cl_khr_fp16 : enable


// Disallowed: parameters with type
// bool, half, size_t, ptrdiff_t, intptr_t, and uintptr_t
// or a struct / union with any of these types in them

typedef __SIZE_TYPE__ size_t; // expected-note{{'size_t' (aka 'unsigned int') declared here}}
                              // expected-note@-1{{'size_t' (aka 'unsigned int') declared here}}
typedef __PTRDIFF_TYPE__ ptrdiff_t; // expected-note{{'ptrdiff_t' (aka 'int') declared here}}
typedef __INTPTR_TYPE__ intptr_t; // expected-note{{'intptr_t' (aka 'int') declared here}}
typedef __UINTPTR_TYPE__ uintptr_t; // expected-note{{'uintptr_t' (aka 'unsigned int') declared here}}

kernel void size_t_arg(size_t x) {} // expected-error{{'size_t' (aka 'unsigned int') cannot be used as the type of a kernel parameter}}

kernel void ptrdiff_t_arg(ptrdiff_t x) {} // expected-error{{'ptrdiff_t' (aka 'int') cannot be used as the type of a kernel parameter}}

kernel void intptr_t_arg(intptr_t x) {} // expected-error{{'intptr_t' (aka 'int') cannot be used as the type of a kernel parameter}}

kernel void uintptr_t_arg(uintptr_t x) {} // expected-error{{'uintptr_t' (aka 'unsigned int') cannot be used as the type of a kernel parameter}}

typedef size_t size_ty;
struct SizeTStruct { // expected-note{{within field of type 'SizeTStruct' declared here}}
  size_ty s; // expected-note{{field of illegal type 'size_ty' (aka 'unsigned int') declared here}}
};
kernel void size_t_struct_arg(struct SizeTStruct x) {} // expected-error{{'struct SizeTStruct' cannot be used as the type of a kernel parameter}}

union SizeTUnion { // expected-note{{within field of type 'SizeTUnion' declared here}}
  size_t s; // expected-note{{field of illegal type 'size_t' (aka 'unsigned int') declared here}}
  float f;
};
kernel void size_t_union_arg(union SizeTUnion x) {} // expected-error{{'union SizeTUnion' cannot be used as the type of a kernel parameter}}

typedef size_t s_ty; // expected-note{{'s_ty' (aka 'unsigned int') declared here}}
typedef s_ty ss_ty; // expected-note{{'ss_ty' (aka 'unsigned int') declared here}}
kernel void typedef_to_size_t(ss_ty s) {} // expected-error{{'ss_ty' (aka 'unsigned int') cannot be used as the type of a kernel parameter}}

kernel void bool_arg(bool x) { } // expected-error{{'bool' cannot be used as the type of a kernel parameter}}

// half kernel argument is allowed when cl_khr_fp16 is enabled.
kernel void half_arg(half x) { }

typedef struct ContainsBool // expected-note{{within field of type 'ContainsBool' declared here}}
{
  bool x; // expected-note{{field of illegal type 'bool' declared here}}
} ContainsBool;

kernel void bool_in_struct_arg(ContainsBool x) { } // expected-error{{'ContainsBool' (aka 'struct ContainsBool') cannot be used as the type of a kernel parameter}}



typedef struct FooImage2D // expected-note{{within field of type 'FooImage2D' declared here}}
{
  // TODO: Clean up needed - we don't really need to check for image, event, etc
  // as a note here any longer.
  // They are diagnosed as an error for all struct fields (OpenCL v1.2 s6.9b,r).
  image2d_t imageField; // expected-note{{field of illegal type '__read_only image2d_t' declared here}} expected-error{{the '__read_only image2d_t' type cannot be used to declare a structure or union field}}
} FooImage2D;

kernel void image_in_struct_arg(FooImage2D arg) { } // expected-error{{struct kernel parameters may not contain pointers}}

typedef struct Foo // expected-note{{within field of type 'Foo' declared here}}
{
  int* ptrField; // expected-note{{field of illegal pointer type 'int *' declared here}}
} Foo;

kernel void pointer_in_struct_arg(Foo arg) { } // expected-error{{struct kernel parameters may not contain pointers}}

typedef union FooUnion // expected-note{{within field of type 'FooUnion' declared here}}
{
  int* ptrField; // expected-note{{field of illegal pointer type 'int *' declared here}}
} FooUnion;

kernel void pointer_in_union_arg(FooUnion arg) { }// expected-error{{union kernel parameters may not contain pointers}}

typedef struct NestedPointer // expected-note 2 {{within field of type 'NestedPointer' declared here}}
{
  int x;
  struct InnerNestedPointer
  {
    int* ptrField; // expected-note 3 {{field of illegal pointer type 'int *' declared here}}
  } inner; // expected-note 3 {{within field of type 'struct InnerNestedPointer' declared here}}
} NestedPointer;

kernel void pointer_in_nested_struct_arg(NestedPointer arg) { }// expected-error{{struct kernel parameters may not contain pointers}}

struct NestedPointerComplex // expected-note{{within field of type 'NestedPointerComplex' declared here}}
{
  int foo;
  float bar;

  struct InnerNestedPointerComplex
  {
    int innerFoo;
    int* innerPtrField; // expected-note{{field of illegal pointer type 'int *' declared here}}
  } inner; // expected-note{{within field of type 'struct InnerNestedPointerComplex' declared here}}

  float y;
  float z[4];
};

kernel void pointer_in_nested_struct_arg_complex(struct NestedPointerComplex arg) { }// expected-error{{struct kernel parameters may not contain pointers}}

typedef struct NestedBool // expected-note 2 {{within field of type 'NestedBool' declared here}}
{
  int x;
  struct InnerNestedBool
  {
    bool boolField; // expected-note 2 {{field of illegal type 'bool' declared here}}
  } inner; // expected-note 2 {{within field of type 'struct InnerNestedBool' declared here}}
} NestedBool;

kernel void bool_in_nested_struct_arg(NestedBool arg) { } // expected-error{{'NestedBool' (aka 'struct NestedBool') cannot be used as the type of a kernel parameter}}

// Warning emitted again for argument used in other kernel
kernel void bool_in_nested_struct_arg_again(NestedBool arg) { } // expected-error{{'NestedBool' (aka 'struct NestedBool') cannot be used as the type of a kernel parameter}}


// Check for note with a struct not defined inside the struct
typedef struct NestedBool2Inner
{
  bool boolField; // expected-note{{field of illegal type 'bool' declared here}}
} NestedBool2Inner;

typedef struct NestedBool2 // expected-note{{within field of type 'NestedBool2' declared here}}
{
  int x;
  NestedBool2Inner inner; // expected-note{{within field of type 'NestedBool2Inner' (aka 'struct NestedBool2Inner') declared here}}
} NestedBool2;

kernel void bool_in_nested_struct_2_arg(NestedBool2 arg) { } // expected-error{{'NestedBool2' (aka 'struct NestedBool2') cannot be used as the type of a kernel parameter}}


struct InnerInner
{
  int* foo;
  bool x;
};

struct Valid
{
  float c;
  float d;
};

struct Inner
{
  struct Valid v;
  struct InnerInner a;
  struct Valid g;
  struct InnerInner b;
};

struct AlsoUser // expected-note{{within field of type 'AlsoUser' declared here}}
{
  float x;
  struct Valid valid1;
  struct Valid valid2;
  struct NestedPointer aaaa; // expected-note{{within field of type 'struct NestedPointer' declared here}}
};

kernel void pointer_in_nested_struct_arg_2(struct Valid valid, struct NestedPointer arg, struct AlsoUser also) { } // expected-error 2 {{struct kernel parameters may not contain pointers}}

struct ArrayOfPtr // expected-note{{within field of type 'ArrayOfPtr' declared here}}
{
  float *arr[3]; // expected-note{{field of illegal type 'float *[3]' declared here}}
                 // expected-note@-1{{field of illegal type 'float *[3]' declared here}}
};
kernel void array_of_ptr(struct ArrayOfPtr arr) {} // expected-error{{struct kernel parameters may not contain pointers}}

struct ArrayOfStruct // expected-note{{within field of type 'ArrayOfStruct' declared here}}
{
  struct ArrayOfPtr arr[3]; // expected-note{{within field of type 'struct ArrayOfPtr [3]' declared here}}
};
kernel void array_of_struct(struct ArrayOfStruct arr) {} // expected-error{{struct kernel parameters may not contain pointers}}

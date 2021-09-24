
#pragma once


#include <template_system.h>
#include <no_compile_header.h>


//utility macros
#define tuple_set1(T1, T2)(tuple_t(T1, T2) tuple, T1 v) __tuple_set1(T1, T2)(&tuple, v)
#define tuple_set2(T1, T2)(tuple_t(T1, T2) tuple, T2 v) __tuple_set2(T1, T2)(&tuple, v)

//type
#define tuple_t(T1, T2) template2(tuple_t, T1, T2)

//constructor
#define tuple(T1, T2) template2(tuple, T1, T2)

//member functions
#define tuple_get1(T1, T2) template2(tuple_get1, T1, T2)
#define tuple_get2(T1, T2) template2(tuple_get2, T1, T2)
#define __tuple_set1(T1, T2) template2(tuple_set1, T1, T2)
#define __tuple_set2(T1, T2) template2(tuple_set2, T1, T2)


#define instantiate_tuple_t(T1, T2)\
typedef struct tuple_t(T1, T2)\
{\
	T1 value1;\
	T2 value2;\
} tuple_t(T1, T2)\

#define instantiate_declaration_tuple_get1(T1, T2) T1 tuple_get1(T1, T2)(tuple_t(T1, T2) tuple)
#define instantiate_declaration_tuple_get2(T1, T2) T2 tuple_get2(T1, T2)(tuple_t(T1, T2) tuple)
#define instantiate_declaration_tuple_set1(T1, T2) void __tuple_set1(T1, T2)(tuple_t(T1, T2)* tuple, T1 v)
#define instantiate_declaration_tuple_set2(T1, T2) void __tuple_set2(T1, T2)(tuple_t(T1, T2)* tuple, T2 v)

#define instantiate_implementation_tuple_get1(T1, T2) T1 tuple_get1(T1, T2)(tuple_t(T1, T2) tuple) { return tuple.value1; }
#define instantiate_implementation_tuple_get2(T1, T2) T2 tuple_get2(T1, T2)(tuple_t(T1, T2) tuple) { return tuple.value2; }
#define instantiate_implementation_tuple_set1(T1, T2) void __tuple_set1(T1, T2)(tuple_t(T1, T2)* tuple, T1 v) { tuple->value1 = v; }
#define instantiate_implementation_tuple_set2(T1, T2) void __tuple_set2(T1, T2)(tuple_t(T1, T2)* tuple, T2 v) { tuple->value2 = v; }
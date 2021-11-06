

#pragma once

/*
 SWITCHES:
 MEMORY_ALLOCATION_SAFETY_LEVEL_0
 MEMORY_ALLOCATION_SAFETY_LEVEL_1
 MEMORY_ALLOCATION_SAFETY_LEVEL_2
 */

#if defined(MEMORY_ALLOCATION_SAFETY_LEVEL_0)
#	warning "[MemoryAllocator] Using USE_STDLIB"
#	define USE_STDLIB
#elif defined(MEMORY_ALLOCATION_SAFETY_LEVEL_1)
#	warning "[MemoryAllocator] Using USE_GARBAGE_COLLECTOR"
#	define USE_GARBAGE_COLLECTOR
#elif defined(MEMORY_ALLOCATION_SAFETY_LEVEL_2)
#	warning "[MemoryAllocator] Using USE_SAFE_MEMORY"
#	define USE_SAFE_MEMORY
#else
#	warning "[MemoryAllocator] None of MEMORY_ALLOCATION_SAFETY_LEVEL_0, MEMORY_ALLOCATION_SAFETY_LEVEL_1, or MEMORY_ALLOCATION_SAFETY_LEVEL_2 is defined; using MEMORY_ALLOCATION_SAFETY_LEVEL_2"
#	define USE_SAFE_MEMORY /*MEMORY_ALLOCATION_SAFETY_LEVEL_2*/
#endif

#ifdef USE_SAFE_MEMORY
#	if defined(GLOBAL_DEBUG) && !defined(SAFE_MEMORY_DEBUG)
#		define SAFE_MEMORY_DEBUG
#	elif defined(GLOBAL_RELEASE) && !defined(SAFE_MEMORY_RELEASE)
#		define SAFE_MEMORY_RELEASE
#	endif
#   ifdef IMPLEMENTATION
#   error "IMPLEMENTATION is defined"
#   endif
#	include <safe_memory/safe_memory.h>
#	define memory_allocator_init(x) safe_memory_init()
#	define memory_allocator_terminate() safe_memory_terminate()
#   define add_alloc(basePtr, size) checked(basePtr, size)
#	define heap_alloc(size) checked_malloc(size)
#	define stack_alloc(size) checked_alloca(size)
#	define stack_free(basePtr) checked_free(basePtr)
#	define heap_free(basePtr) checked_free(basePtr)
#	define stack_array(type, count, ...)  safe_array(type)(checked_alloca(sizeof(type) * (count)), (count), __VA_ARGS__)
#	define heap_array(type, count, ...)  safe_array(type)(checked_malloc(sizeof(type) * (count)), (count), __VA_ARGS__)
#	define ref(type, validPtr, index) checked_ref(type, validPtr, index)
#   define instantiate_stack_array(type) instantiate_declaration_safe_array(type); instantiate_implementation_safe_array(type)
#endif

#ifdef USE_GARBAGE_COLLECTOR
#	if defined(GLOBAL_DEBUG) && !defined(GARBAGE_COLLECTOR_DEBUG)
#		define GARBAGE_COLLECTOR_DEBUG
#	elif defined(GLOBAL_RELEASE) && !defined(GARBAGE_COLLECTOR_RELEASE)
#		define GARBAGE_COLLECTOR_RELEASE
#	endif
#	include <garbage_collector/garbage_collector.h>
#	include <stdlib.h>
#	define memory_allocator_init(x) GC_START(x)
#	define memory_allocator_terminate() GC_STOP()
#   define add_alloc(basePtr, size)
#	define heap_alloc(size) GC_ALLOC(size)
#	define stack_alloc(size) alloca(size)
#	define stack_free(basePtr)
#	define heap_free(basePtr) GC_FREE(basePtr)
#	define stack_array(type, count, ...) memory_allocator_array(type)(checked_alloca(sizeof(type) * (count)), (count), __VA_ARGS__)
#	define heap_array(type, count, ...) memory_allocator_array(type)(checked_malloc(sizeof(type) * (count)), (count), __VA_ARGS__)
#	define ref(type, validPtr, index) validPtr[index]
#   define instantiate_stack_array(type)
#endif

#ifdef USE_STDLIB
#	include <stdlib.h>
#	define memory_allocator_init(x)
#	define memory_allocator_terminate()
#   define add_alloc(basePtr, size)
#	define heap_alloc(size) malloc(size)
#	define stack_alloc(size) alloca(size)
#	define stack_free(basePtr) 
#	define heap_free(basePtr) free(basePtr)
#   define stack_array(type, count, ...) memory_allocator_array(type)(checked_alloca(sizeof(type) * (count)), (count), __VA_ARGS__)
#   define heap_array(type, count, ...) memory_allocator_array(type)(checked_malloc(sizeof(type) * (count)), (count), __VA_ARGS__)
#	define ref(type, validPtr, index) validPtr[index]
#   define instantiate_stack_array(type)
#endif

#define stack_new(type) ((type*)stack_alloc(sizeof(type)))
#define stack_newv(type, count) ((type*)heap_alloc(sizeof(type) * count))
#define heap_new(type) ((type*)heap_alloc(sizeof(type)))
#define heap_newv(type, count) ((type*)heap_alloc(sizeof(type) * count))

#include <memory_allocator/template_instantiations.h>
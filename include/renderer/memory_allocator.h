#pragma once

/*
 SWITCHES:
 MEMORY_ALLOCATION_SAFETY_LEVEL_0
 MEMORY_ALLOCATION_SAFETY_LEVEL_1
 MEMORY_ALLOCATION_SAFETY_LEVEL_2
 */

#if defined(MEMORY_ALLOCATION_SAFETY_LEVEL_0)
#	define USE_STDLIB
#elif defined(MEMORY_ALLOCATION_SAFETY_LEVEL_1)
#	define USE_GARBAGE_COLLECTOR
#elif defined(MEMORY_ALLOCATION_SAFETY_LEVEL_2)
#	define USE_SAFE_MEMORY
#else
#	warning "[MemoryAllocator] None of MEMORY_ALLOCATION_SAFETY_LEVEL_0, MEMORY_ALLOCATION_SAFETY_LEVEL_1, or MEMORY_ALLOCATION_SAFETY_LEVEL_2 is defined; using MEMORY_ALLOCATION_SAFETY_LEVEL_2"
#	define USE_SAFE_MEMORY /*MEMORY_ALLOCATION_SAFETY_LEVEL_2*/
#endif

#include <stdarg.h>

#ifdef USE_SAFE_MEMORY
#	if defined(GLOBAL_DEBUG) && !defined(SAFE_MEMORY_DEBUG)
#		define SAFE_MEMORY_DEBUG
#	elif defined(GLOBAL_RELEASE) && !defined(SAFE_MEMORY_RELEASE)
#		define SAFE_MEMORY_RELEASE
#	endif
#	include <safe_memory/safe_memory.h>
#	define memory_allocator_init(x) safe_memory_init()
#	define memory_allocator_terminate() safe_memory_terminate()
#   define add_alloc(basePtr, size) checked(basePtr, size)
#	define heap_alloc(size) checked_malloc(size)
#	define stack_alloc(size) checked_alloca(size)
#	define stack_free(basePtr) checked_free(basePtr)
#	define heap_free(basePtr) checked_free(basePtr)
#	define get_element(type, validPtr, index) checked_ref(type, validPtr, index)
#   define get_element_ptr(type, validPtr, index) checked_refp(type, validPtr, index)
#   define MEM_CHECK(ptr) get_element_ptr(u8, ptr, 0)
#   define var(type, id) type* id = stack_alloc(sizeof(type))
#   define val(id) *id
#   define ptr(id) id
#elif defined(USE_GARBAGE_COLLECTOR)
#	if defined(GLOBAL_DEBUG) && !defined(GARBAGE_COLLECTOR_DEBUG)
#		define GARBAGE_COLLECTOR_DEBUG
#	elif defined(GLOBAL_RELEASE) && !defined(GARBAGE_COLLECTOR_RELEASE)
#		define GARBAGE_COLLECTOR_RELEASE
#	endif
#	include <renderer/garbage_collector.h>
#	include <stdlib.h>
#	define memory_allocator_init(stackBaseAddress) GC_START(stackBaseAddress)
#	define memory_allocator_terminate() GC_STOP()
#   define add_alloc(basePtr, size) basePtr
#	define heap_alloc(size) GC_ALLOC(size)
#	define stack_alloc(size) alloca(size)
#	define stack_free(basePtr)
#	define heap_free(basePtr) GC_FREE(basePtr)
#   define get_element(type, validPtr, index) (validPtr)[index]
#   define get_element_ptr(type, validPtr, index) (&(validPtr)[index])
#   define MEM_CHECK(ptr)
#   define var(type, id) type id
#   define val(id) id
#   define ptr(id) &id
#elif defined(USE_STDLIB)
#	include <stdlib.h>
#	define memory_allocator_init(x)
#	define memory_allocator_terminate()
#   define add_alloc(basePtr, size) basePtr
#	define heap_alloc(size) malloc(size)
#	define stack_alloc(size) alloca(size)
#	define stack_free(basePtr) 
#	define heap_free(basePtr) free(basePtr)
#   define get_element(type, validPtr, index) (validPtr)[index]
#   define get_element_ptr(type, validPtr, index) (&(validPtr)[index])
#   define MEM_CHECK(ptr)
#   define var(type, id) type id
#   define val(id) id
#   define ptr(id) &id
#endif

#define stack_new(type) ((type*)stack_alloc(sizeof(type)))
#define stack_newv(type, count) ((type*)stack_alloc(sizeof(type) * (count)))
#define heap_new(type) ((type*)heap_alloc(sizeof(type)))
#define heap_newv(type, count) ((type*)heap_alloc(sizeof(type) * (count)))

#include <memory.h>

#define memzero(ptr, type) memset(ptr, 0, sizeof(type))
#define memzerov(ptr, type, count) memset(ptr, 0, sizeof(type) * (count))


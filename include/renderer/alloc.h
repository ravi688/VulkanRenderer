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
#	warning "[alloc.h] None of MEMORY_ALLOCATION_SAFETY_LEVEL_0, MEMORY_ALLOCATION_SAFETY_LEVEL_1, or MEMORY_ALLOCATION_SAFETY_LEVEL_2 is defined; using MEMORY_ALLOCATION_SAFETY_LEVEL_2"
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
#	define alloc_init(x) safe_memory_init()
#	define alloc_terminate() safe_memory_terminate()
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
#	define alloc_init(stackBaseAddress) GC_START(stackBaseAddress)
#	define alloc_terminate() GC_STOP()
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
#	define alloc_init(x)
#	define alloc_terminate()
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

#include <renderer/defines.h>
#include <renderer/debug.h>

/* NOTE: DESCRIPTION is already defined in common/assert.h but we can't include common/assert.h as it leads to conflicts with renderer/assert.h */
#ifndef DESCRIPTION
#   define DESCRIPTION(bool_value) (CAST_TO(u64, (bool_value)) | (1ULL << 16))
#endif

/* sets the memory zero; and checks if the 'actual_size' is equal to the 'requested_size' */
static INLINE_IF_DEBUG_MODE void* __memzero(void* const ptr, const u32 actual_size, const u32 requested_size, const u32 line, const char* function, const char* file)
{
    #ifdef GLOBAL_DEBUG
    if(actual_size > requested_size)
        debug_assert(line, function, file, DESCRIPTION(false), 
            "actual size %u is not equal to the size of the type you passed which is %u", actual_size, requested_size);
    else if(actual_size < requested_size)
        debug_log("[Performance Warning] ", line, function, file, "actual size of the object is %u but you allocated more than that which is %u", actual_size, requested_size);
    #endif

    return memset(ptr, 0, actual_size);
}

/* sets the memory zero; and checks if the size of the type of the pointer is equal to the size of the type */
#define memzero(ptr, type) __memzero(ptr, sizeof(*(ptr)), sizeof(type), __LINE__, __FUNCTION__, __FILE__)
/* vector version of memzero */
#define memzerov(ptr, type, count) __memzero(ptr, sizeof(*(ptr)) * (count), sizeof(type) * (count), __LINE__, __FUNCTION__, __FILE__)

/* sets the memory zero; but it doesn't check if the size of the type of the pointer is equal to the size of the type */
#define unsafe_memzero(ptr, type) memset(ptr, 0, sizeof(type))
/* vector version of memzero */
#define unsafe_memzerov(ptr, type, count) memset(ptr, 0, sizeof(type) * (count))

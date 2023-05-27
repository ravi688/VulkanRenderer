/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: alloc.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/

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
#   define heap_realloc(old_ptr, size) checked_realloc(old_ptr, size)
#   define heap_aligned_alloc(size, align) checked_aligned_malloc(size, align)
#   define heap_aligned_realloc(old_ptr, size, align) checked_aligned_realloc(old_ptr, size, align)
#	define stack_alloc(size) checked_alloca(size)
#	define stack_free(basePtr) checked_free(basePtr)
#	define heap_free(basePtr) checked_free(basePtr)
#   define heap_aligned_free(basePtr) checked_aligned_free(basePtr)
#   define heap_silent_free(basePtr) checked_silent_free(basePtr)
#   define heap_silent_aligned_free(basePtr) checked_silent_free(basePtr)
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
#   define heap_realloc(old_ptr, size)
#   define heap_aligned_alloc(size, align)
#   define heap_aligned_realloc(old_ptr, size, align)
#	define stack_alloc(size) alloca(size)
#	define stack_free(basePtr)
#	define heap_free(basePtr) GC_FREE(basePtr)
#   define heap_aligned_free(basePtr)
#   define heap_silent_free(basePtr)
#   define heap_slient_aligned_free(basePtr)
#   define get_element(type, validPtr, index) (validPtr)[index]
#   define get_element_ptr(type, validPtr, index) (&(validPtr)[index])
#   define MEM_CHECK(ptr)
#   define var(type, id) type id
#   define val(id) id
#   define ptr(id) &id
#elif defined(USE_STDLIB)
#	include <stdlib.h>

static INLINE_IF_RELEASE_MODE void* _debug_malloc(size_t size) { return malloc(size); }
static INLINE_IF_RELEASE_MODE void* _debug_realloc(void* old_ptr, size_t size) { return realloc(old_ptr, size); }
static INLINE_IF_RELEASE_MODE void _debug_free(void* ptr){ if(ptr == NULL) return; free(ptr); }
static INLINE_IF_RELEASE_MODE void* _debug_aligned_malloc(size_t size, size_t align) { _debug_assert__(align != 0); return _aligned_malloc(size, align); }
static INLINE_IF_RELEASE_MODE void* _debug_aligned_realloc(void* old_ptr, size_t size, size_t align) { _debug_assert__(align != 0); return _aligned_realloc(old_ptr, size, align); }
static INLINE_IF_RELEASE_MODE void _debug_aligned_free(void* ptr) { if(ptr == NULL) return; _aligned_free(ptr); }

#	define alloc_init(x)
#	define alloc_terminate()
#   define add_alloc(basePtr, size) basePtr
#	define heap_alloc(size) _debug_malloc(size)
#   define heap_realloc(old_ptr, size) _debug_realloc(old_ptr, size)
#   define heap_aligned_alloc(size, align) _debug_aligned_malloc(size, align)
#   define heap_aligned_realloc(old_ptr, size, align) _debug_aligned_realloc(old_ptr, size, align)
#	define stack_alloc(size) alloca(size)
#	define stack_free(basePtr) 
#	define heap_free(basePtr) _debug_free(basePtr)
#   define heap_aligned_free(basePtr) _debug_aligned_free(basePtr)
#   define heap_silent_free(basePtr) _debug_free(basePtr)
#   define heap_silent_aligned_free(basePtr) _debug_aligned_free(basePtr)
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

static INLINE_IF_RELEASE_MODE void* _debug_memset(void* ptr, int value, size_t size)  { _debug_assert__(ptr != NULL); return memset(ptr, value, size); }

#define _debug_memcpy(dst, src, size) __debug_memcpy((void*)(dst), (void*)(src), size)
static INLINE_IF_RELEASE_MODE void __debug_memcpy(void* dst, void* src, size_t size) { _debug_assert__((size == 0) || ((dst != NULL) && (src != NULL))); memcpy(dst, src, size); }

static INLINE_IF_RELEASE_MODE void __memcopy(void* const dst, const void* const src, u32 dst_size, u32 src_size, u32 requested_size, u32 count, const u32 line, const char* const function, const char* const file)
{
    #ifdef GLOBAL_DEBUG
    if((src_size < requested_size) || (dst_size < requested_size))
        debug_assert(line, function, file, DESCRIPTION(false), 
            "memcopy failed! either src element size or dst element size is less than the requested element size");
    else if((src_size > requested_size) && (dst_size > requested_size))
        debug_log("[Performance Warning] ", line, function, file, 
            "src element size and dst element size both are larger than the requested size");
    #endif
    _debug_memcpy(dst, src, requested_size * count);
}

#define memcopy(dst, src, type) __memcopy(dst, src, sizeof(*(src)), sizeof(*(dst)), sizeof(type), 1, __LINE__, __FUNCTION__, __FILE__)
#define memcopyv(dst, src, type, count) __memcopy(dst, src, sizeof(*(src)), sizeof(*(dst)), sizeof(type), count, __LINE__, __FUNCTION__, __FILE__)

/* NOTE: DESCRIPTION is already defined in common/assert.h but we can't include common/assert.h as it leads to conflicts with renderer/assert.h */
#ifndef DESCRIPTION
#   define DESCRIPTION(bool_value) (CAST_TO(u64, (bool_value)) | (1ULL << 16))
#endif

/* sets the memory zero; and checks if the 'actual_size' is equal to the 'requested_size' */
static INLINE_IF_RELEASE_MODE void* __memzero(void* const ptr, const u32 actual_size, const u32 requested_size, const u32 line, const char* function, const char* file)
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
#if defined(MEMORY_ALLOCATION_SAFETY_LEVEL_2) || defined(MEMORY_ALLOCATION_SAFETY_LEVEL_1)
#   define memzero(ptr, type) __memzero(ptr, sizeof(*(ptr)), sizeof(type), __LINE__, __FUNCTION__, __FILE__)
#else
#   define memzero(ptr, type) unsafe_memzero(ptr, type)
#endif
/* vector version of memzero */
#if defined(MEMORY_ALLOCATION_SAFETY_LEVEL_2) || defined(MEMORY_ALLOCATION_SAFETY_LEVEL_1)
#   define memzerov(ptr, type, count) __memzero(ptr, sizeof(*(ptr)) * (count), sizeof(type) * (count), __LINE__, __FUNCTION__, __FILE__)
#else
#   define memzerov(ptr, type, count) unsafe_memzerov(ptr, type, count)
#endif
/* sets the memory zero; but it doesn't check if the size of the type of the pointer is equal to the size of the type */
#define unsafe_memzero(ptr, type) _debug_memset(ptr, 0, sizeof(type))
/* vector version of memzero */
#define unsafe_memzerov(ptr, type, count) _debug_memset(ptr, 0, sizeof(type) * (count))

#if defined(USE_SAFE_MEMORY)
#   define __safe_memset(ptr, value, size) safe_memset(ptr, value, size)
#   define safe_memzero(ptr, type) __safe_memset(ptr, 0, sizeof(type))
#   define safe_memzerov(ptr, type, count) __safe_memset(ptr, 0, sizeof(type) * (count))
#else
#   define __safe_memset(ptr, value, size) _debug_memset(ptr, value, size)
#   define safe_memzero(ptr, type) memzero(ptr, type)
#   define safe_memzerov(ptr, type, count) memzerov(ptr, type, count)
#endif /* USE_SAFE_MEMORY */

/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: defines.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

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

#include <common/defines.h>
#include <renderer/debug_switches.h>

#define INTERNAL

#ifdef RENDERER_STATIC_LIBRARY
#	define RENDERER_API
#elif RENDERER_DYNAMIC_LIBRARY
#	define RENDERER_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define RENDERER_API __declspec(dllexport)
#else
#	define RENDERER_API
#endif

#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_renderer_t vulkan_renderer_t;
#elif defined(RENDERER_OPENGL_DRIVER)
	typedef struct opengl_renderer_t opengl_renderer_t;
#elif defined(RENDERER_DIRECTX_DRIVER)
	typedef struct directx_renderer_t directx_renderer_t;
#elif defined(RENDERER_METAL_DRIVER)
	typedef struct metal_renderer_t metal_renderer_t;
#endif

typedef struct renderer_t renderer_t;
typedef struct memory_allocator_t memory_allocator_t;

#include <renderer/assert.h>
#define NOT_IMPLEMENTED_FUNCTION() do { LOG_FETAL_ERR("You're trying to call \"%s\" which isn't implemented yet!\n", __FUNCTION__); UNREACHABLE(); } while(false)

#ifdef GLOBAL_DEBUG
#	define INLINE_IF_RELEASE_MODE /* no inline in debug mode */
#	define IF_DEBUG_MODE(x) x
#	define ELSE(x)
#	define PARAM_IF_DEBUG(x) , x
#	define debug_if(x) if(x)
#	define debug_else 	else
#	define debug_else_if(x) else debug_if(x)
#else
#	define INLINE_IF_RELEASE_MODE INLINE
#	define IF_DEBUG_MODE(x)
#	define ELSE(x) x
#	define PARAM_IF_DEBUG(x)
#	define debug_if(x)
#	define debug_else
#	define debug_else_if(x)
#endif /* GLOBAL_DEBUG */

#define IF_DEBUG(x) IF_DEBUG_MODE(x)

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u32 max(u32 v1, u32 v2) { return (v1 > v2) ? v1 : v2; }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u32 min(u32 v1, u32 v2) { return (v1 < v2) ? v1 : v2; }


#define DYNAMIC_CAST(target_type, ptr) CAST_TO(target_type, ptr)

#define typedef_pair_t(T1, T2) struct __pair_##T1##_##T2##_t { T1 first; T2 second; }
#define pair_t(T1, T2) struct __pair_##T1##_##T2##_t
#define make_pair(T1, T2) (pair_t(T1, T2))

#define typedef_tuple2_t(T1, T2) struct __tuple_##T1##_##T2##_t { T1 first; T2 second; }
#define tuple2_t(T1, T2) struct __tuple_##T1##_##T2##_t
#define make_tuple2(T1, T2) (tuple2_t(T1, T2))

#define typedef_tuple3_t(T1, T2, T3) struct __tuple_##T1##_##T2##_##T3##_t { T1 first; T2 second; T3 third; }
#define tuple3_t(T1, T2, T3) struct __tuple_##T1##_##T2##_##T3##_t
#define make_tuple3(T1, T2, T3) (tuple3_t(T1, T2))

#define SIZEOF_ARRAY(array) (sizeof(array) / sizeof((array)[0]))
#define DREF_VOID_PTR(ptr) CAST_TO(void*, DREF_TO(u8*, (void**)(ptr)))

#define U32_TO_U64(src) _u32_to_u64(sizeof(src), src)
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u64 _u32_to_u64(u32 src_size, u32 src)
{
	IF_DEBUG_MODE(_debug_assert__(src_size == sizeof(u32)));
	return CAST_TO(u64, src);
}

#define U64_TO_U32(src) _u64_to_u32(sizeof(src), src)
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u32 _u64_to_u32(u32 src_size, u64 src)
{
	IF_DEBUG_MODE(_debug_assert__(src_size == sizeof(u64)));
	return CAST_TO(u32, src);
}

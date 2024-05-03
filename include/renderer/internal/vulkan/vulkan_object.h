/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_object.h is a part of VulkanRenderer

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

#include <renderer/defines.h>			// stdbool.h
#include <renderer/alloc.h>				// memset
#include <renderer/assert.h> 			// _debug_assert__

typedef enum vulkan_object_nationality_t
{
	VULKAN_OBJECT_NATIONALITY_UNDEFINED = 0,
	VULKAN_OBJECT_NATIONALITY_EXTERNAL,
	VULKAN_OBJECT_NATIONALITY_INTERNAL,
	VULKAN_OBJECT_NATIONALITY_MAX
} vulkan_object_nationality_t;

typedef enum vulkan_object_type_t
{
	VULKAN_OBJECT_TYPE_UNDEFINED = 0,
	VULKAN_OBJECT_TYPE_BUFFER,
	VULKAN_OBJECT_TYPE_SHADER_MODULE,
	VULKAN_OBJECT_TYPE_INSTANCE_BUFFER,
	VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER,
	VULKAN_OBJECT_TYPE_MESH,
	VULKAN_OBJECT_TYPE_IMAGE,
	VULKAN_OBJECT_TYPE_IMAGE_VIEW,
	VULKAN_OBJECT_TYPE_TEXTURE,
	VULKAN_OBJECT_TYPE_RENDER_TEXTURE = VULKAN_OBJECT_TYPE_TEXTURE,
	VULKAN_OBJECT_TYPE_HOST_BUFFERED_TEXTURE,
	VULKAN_OBJECT_TYPE_BITMAP_TEXT,
	VULKAN_OBJECT_TYPE_BITMAP_GLYPH_ATLAS_TEXTURE,
	VULKAN_OBJECT_TYPE_ATTACHMENT,
	VULKAN_OBJECT_TYPE_CAMERA,
	VULKAN_OBJECT_TYPE_CAMERA_SYSTEM,
	VULKAN_OBJECT_TYPE_DESCRIPTOR_SET,
	VULKAN_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
	VULKAN_OBJECT_TYPE_FRAMEBUFFER,
	VULKAN_OBJECT_TYPE_GLYPH_MESH_POOL,
	VULKAN_OBJECT_TYPE_GRAPHICS_PIPELINE,
	VULKAN_OBJECT_TYPE_INSTANCE,
	VULKAN_OBJECT_TYPE_LIGHT,
	VULKAN_OBJECT_TYPE_LOGICAL_DEVICE,
	VULKAN_OBJECT_TYPE_MATERIAL,
	VULKAN_OBJECT_TYPE_MATERIAL_LIBRARY,
	VULKAN_OBJECT_TYPE_PHYSICAL_DEVICE,
	VULKAN_OBJECT_TYPE_PIPELINE_LAYOUT,
	VULKAN_OBJECT_TYPE_RENDER_OBJECT,
	VULKAN_OBJECT_TYPE_RENDER_PASS,
	VULKAN_OBJECT_TYPE_RENDER_PASS_GRAPH,
	VULKAN_OBJECT_TYPE_RENDER_PASS_POOL,
	VULKAN_OBJECT_TYPE_RENDER_QUEUE,
	VULKAN_OBJECT_TYPE_RENDER_SCENE,
	VULKAN_OBJECT_TYPE_SHADER,
	VULKAN_OBJECT_TYPE_SHADER_LIBRARY,
	VULKAN_OBJECT_TYPE_SWAPCHAIN,
	VULKAN_OBJECT_TYPE_MAX
} vulkan_object_type_t;

#define RETURN_STR_CASE(case_value) case case_value: return #case_value

static CAN_BE_UNUSED_FUNCTION const char* vulkan_object_type_to_string(vulkan_object_type_t type)
{
	switch(type)
	{
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_UNDEFINED);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_BUFFER);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_SHADER_MODULE);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_INSTANCE_BUFFER);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_MESH);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_IMAGE);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_IMAGE_VIEW);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_TEXTURE);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_HOST_BUFFERED_TEXTURE);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_BITMAP_TEXT);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_BITMAP_GLYPH_ATLAS_TEXTURE);
		RETURN_STR_CASE(VULKAN_OBJECT_TYPE_MAX);
		default: return "<Unknown Type>";
	}
}
typedef struct vulkan_object_t
{
	vulkan_object_nationality_t nationality;
	vulkan_object_type_t type;
	bool is_next;
} vulkan_object_t;

#define VULKAN_OBJECT(typed_ptr) REINTERPRET_CAST(vulkan_object_t*, typed_ptr)
#define VULKAN_OBJECT_CONST(typed_ptr) REINTERPRET_CONST_CAST(const vulkan_object_t*, typed_ptr)
#define VULKAN_OBJECT_VOID(void_ptr) CAST_TO(vulkan_object_t*, void_ptr)
#define VULKAN_OBJECT_CONST_VOID(void_ptr) CAST_TO(const vulkan_object_t*, void_ptr)

#define __VULKAN_OBJECT__ vulkan_object_t __vo##__LINE__
#define VULKAN_OBJECT_INIT(typed_ptr, type, nationality) vulkan_object_init(VULKAN_OBJECT(typed_ptr), type, nationality)
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_init(vulkan_object_t* obj, vulkan_object_type_t type, bool nationality)
{
	obj->nationality = nationality;
	obj->is_next = false;
	obj->type = type;
}
#define VULKAN_OBJECT_SET_BASE(typed_ptr, is_base) vulkan_object_set_base(VULKAN_OBJECT(typed_ptr), is_base)
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_set_base(vulkan_object_t* obj, bool is_base)
{
	obj->is_next = true;
}
#define VULKAN_OBJECT_IS_BASE(typed_ptr) vulkan_object_is_base(VULKAN_OBJECT_CONST(typed_ptr))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_object_is_base(const vulkan_object_t* obj)
{
	return obj->is_next;
}

#define VULKAN_OBJECT_MEMZERO(typed_ptr, type) vulkan_object_memzero(CAST_TO(void*, VULKAN_OBJECT_VERIFY_FORWARD(typed_ptr)), sizeof(typed_ptr[0]))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_memzero(void* ptr, u32 size)
{
	_debug_assert__((ptr != NULL) && (size != 0));
	_debug_assert__(size >= sizeof(vulkan_object_t));
	memset(ptr + sizeof(vulkan_object_t), 0, size - sizeof(vulkan_object_t));
}




/* nationality (memory ownership) */
#define VULKAN_OBJECT_IS_INTERNAL(typed_ptr) (VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) == VULKAN_OBJECT_NATIONALITY_INTERNAL)
#define VULKAN_OBJECT_IS_EXTERNAL(typed_ptr) (VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) == VULKAN_OBJECT_NATIONALITY_EXTERNAL)
#define VULKAN_OBJECT_IS_UNDEFINED(typed_ptr) (VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) == VULKAN_OBJECT_NATIONALITY_UNDEFINED)
#define VULKAN_OBJECT_IS_NATIONALITY_GARBAGE(typed_ptr) (VULKAN_OBJECT_IS_UNDEFINED(typed_ptr) || (VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) >= VULKAN_OBJECT_NATIONALITY_MAX))

/* type of the object */
#define VULKAN_OBJECT_IS_TYPE_UNDEFINED(typed_ptr) (VULKAN_OBJECT_GET_TYPE(typed_ptr) == VULKAN_OBJECT_TYPE_UNDEFINED)
#define VULKAN_OBJECT_IS_TYPE_GARBAGE(typed_ptr) (VULKAN_OBJECT_IS_TYPE_UNDEFINED(typed_ptr) || (VULKAN_OBJECT_GET_TYPE(typed_ptr) >= VULKAN_OBJECT_TYPE_MAX))

#define VULKAN_OBJECT_SET_NATIONALITY(typed_ptr, nationality) vulkan_object_set_nationality(VULKAN_OBJECT(typed_ptr), nationality)
#define VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) vulkan_object_get_nationality(VULKAN_OBJECT_CONST(typed_ptr))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_object_nationality_t vulkan_object_get_nationality(const vulkan_object_t* obj) { return obj->nationality; }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_set_nationality(vulkan_object_t* obj, bool nationality) { obj->nationality = nationality; }

#define VULKAN_OBJECT_SET_TYPE(typed_ptr, type) vulkan_object_set_type(VULKAN_OBJECT(typed_ptr), type)
#define VULKAN_OBJECT_GET_TYPE(typed_ptr) vulkan_object_get_type(VULKAN_OBJECT_CONST(typed_ptr))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_object_type_t vulkan_object_get_type(const vulkan_object_t* obj) { return obj->type; }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_set_type(vulkan_object_t* obj, vulkan_object_type_t type) { obj->type = type; }

/* returns the lowest multiple of x greater or equal to y*/
static INLINE u32 lowest_multiple_of(u32 x, u32 y)
{
	return (x % y) + x;
}

#define VULKAN_OBJECT_UP_CAST(dst_ptr_type, dst_vulkan_object_type, src_typed_ptr)  CAST_TO(dst_ptr_type, vulkan_object_up_cast(VULKAN_OBJECT(src_typed_ptr), dst_vulkan_object_type))
static CAN_BE_UNUSED_FUNCTION void* vulkan_object_up_cast(vulkan_object_t* obj, vulkan_object_type_t dst_type)
{
	if(obj->type == dst_type)
		return CAST_TO(void*, obj);
	debug_assert__(obj->is_next == true, "Vulkan Object Down Cast failed, end of derivation hierarchy is reached, %s -> %s",
										vulkan_object_type_to_string(obj->type), vulkan_object_type_to_string(dst_type));
	#ifndef GLOBAL_DEBUG
	if(!vulkan_object_is_base(obj))
		return NULL;
	#endif /* not GLOBAL_DEBUG */

	return vulkan_object_up_cast(CAST_TO(void*, obj) + lowest_multiple_of(sizeof(vulkan_object_t), sizeof(void*)), dst_type);
}
#define VULKAN_OBJECT_UP_CAST_CONST(dst_ptr_type, dst_vulkan_object_type, src_typed_ptr) CAST_TO(dst_ptr_type, vulkan_object_up_cast_const(VULKAN_OBJECT_CONST(src_typed_ptr), dst_vulkan_object_type))
static CAN_BE_UNUSED_FUNCTION const void* vulkan_object_up_cast_const(const vulkan_object_t* obj, vulkan_object_type_t dst_type)
{
	if(obj->type == dst_type)
		return CAST_TO(void*, obj);
	debug_assert__(obj->is_next == true, "Vulkan Object Down Cast failed, end of derivation hierarchy is reached, %s -> %s",
										vulkan_object_type_to_string(obj->type), vulkan_object_type_to_string(dst_type));
	#ifndef GLOBAL_DEBUG
	if(!vulkan_object_is_base(obj))
		return NULL;
	#endif /* not GLOBAL_DEBUG */

	return vulkan_object_up_cast_const(CAST_TO(void*, obj) + lowest_multiple_of(sizeof(vulkan_object_t), sizeof(void*)), dst_type);
}

#ifdef GLOBAL_DEBUG
#	define VULKAN_OBJECT_TYPE_CAST(dst_ptr_type, vulkan_object_type, src_typed_ptr) REINTERPRET_CAST(dst_ptr_type, VULKAN_OBJECT_TYPE_CHECK_FORWARD(src_typed_ptr, vulkan_object_type))
#	define VULKAN_OBJECT_TYPE_CONST_CAST(dst_ptr_type, vulkan_object_type, src_typed_ptr) REINTERPRET_CONST_CAST(dst_ptr_type, VULKAN_OBJECT_TYPE_CHECK_CONST_FORWARD(src_typed_ptr, vulkan_object_type))
#	define VULKAN_OBJECT_TYPE_CHECK(type_ptr, vulkan_object_type) vulkan_object_type_check(type_ptr, vulkan_object_type)
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_type_check(const vulkan_object_t* obj, vulkan_object_type_t type)
	{
		debug_assert__(obj->type == type, "Vulkan Object type mismatched, %s to %s failed", vulkan_object_type_to_string(obj->type), vulkan_object_type_to_string(type));
	}
#	define VULKAN_OBJECT_TYPE_CHECK_FORWARD(typed_ptr, vulkan_object_type) vulkan_object_type_check_forward(VULKAN_OBJECT_CONST(typed_ptr), vulkan_object_type)
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void* vulkan_object_type_check_forward(const vulkan_object_t* obj, vulkan_object_type_t type)
	{
		vulkan_object_type_check(obj, type);
		return CAST_TO(void*, obj);
	}
#	define VULKAN_OBJECT_TYPE_CHECK_CONST_FORWARD(typed_ptr, vulkan_object_type) vulkan_object_type_check_const_forward(VULKAN_OBJECT_CONST(typed_ptr), vulkan_object_type)
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const void* vulkan_object_type_check_const_forward(const vulkan_object_t* obj, vulkan_object_type_t type)
	{
		vulkan_object_type_check(obj, type);
		return CAST_TO(const void*, obj);
	}
#else /* GLOBAL_DEBUG */
#	define VULKAN_OBJECT_TYPE_CAST(dst_ptr_type, vulkan_object_type, src_typed_ptr) REINTERPRET_CAST(dst_ptr_type, src_typed_ptr)
#	define VULKAN_OBJECT_TYPE_CONST_CAST(dst_ptr_type, vulkan_object_type, src_typed_ptr) REINTERPRET_CONST_CAST(dst_ptr_type, src_typed_ptr)
#endif /* GLOBAL_RELEASE */

/* no verification of vulkan objects can be done in release mode */
#ifdef GLOBAL_DEBUG
#	define VULKAN_OBJECT_VERIFY(typed_ptr) vulkan_object_verify(VULKAN_OBJECT_CONST(typed_ptr))
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_verify(const vulkan_object_t* obj)
	{
		debug_assert__(!VULKAN_OBJECT_IS_TYPE_GARBAGE(obj), "Vulkan Object Type is Garbage");
		debug_assert__(!VULKAN_OBJECT_IS_NATIONALITY_GARBAGE(obj), "Vulkan Object Nationality is Garbage");
	}
#	define VULKAN_OBJECT_VERIFY_FORWARD(typed_ptr) vulkan_object_verify_forward(VULKAN_OBJECT_CONST(typed_ptr))
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const void* vulkan_object_verify_forward(const vulkan_object_t* obj)
	{
		vulkan_object_verify(obj);
		return CAST_TO(const void*, obj);
	}
#else
#	define VULKAN_OBJECT_VERIFY(typed_ptr)
#	define VULKAN_OBJECT_VERIFY_FORWARD(typed_ptr) typed_ptr
#endif

/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: object.h is a part of VulkanRenderer

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

#include <sge/defines.h>			// stdbool.h
#include <sge/assert.h> 			// _debug_assert__
#include <common/alloc.h>				// memset

typedef enum object_nationality_t
{
	OBJECT_NATIONALITY_UNDEFINED = 0,
	OBJECT_NATIONALITY_EXTERNAL,
	OBJECT_NATIONALITY_INTERNAL,
	OBJECT_NATIONALITY_MAX
} object_nationality_t;

typedef enum object_type_t
{
	OBJECT_TYPE_UNDEFINED = 0,

	/* Vulkan Objects */
	OBJECT_TYPE_VK_BUFFER,
	OBJECT_TYPE_VK_SHADER_MODULE,
	OBJECT_TYPE_VK_INSTANCE_BUFFER,
	OBJECT_TYPE_VK_HOST_BUFFERED_BUFFER,
	OBJECT_TYPE_VK_FORMATTED_BUFFER,
	OBJECT_TYPE_VK_MESH,
	OBJECT_TYPE_VK_IMAGE,
	OBJECT_TYPE_VK_IMAGE_VIEW,
	OBJECT_TYPE_VK_TEXTURE,
	OBJECT_TYPE_VK_HOST_BUFFERED_TEXTURE,
	OBJECT_TYPE_VK_TEXT_MESH,
	OBJECT_TYPE_VK_BITMAP_TEXT,
	OBJECT_TYPE_VK_BITMAP_GLYPH_ATLAS_TEXTURE,
	OBJECT_TYPE_VK_ATTACHMENT,
	OBJECT_TYPE_VK_CAMERA,
	OBJECT_TYPE_VK_CAMERA_SYSTEM,
	OBJECT_TYPE_VK_DESCRIPTOR_SET,
	OBJECT_TYPE_VK_DESCRIPTOR_SET_LAYOUT,
	OBJECT_TYPE_VK_FRAMEBUFFER,
	OBJECT_TYPE_VK_GLYPH_MESH_POOL,
	OBJECT_TYPE_VK_GRAPHICS_PIPELINE,
	OBJECT_TYPE_VK_INSTANCE,
	OBJECT_TYPE_VK_LIGHT,
	OBJECT_TYPE_VK_DIRECTIONAL_LIGHT,
	OBJECT_TYPE_VK_SPOT_LIGHT,
	OBJECT_TYPE_VK_POINT_LIGHT,
	OBJECT_TYPE_VK_LOGICAL_DEVICE,
	OBJECT_TYPE_VK_MATERIAL,
	OBJECT_TYPE_VK_MATERIAL_LIBRARY,
	OBJECT_TYPE_VK_PHYSICAL_DEVICE,
	OBJECT_TYPE_VK_PIPELINE_LAYOUT,
	OBJECT_TYPE_VK_RENDER_OBJECT,
	OBJECT_TYPE_VK_RENDER_PASS,
	OBJECT_TYPE_VK_RENDER_PASS_GRAPH,
	OBJECT_TYPE_VK_RENDER_PASS_POOL,
	OBJECT_TYPE_VK_RENDER_QUEUE,
	OBJECT_TYPE_VK_RENDER_SCENE,
	OBJECT_TYPE_VK_SHADER,
	OBJECT_TYPE_VK_SHADER_LIBRARY,
	OBJECT_TYPE_VK_SWAPCHAIN,
	OBJECT_TYPE_VK_ALLOCATOR,

	OBJECT_TYPE_RENDER_WINDOW,
	OBJECT_TYPE_EVENT,
	OBJECT_TYPE_BUFFER2D_VIEW,
	OBJECT_TYPE_BUFFER2D,
	OBJECT_TYPE_BITMAP_GLYPH_POOL,
	OBJECT_TYPE_STRUCT_DESCRIPTOR,

	OBJECT_TYPE_MAX,
	OBJECT_TYPE_VK_AMBIENT_LIGHT = OBJECT_TYPE_VK_LIGHT,
	OBJECT_TYPE_VK_RENDER_TEXTURE = OBJECT_TYPE_VK_TEXTURE,
} object_type_t;

#define RETURN_STR_CASE(case_value) case case_value: return #case_value

static CAN_BE_UNUSED_FUNCTION const char* object_type_to_string(object_type_t type)
{
	switch(type)
	{
		RETURN_STR_CASE(OBJECT_TYPE_UNDEFINED);
		RETURN_STR_CASE(OBJECT_TYPE_VK_BUFFER);
		RETURN_STR_CASE(OBJECT_TYPE_VK_SHADER_MODULE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_INSTANCE_BUFFER);
		RETURN_STR_CASE(OBJECT_TYPE_VK_HOST_BUFFERED_BUFFER);
		RETURN_STR_CASE(OBJECT_TYPE_VK_FORMATTED_BUFFER);
		RETURN_STR_CASE(OBJECT_TYPE_VK_MESH);
		RETURN_STR_CASE(OBJECT_TYPE_VK_IMAGE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_IMAGE_VIEW);
		RETURN_STR_CASE(OBJECT_TYPE_VK_TEXTURE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_HOST_BUFFERED_TEXTURE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_TEXT_MESH);
		RETURN_STR_CASE(OBJECT_TYPE_VK_BITMAP_TEXT);
		RETURN_STR_CASE(OBJECT_TYPE_VK_BITMAP_GLYPH_ATLAS_TEXTURE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_ATTACHMENT);
		RETURN_STR_CASE(OBJECT_TYPE_VK_CAMERA);
		RETURN_STR_CASE(OBJECT_TYPE_VK_CAMERA_SYSTEM);
		RETURN_STR_CASE(OBJECT_TYPE_VK_DESCRIPTOR_SET);
		RETURN_STR_CASE(OBJECT_TYPE_VK_DESCRIPTOR_SET_LAYOUT);
		RETURN_STR_CASE(OBJECT_TYPE_VK_FRAMEBUFFER);
		RETURN_STR_CASE(OBJECT_TYPE_VK_GLYPH_MESH_POOL);
		RETURN_STR_CASE(OBJECT_TYPE_VK_GRAPHICS_PIPELINE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_INSTANCE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_LIGHT);
		RETURN_STR_CASE(OBJECT_TYPE_VK_DIRECTIONAL_LIGHT);
		RETURN_STR_CASE(OBJECT_TYPE_VK_SPOT_LIGHT);
		RETURN_STR_CASE(OBJECT_TYPE_VK_POINT_LIGHT);
		RETURN_STR_CASE(OBJECT_TYPE_VK_LOGICAL_DEVICE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_MATERIAL);
		RETURN_STR_CASE(OBJECT_TYPE_VK_MATERIAL_LIBRARY);
		RETURN_STR_CASE(OBJECT_TYPE_VK_PHYSICAL_DEVICE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_PIPELINE_LAYOUT);
		RETURN_STR_CASE(OBJECT_TYPE_VK_RENDER_OBJECT);
		RETURN_STR_CASE(OBJECT_TYPE_VK_RENDER_PASS);
		RETURN_STR_CASE(OBJECT_TYPE_VK_RENDER_PASS_GRAPH);
		RETURN_STR_CASE(OBJECT_TYPE_VK_RENDER_PASS_POOL);
		RETURN_STR_CASE(OBJECT_TYPE_VK_RENDER_QUEUE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_RENDER_SCENE);
		RETURN_STR_CASE(OBJECT_TYPE_VK_SHADER);
		RETURN_STR_CASE(OBJECT_TYPE_VK_SHADER_LIBRARY);
		RETURN_STR_CASE(OBJECT_TYPE_VK_SWAPCHAIN);
		RETURN_STR_CASE(OBJECT_TYPE_VK_ALLOCATOR);
		RETURN_STR_CASE(OBJECT_TYPE_RENDER_WINDOW);
		RETURN_STR_CASE(OBJECT_TYPE_EVENT);
		RETURN_STR_CASE(OBJECT_TYPE_BUFFER2D_VIEW);
		RETURN_STR_CASE(OBJECT_TYPE_BUFFER2D);
		RETURN_STR_CASE(OBJECT_TYPE_BITMAP_GLYPH_POOL);
		RETURN_STR_CASE(OBJECT_TYPE_STRUCT_DESCRIPTOR);
		RETURN_STR_CASE(OBJECT_TYPE_MAX);
		default: return "<Unknown Type>";
	}
}

#undef RETURN_STR_CASE

typedef struct object_t
{
	object_nationality_t nationality;
	object_type_t type;
	bool is_next;
	bool is_prev;
} object_t;

#define OBJECT(typed_ptr) OBJECT_VOID(typed_ptr)
#define OBJECT_CONST(typed_ptr) OBJECT_CONST_VOID(typed_ptr)
#define OBJECT_VOID(void_ptr) CAST_TO(object_t*, void_ptr)
#define OBJECT_CONST_VOID(void_ptr) CAST_TO(const object_t*, void_ptr)

#define __OBJECT__ object_t __vo##__LINE__
#define OBJECT_INIT(typed_ptr, type, nationality) object_init(OBJECT(typed_ptr), type, nationality)
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void object_init(object_t* obj, object_type_t type, object_nationality_t nationality)
{
	obj->nationality = nationality;
	obj->is_next = false;
	obj->is_prev = false;
	obj->type = type;
}
/* returns the lowest multiple of x greater or equal to y*/
static INLINE u32 lowest_multiple_of(u32 x, u32 y)
{
	return (x % y) + x;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"

#define OBJECT_STRIDE lowest_multiple_of(sizeof(object_t), sizeof(void*))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE object_t* object_get_next(object_t* obj)
{
	return CAST_TO(object_t*, CAST_TO(u8*, obj) + OBJECT_STRIDE);
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const object_t* object_get_next_const(const object_t* obj)
{
	return CAST_TO(const object_t*, CAST_TO(const u8*, obj) + OBJECT_STRIDE);
}
#pragma GCC diagnostic pop

#define OBJECT_SET_BASE(typed_ptr, is_base) object_set_base(OBJECT(typed_ptr), is_base)
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void object_set_base(object_t* obj, bool is_base)
{
	obj->is_next = true;
	object_get_next(obj)->is_prev = true;
}
#define OBJECT_IS_BASE(typed_ptr) object_is_base(OBJECT_CONST(typed_ptr))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool object_is_base(const object_t* obj)
{
	return obj->is_next;
}
#define OBJECT_IS_PREV(typed_ptr) object_is_prev(OBJECT_CONST(typed_ptr))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool object_is_prev(const object_t* obj)
{
	return obj->is_prev;
}

/* nationality (memory ownership) */
#define OBJECT_IS_INTERNAL(typed_ptr) (OBJECT_GET_NATIONALITY(typed_ptr) == OBJECT_NATIONALITY_INTERNAL)
#define OBJECT_IS_EXTERNAL(typed_ptr) (OBJECT_GET_NATIONALITY(typed_ptr) == OBJECT_NATIONALITY_EXTERNAL)
#define OBJECT_IS_UNDEFINED(typed_ptr) (OBJECT_GET_NATIONALITY(typed_ptr) == OBJECT_NATIONALITY_UNDEFINED)
#define OBJECT_IS_NATIONALITY_GARBAGE(typed_ptr) (OBJECT_IS_UNDEFINED(typed_ptr) || (OBJECT_GET_NATIONALITY(typed_ptr) >= OBJECT_NATIONALITY_MAX))

/* type of the object */
#define OBJECT_IS_TYPE_UNDEFINED(typed_ptr) (OBJECT_GET_TYPE(typed_ptr) == OBJECT_TYPE_UNDEFINED)
#define OBJECT_IS_TYPE_GARBAGE(typed_ptr) (OBJECT_IS_TYPE_UNDEFINED(typed_ptr) || (OBJECT_GET_TYPE(typed_ptr) >= OBJECT_TYPE_MAX))

#define OBJECT_SET_NATIONALITY(typed_ptr, nationality) object_set_nationality(OBJECT(typed_ptr), nationality)
#define OBJECT_GET_NATIONALITY(typed_ptr) object_get_nationality(OBJECT_CONST(typed_ptr))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE object_nationality_t object_get_nationality(const object_t* obj) { return obj->nationality; }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void object_set_nationality(object_t* obj, object_nationality_t nationality) { obj->nationality = nationality; }

#define OBJECT_SET_TYPE(typed_ptr, type) object_set_type(OBJECT(typed_ptr), type)
#define OBJECT_GET_TYPE(typed_ptr) object_get_type(OBJECT_CONST(typed_ptr))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE object_type_t object_get_type(const object_t* obj) { return obj->type; }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void object_set_type(object_t* obj, object_type_t type) { obj->type = type; }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"

/* OBJECT_MEMZERO preserves the derivation from __OBJECT__ and zeros out the memory following that */
#define OBJECT_MEMZERO(typed_ptr, type) object_memzero(CAST_TO(void*, OBJECT_VERIFY_FORWARD(typed_ptr)), sizeof(typed_ptr[0]))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void object_memzero(void* ptr, u32 size)
{
	_debug_assert__((ptr != NULL) && (size != 0));
	_debug_assert__(size >= sizeof(object_t));
	memset(CAST_TO(u8*, ptr) + sizeof(object_t), 0, size - sizeof(object_t));
}
#pragma GCC diagnostic pop

/* OBJECT_MEMCOPY preserves the nationality of the destination object, so it must be used to copy objects which are derived from __OBJECT__ */
#define OBJECT_MEMCOPY(dst_typed_ptr, src_typed_ptr, type) object_memcpy(CAST_TO(void*, OBJECT_VERIFY_FORWARD(dst_typed_ptr)), CAST_TO(void*, OBJECT_VERIFY_FORWARD(src_typed_ptr)), sizeof(dst_typed_ptr[0]) \
														PARAM_IF_DEBUG(sizeof(src_typed_ptr[0])) \
														PARAM_IF_DEBUG(sizeof(type)))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void object_memcpy(void* dst_ptr, void* src_ptr, u32 dst_size 
														PARAM_IF_DEBUG(u32 src_size)
														PARAM_IF_DEBUG(u32 assert_size))
{
	_debug_assert__(OBJECT_GET_TYPE(dst_ptr) == OBJECT_GET_TYPE(src_ptr));
DEBUG_BLOCK(
	_debug_assert__(dst_size == src_size);
	_debug_assert__(src_size == assert_size);
)
	AUTO dst_nationality = OBJECT_GET_NATIONALITY(dst_ptr);
	memcpy(dst_ptr, src_ptr, dst_size);
	OBJECT_SET_NATIONALITY(dst_ptr, dst_nationality);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"

#define OBJECT_UP_CAST(dst_ptr_type, dst_object_type, src_typed_ptr)  CAST_TO(dst_ptr_type, object_up_cast(OBJECT(src_typed_ptr), dst_object_type))
static CAN_BE_UNUSED_FUNCTION void* object_up_cast(object_t* obj, object_type_t dst_type)
{
	if(obj->type == dst_type)
		return CAST_TO(void*, obj);
	debug_assert__(obj->is_next == true, "Object Down Cast failed, end of derivation hierarchy is reached, %s -> %s",
										object_type_to_string(obj->type), object_type_to_string(dst_type));
	#ifndef GLOBAL_DEBUG
	if(!object_is_base(obj))
		return NULL;
	#endif /* not GLOBAL_DEBUG */

	return object_up_cast(object_get_next(obj), dst_type);
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE object_t* object_get_prev(object_t* obj)
{
	_debug_assert__(obj != NULL);
	return CAST_TO(object_t*, CAST_TO(u8*, obj) - OBJECT_STRIDE);
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const object_t* object_get_prev_const(const object_t* obj)
{
	_debug_assert__(obj != NULL);
	return CAST_TO(const object_t*, CAST_TO(const u8*, obj) - OBJECT_STRIDE);
}
#define OBJECT_DOWN_CAST(dst_ptr_type, dst_object_type, src_typed_ptr)  CAST_TO(dst_ptr_type, object_down_cast(OBJECT(src_typed_ptr), dst_object_type))
static CAN_BE_UNUSED_FUNCTION void* object_down_cast(object_t* obj, object_type_t dst_type)
{
	if(obj->type == dst_type)
		return CAST_TO(void*, obj);
	debug_assert__(obj->is_prev == true, "Object Up Cast failed, end of derivation hierarchy is reached, %s -> %s",
										object_type_to_string(obj->type), object_type_to_string(dst_type));
	#ifndef GLOBAL_DEBUG
	if(!object_is_prev(obj))
		return NULL;
	#endif /* not GLOBAL_DEBUG */

	return object_down_cast(object_get_prev(obj), dst_type);
}
#define OBJECT_UP_CAST_CONST(dst_ptr_type, dst_object_type, src_typed_ptr) CAST_TO(dst_ptr_type, object_up_cast_const(OBJECT_CONST(src_typed_ptr), dst_object_type))
static CAN_BE_UNUSED_FUNCTION const void* object_up_cast_const(const object_t* obj, object_type_t dst_type)
{
	if(obj->type == dst_type)
		return CAST_TO(void*, obj);
	debug_assert__(obj->is_next == true, "Object Down Cast failed, end of derivation hierarchy is reached, %s -> %s",
										object_type_to_string(obj->type), object_type_to_string(dst_type));
	#ifndef GLOBAL_DEBUG
	if(!object_is_base(obj))
		return NULL;
	#endif /* not GLOBAL_DEBUG */

	return object_up_cast_const(object_get_next_const(obj), dst_type);
}
#define OBJECT_DOWN_CAST_CONST(dst_ptr_type, dst_object_type, src_typed_ptr) CAST_TO(dst_ptr_type, object_down_cast_const(OBJECT_CONST(src_typed_ptr), dst_object_type))
static CAN_BE_UNUSED_FUNCTION const void* object_down_cast_const(const object_t* obj, object_type_t dst_type)
{
	if(obj->type == dst_type)
		return CAST_TO(void*, obj);
	debug_assert__(obj->is_prev == true, "Object Up Cast failed, end of derivation hierarchy is reached, %s -> %s",
										object_type_to_string(obj->type), object_type_to_string(dst_type));
	#ifndef GLOBAL_DEBUG
	if(!object_is_prev(obj))
		return NULL;
	#endif /* not GLOBAL_DEBUG */

	return object_down_cast_const(object_get_prev_const(obj), dst_type);
}

#pragma GCC diagnostic pop

#ifdef GLOBAL_DEBUG
#	define OBJECT_TYPE_CAST(dst_ptr_type, object_type, src_typed_ptr) _REINTERPRET_CAST(dst_ptr_type, OBJECT_TYPE_CHECK_FORWARD(src_typed_ptr, object_type), sizeof(DREF(src_typed_ptr)))
#	define OBJECT_TYPE_CONST_CAST(dst_ptr_type, object_type, src_typed_ptr) _REINTERPRET_CONST_CAST(dst_ptr_type, OBJECT_TYPE_CHECK_CONST_FORWARD(src_typed_ptr, object_type), sizeof(DREF(src_typed_ptr)))
#	define OBJECT_TYPE_CHECK(type_ptr, object_type) object_type_check(type_ptr, object_type)
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void object_type_check(const object_t* obj, object_type_t type)
	{
		debug_assert__(obj->type == type, "Object type mismatched, %s to %s failed", object_type_to_string(obj->type), object_type_to_string(type));
	}
#	define OBJECT_TYPE_CHECK_FORWARD(typed_ptr, object_type) object_type_check_forward(OBJECT_CONST(typed_ptr), object_type)
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void* object_type_check_forward(const object_t* obj, object_type_t type)
	{
		object_type_check(obj, type);
		return CAST_TO(void*, obj);
	}
#	define OBJECT_TYPE_CHECK_CONST_FORWARD(typed_ptr, object_type) object_type_check_const_forward(OBJECT_CONST(typed_ptr), object_type)
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const void* object_type_check_const_forward(const object_t* obj, object_type_t type)
	{
		object_type_check(obj, type);
		return CAST_TO(const void*, obj);
	}
#else /* GLOBAL_DEBUG */
#	define OBJECT_TYPE_CAST(dst_ptr_type, object_type, src_typed_ptr) REINTERPRET_CAST(dst_ptr_type, src_typed_ptr)
#	define OBJECT_TYPE_CONST_CAST(dst_ptr_type, object_type, src_typed_ptr) REINTERPRET_CONST_CAST(dst_ptr_type, src_typed_ptr)
#endif /* GLOBAL_RELEASE */

/* no verification of vulkan objects can be done in release mode */
#ifdef GLOBAL_DEBUG
#	define OBJECT_VERIFY(typed_ptr) object_verify(OBJECT_CONST(typed_ptr))
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void object_verify(const object_t* obj)
	{
		debug_assert__(!OBJECT_IS_TYPE_GARBAGE(obj), "Object Type is Garbage");
		debug_assert__(!OBJECT_IS_NATIONALITY_GARBAGE(obj), "Object Nationality is Garbage");
	}
#	define OBJECT_VERIFY_FORWARD(typed_ptr) object_verify_forward(OBJECT_CONST(typed_ptr))
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const void* object_verify_forward(const object_t* obj)
	{
		object_verify(obj);
		return CAST_TO(const void*, obj);
	}
#else
#	define OBJECT_VERIFY(typed_ptr)
#	define OBJECT_VERIFY_FORWARD(typed_ptr) typed_ptr
#endif

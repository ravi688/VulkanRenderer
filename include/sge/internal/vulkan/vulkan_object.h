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

#include <sge/object.h>

typedef enum vulkan_object_nationality_t
{
	VULKAN_OBJECT_NATIONALITY_UNDEFINED = OBJECT_NATIONALITY_UNDEFINED,
	VULKAN_OBJECT_NATIONALITY_EXTERNAL = OBJECT_NATIONALITY_EXTERNAL,
	VULKAN_OBJECT_NATIONALITY_INTERNAL = OBJECT_NATIONALITY_INTERNAL,
	VULKAN_OBJECT_NATIONALITY_MAX = OBJECT_NATIONALITY_MAX
} vulkan_object_nationality_t;

typedef enum vulkan_object_type_t
{
	VULKAN_OBJECT_TYPE_UNDEFINED = OBJECT_TYPE_UNDEFINED,
	VULKAN_OBJECT_TYPE_BUFFER = OBJECT_TYPE_VK_BUFFER,
	VULKAN_OBJECT_TYPE_SHADER_MODULE = OBJECT_TYPE_VK_SHADER_MODULE,
	VULKAN_OBJECT_TYPE_INSTANCE_BUFFER = OBJECT_TYPE_VK_INSTANCE_BUFFER,
	VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER = OBJECT_TYPE_VK_HOST_BUFFERED_BUFFER,
	VULKAN_OBJECT_TYPE_FORMATTED_BUFFER = OBJECT_TYPE_VK_FORMATTED_BUFFER,
	VULKAN_OBJECT_TYPE_MESH = OBJECT_TYPE_VK_MESH,
	VULKAN_OBJECT_TYPE_IMAGE = OBJECT_TYPE_VK_IMAGE,
	VULKAN_OBJECT_TYPE_IMAGE_VIEW = OBJECT_TYPE_VK_IMAGE_VIEW,
	VULKAN_OBJECT_TYPE_TEXTURE = OBJECT_TYPE_VK_TEXTURE,
	VULKAN_OBJECT_TYPE_RENDER_TEXTURE = OBJECT_TYPE_VK_RENDER_TEXTURE,
	VULKAN_OBJECT_TYPE_HOST_BUFFERED_TEXTURE = OBJECT_TYPE_VK_HOST_BUFFERED_TEXTURE,
	VULKAN_OBJECT_TYPE_TEXT_MESH = OBJECT_TYPE_VK_TEXT_MESH,
	VULKAN_OBJECT_TYPE_BITMAP_TEXT = OBJECT_TYPE_VK_BITMAP_TEXT,
	VULKAN_OBJECT_TYPE_BITMAP_GLYPH_ATLAS_TEXTURE = OBJECT_TYPE_VK_BITMAP_GLYPH_ATLAS_TEXTURE,
	VULKAN_OBJECT_TYPE_ATTACHMENT = OBJECT_TYPE_VK_ATTACHMENT,
	VULKAN_OBJECT_TYPE_CAMERA = OBJECT_TYPE_VK_CAMERA,
	VULKAN_OBJECT_TYPE_CAMERA_SYSTEM = OBJECT_TYPE_VK_CAMERA_SYSTEM,
	VULKAN_OBJECT_TYPE_DESCRIPTOR_SET = OBJECT_TYPE_VK_DESCRIPTOR_SET,
	VULKAN_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT = OBJECT_TYPE_VK_DESCRIPTOR_SET_LAYOUT,
	VULKAN_OBJECT_TYPE_FRAMEBUFFER = OBJECT_TYPE_VK_FRAMEBUFFER,
	VULKAN_OBJECT_TYPE_GLYPH_MESH_POOL = OBJECT_TYPE_VK_GLYPH_MESH_POOL,
	VULKAN_OBJECT_TYPE_GRAPHICS_PIPELINE = OBJECT_TYPE_VK_GRAPHICS_PIPELINE,
	VULKAN_OBJECT_TYPE_INSTANCE = OBJECT_TYPE_VK_INSTANCE,
	VULKAN_OBJECT_TYPE_LIGHT = OBJECT_TYPE_VK_LIGHT,
	VULKAN_OBJECT_TYPE_DIRECTIONAL_LIGHT = OBJECT_TYPE_VK_DIRECTIONAL_LIGHT,
	VULKAN_OBJECT_TYPE_SPOT_LIGHT = OBJECT_TYPE_VK_SPOT_LIGHT,
	VULKAN_OBJECT_TYPE_POINT_LIGHT = OBJECT_TYPE_VK_POINT_LIGHT,
	VULKAN_OBJECT_TYPE_AMBIENT_LIGHT = OBJECT_TYPE_VK_AMBIENT_LIGHT,
	VULKAN_OBJECT_TYPE_LOGICAL_DEVICE = OBJECT_TYPE_VK_LOGICAL_DEVICE,
	VULKAN_OBJECT_TYPE_MATERIAL = OBJECT_TYPE_VK_MATERIAL,
	VULKAN_OBJECT_TYPE_MATERIAL_LIBRARY = OBJECT_TYPE_VK_MATERIAL_LIBRARY,
	VULKAN_OBJECT_TYPE_PHYSICAL_DEVICE = OBJECT_TYPE_VK_PHYSICAL_DEVICE,
	VULKAN_OBJECT_TYPE_PIPELINE_LAYOUT = OBJECT_TYPE_VK_PIPELINE_LAYOUT,
	VULKAN_OBJECT_TYPE_RENDER_OBJECT = OBJECT_TYPE_VK_RENDER_OBJECT,
	VULKAN_OBJECT_TYPE_RENDER_PASS = OBJECT_TYPE_VK_RENDER_PASS,
	VULKAN_OBJECT_TYPE_RENDER_PASS_GRAPH = OBJECT_TYPE_VK_RENDER_PASS_GRAPH,
	VULKAN_OBJECT_TYPE_RENDER_PASS_POOL = OBJECT_TYPE_VK_RENDER_PASS_POOL,
	VULKAN_OBJECT_TYPE_RENDER_QUEUE = OBJECT_TYPE_VK_RENDER_QUEUE,
	VULKAN_OBJECT_TYPE_RENDER_SCENE = OBJECT_TYPE_VK_RENDER_SCENE,
	VULKAN_OBJECT_TYPE_SHADER = OBJECT_TYPE_VK_SHADER,
	VULKAN_OBJECT_TYPE_SHADER_LIBRARY = OBJECT_TYPE_VK_SHADER_LIBRARY,
	VULKAN_OBJECT_TYPE_SWAPCHAIN = OBJECT_TYPE_VK_SWAPCHAIN,
	VULKAN_OBJECT_TYPE_ALLOCATOR = OBJECT_TYPE_VK_ALLOCATOR,
	VULKAN_OBJECT_TYPE_MAX = OBJECT_TYPE_MAX
} vulkan_object_type_t;

#define __VULKAN_OBJECT__ __OBJECT__
#define VULKAN_OBJECT_INIT(typed_ptr, type, nationality) OBJECT_INIT(typed_ptr, CAST_TO(object_type_t, type), nationality)
#define VULKAN_OBJECT_SET_BASE(typed_ptr, is_base) OBJECT_SET_BASE(typed_ptr, is_base)
#define VULKAN_OBJECT_IS_BASE(typed_ptr) OBJECT_IS_BASE(typed_ptr)

#define VULKAN_OBJECT_MEMZERO(typed_ptr, type) OBJECT_MEMZERO(typed_ptr, type)
#define VULKAN_OBJECT_MEMCOPY(dst_typed_ptr, src_typed_ptr)

/* nationality (memory ownership) */
#define VULKAN_OBJECT_IS_INTERNAL(typed_ptr) OBJECT_IS_INTERNAL(typed_ptr)
#define VULKAN_OBJECT_IS_EXTERNAL(typed_ptr) OBJECT_IS_EXTERNAL(typed_ptr)
#define VULKAN_OBJECT_IS_UNDEFINED(typed_ptr) OBJECT_IS_UNDEFINED(typed_ptr)
#define VULKAN_OBJECT_IS_NATIONALITY_GARBAGE(typed_ptr) OBJECT_IS_NATIONALITY_GARBAGE(typed_ptr)

/* type of the object */
#define VULKAN_OBJECT_IS_TYPE_UNDEFINED(typed_ptr) OBJECT_IS_TYPE_UNDEFINED(typed_ptr)
#define VULKAN_OBJECT_IS_TYPE_GARBAGE(typed_ptr) OBJECT_IS_TYPE_GARBAGE(typed_ptr)

#define VULKAN_OBJECT_SET_NATIONALITY(typed_ptr, nationality) OBJECT_SET_NATIONALITY(typed_ptr, nationality)
#define VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) OBJECT_GET_NATIONALITY(typed_ptr)

#define VULKAN_OBJECT_SET_TYPE(typed_ptr, type) OBJECT_SET_TYPE(typed_ptr, CAST_TO(object_type_t, type))
#define VULKAN_OBJECT_GET_TYPE(typed_ptr) OBJECT_GET_TYPE(typed_ptr)

#define VULKAN_OBJECT_UP_CAST(dst_ptr_type, dst_vulkan_object_type, src_typed_ptr) OBJECT_UP_CAST(dst_ptr_type, dst_vulkan_object_type, src_typed_ptr) 
#define VULKAN_OBJECT_UP_CAST_CONST(dst_ptr_type, dst_vulkan_object_type, src_typed_ptr) OBJECT_UP_CAST_CONST(dst_ptr_type, dst_vulkan_object_type, src_typed_ptr)
#define VULKAN_OBJECT_DOWN_CAST(dst_ptr_type, dst_vulkan_object_type, src_typed_ptr) OBJECT_DOWN_CAST(dst_ptr_type, dst_vulkan_object_type, src_typed_ptr) 
#define VULKAN_OBJECT_DOWN_CAST_CONST(dst_ptr_type, dst_vulkan_object_type, src_typed_ptr) OBJECT_DOWN_CAST_CONST(dst_ptr_type, dst_vulkan_object_type, src_typed_ptr)

#define VULKAN_OBJECT_TYPE_CAST(dst_ptr_type, vulkan_object_type, src_typed_ptr) OBJECT_TYPE_CAST(dst_ptr_type, CAST_TO(object_type_t, vulkan_object_type), src_typed_ptr)
#define VULKAN_OBJECT_TYPE_CONST_CAST(dst_ptr_type, vulkan_object_type, src_typed_ptr) OBJECT_TYPE_CONST_CAST(dst_ptr_type, CAST_TO(object_type_t, vulkan_object_type), src_typed_ptr)
#define VULKAN_OBJECT_TYPE_CHECK(type_ptr, vulkan_object_type) OBJECT_TYPE_CHECK(type_ptr, CAST_TO(object_type_t, vulkan_object_type))
#define VULKAN_OBJECT_TYPE_CHECK_FORWARD(typed_ptr, vulkan_object_type) OBJECT_TYPE_CHECK_FORWARD(typed_ptr, CAST_TO(object_type_t, vulkan_object_type))
#define VULKAN_OBJECT_TYPE_CHECK_CONST_FORWARD(typed_ptr, vulkan_object_type) OBJECT_TYPE_CHECK_CONST_FORWARD(typed_ptr, CAST_TO(object_type_t, vulkan_object_type))

#define VULKAN_OBJECT_TYPE_CAST_CONST(dst_ptr_type, vulkan_object_type, src_typed_ptr) OBJECT_TYPE_CONST_CAST(dst_ptr_type, CAST_TO(object_type_t, vulkan_object_type), src_typed_ptr)

#define VULKAN_OBJECT_VERIFY(typed_ptr) OBJECT_VERIFY(typed_ptr)
#define VULKAN_OBJECT_VERIFY_FORWARD(typed_ptr) OBJECT_VERIFY_FORWARD(typed_ptr)

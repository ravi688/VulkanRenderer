/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_resource_description.h is a part of VulkanRenderer

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

#include <sge/defines.h>
#include <sge/struct_descriptor.h>
#include <glslcommon/glsl_types.h>
#include <bufferlib/buffer.h>

typedef struct vulkan_shader_resource_description_t
{
	struct_descriptor_t handle;
	bool is_push_constant;						// is this a push constant ?
	struct
	{
		bool is_per_vertex_attribute; 		// does this represents per vertex attribute ?
		bool is_per_instance_attribute; 	// does this represents per instance attribute ?
	};
	bool is_opaque;								// is this an opaque handle to a resource ?
	bool is_uniform;							// is this an uniform resource ?
	union
	{
		u8 binding_number; 						// if this is a descriptor then what is the binding number ?
		u8 vertex_attrib_location_number;		// if this is an attribute then what is the location number ?
	};
	union
	{
		u8 set_number; 							// if this is a descriptor then what is the set number ?
		u8 vertex_attrib_binding_number;		// if this is an attribute then what is the binding number ?
		u8 push_constant_range_offset; 			// if this is a push constant then what is the range offset ?
	};
	//NOTE: These bit descriptions are based on the vulkan_shader_stage_t and vulkan_shader_stage_t enums
	u8 stage_flags; 	// BIT(0) = vertex shader, BIT(1) = fragment shader, BIT(2) = geometry shader, BIT(3) = tessellation shader
} vulkan_shader_resource_description_t;

typedef vulkan_shader_resource_description_t* vulkan_shader_resource_description_ptr_t;

SGE_API void vulkan_shader_resource_description_create_vertex_attribute(memory_allocator_t* allocator, vulkan_shader_resource_description_t* description, const char* name, glsl_type_t type, u32 location_number, u32 binding_binding);

SGE_API void vulkan_shader_resource_description_create_opaque(memory_allocator_t* allocator,  vulkan_shader_resource_description_t* description, const char* name, glsl_type_t type, u32 set_number, u32 binding_number);

SGE_API struct_descriptor_t* vulkan_shader_resource_description_create_uniform(memory_allocator_t* allocator, vulkan_shader_resource_description_t* description, const char* name, u32 set_number, u32 binding_number);

SGE_API void vulkan_shader_resource_description_end_uniform(memory_allocator_t* allocator, vulkan_shader_resource_description_t* description);

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void create_vertex_attribute(memory_allocator_t* allocator, BUFFER* list, const char* name, glsl_type_t type, u32 location_number, u32 binding_number)
{
	vulkan_shader_resource_description_create_vertex_attribute(allocator, buf_create_element(list), name, type, location_number, binding_number);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void create_opaque(memory_allocator_t* allocator, BUFFER* list, const char* name, u32 type, u32 set_number, u32 binding_number)
{
	vulkan_shader_resource_description_create_opaque(allocator, buf_create_element(list), name, type, set_number, binding_number);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE struct_descriptor_t* create_uniform(memory_allocator_t* allocator, BUFFER* list, const char* name, u32 set_number, u32 binding_number)
{
	return vulkan_shader_resource_description_create_uniform(allocator, buf_create_element(list), name, set_number, binding_number);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void end_uniform(memory_allocator_t* allocator, BUFFER* list)
{
	vulkan_shader_resource_description_end_uniform(allocator, buf_peek_ptr(list));
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_shader_resource_description_is_attribute(vulkan_shader_resource_description_t* description)
{
	return description->is_per_vertex_attribute || description->is_per_instance_attribute;
}

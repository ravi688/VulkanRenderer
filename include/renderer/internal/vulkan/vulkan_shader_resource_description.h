/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects
	such as toolchains written by the same author) is a software to render
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_resource_description.h is a part of VulkanRenderer

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

#include <renderer/defines.h>
#include <renderer/struct_descriptor.h>
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


RENDERER_API void vulkan_shader_resource_description_add_opaque(vulkan_renderer_t* renderer,  vulkan_shader_resource_description_t* description, const char* name, glsl_type_t type, u32 set_number, u32 binding_number);

RENDERER_API struct_descriptor_t* vulkan_shader_resource_description_begin_uniform(vulkan_renderer_t* renderer, vulkan_shader_resource_description_t* description, const char* name, u32 set_number, u32 binding_number);

RENDERER_API void vulkan_shader_resource_description_end_uniform(vulkan_renderer_t* renderer, vulkan_shader_resource_description_t* description);

static INLINE_IF_RELEASE_MODE void add_opaque(vulkan_renderer_t* renderer, BUFFER* list, const char* name, u32 type, u32 set_number, u32 binding_number)
{
	vulkan_shader_resource_description_add_opaque(renderer, buf_create_element(list), name, type, set_number, binding_number);
}

static INLINE_IF_RELEASE_MODE struct_descriptor_t* begin_uniform(vulkan_renderer_t* renderer, BUFFER* list, const char* name, u32 set_number, u32 binding_number)
{
	return vulkan_shader_resource_description_begin_uniform(renderer, buf_create_element(list), name, set_number, binding_number);
}

static INLINE_IF_RELEASE_MODE void end_uniform(vulkan_renderer_t* renderer, BUFFER* list)
{
	vulkan_shader_resource_description_end_uniform(renderer, buf_peek_ptr(list));
}

static INLINE_IF_RELEASE_MODE bool vulkan_shader_resource_description_is_attribute(vulkan_shader_resource_description_t* description)
{
	return description->is_per_vertex_attribute || description->is_per_instance_attribute;
}

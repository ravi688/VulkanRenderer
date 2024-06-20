/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_resource_description.c is a part of VulkanRenderer

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


#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_types.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/glsl_memory_layout.h>

RENDERER_API void vulkan_shader_resource_description_create_vertex_attribute(memory_allocator_t* allocator, vulkan_shader_resource_description_t* description, const char* name, glsl_type_t type, u32 location_number, u32 binding_number)
{
	description->vertex_attrib_location_number = location_number;
	description->vertex_attrib_binding_number = binding_number;
	// NOTE: shader stage must be FRAGMENT for subpassInput
	description->stage_flags = (1 << VULKAN_SHADER_TYPE_FRAGMENT); //| (1 << VULKAN_SHADER_TYPE_VERTEX);

	OBJECT_INIT(&description->handle, OBJECT_TYPE_STRUCT_DESCRIPTOR, OBJECT_NATIONALITY_EXTERNAL);
	struct_descriptor_begin(allocator, &description->handle, name, type, GLSL_MEMORY_LAYOUT_CALLBACKS);
	struct_descriptor_end(allocator, &description->handle);
}

RENDERER_API void vulkan_shader_resource_description_create_opaque(memory_allocator_t* allocator,  vulkan_shader_resource_description_t* description, const char* name, glsl_type_t type, u32 set_number, u32 binding_number)
{
	description->is_opaque = true;
	description->set_number = set_number;
	description->binding_number = binding_number;
	// NOTE: shader stage must be FRAGMENT for subpassInput
	description->stage_flags = (1 << VULKAN_SHADER_TYPE_FRAGMENT); //| (1 << VULKAN_SHADER_TYPE_VERTEX);

	OBJECT_INIT(&description->handle, OBJECT_TYPE_STRUCT_DESCRIPTOR, OBJECT_NATIONALITY_EXTERNAL);
	struct_descriptor_begin(allocator, &description->handle, name, type, GLSL_MEMORY_LAYOUT_CALLBACKS);
	struct_descriptor_end(allocator, &description->handle);
}

RENDERER_API struct_descriptor_t* vulkan_shader_resource_description_create_uniform(memory_allocator_t* allocator,  vulkan_shader_resource_description_t* description, const char* name, u32 set_number, u32 binding_number)
{
	description->is_uniform = true;
	description->set_number = set_number;
	description->binding_number = binding_number;
	description->stage_flags = (1 << VULKAN_SHADER_TYPE_VERTEX) | (1 << VULKAN_SHADER_TYPE_FRAGMENT);

	OBJECT_INIT(&description->handle, OBJECT_TYPE_STRUCT_DESCRIPTOR, OBJECT_NATIONALITY_EXTERNAL);
	struct_descriptor_begin(allocator, &description->handle, name, GLSL_TYPE_UNIFORM_BUFFER, GLSL_MEMORY_LAYOUT_CALLBACKS);
	return &description->handle;
}

RENDERER_API void vulkan_shader_resource_description_end_uniform(memory_allocator_t* allocator, vulkan_shader_resource_description_t* description)
{
	struct_descriptor_end(allocator, &description->handle);
}


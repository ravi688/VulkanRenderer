/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_resource_description_builder.h is a part of VulkanRenderer

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

#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <bufferlib/buffer.h>  // buffer_t


typedef struct vulkan_shader_resource_description_builder_t
{
	memory_allocator_t* allocator;
	/* array of vulkan_shader_resource_description_t objects */
	buffer_t description_array;
	u32 bind_index;
} vulkan_shader_resource_description_builder_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_shader_resource_description_builder_t* vulkan_shader_resource_description_builder_create(memory_allocator_t* allocator);
RENDERER_API void vulkan_shader_resource_description_builder_destroy(vulkan_shader_resource_description_builder_t* builder);

RENDERER_API void vulkan_shader_resource_description_builder_add(vulkan_shader_resource_description_builder_t* builder, u32 count);
RENDERER_API void vulkan_shader_resource_description_builder_bind(vulkan_shader_resource_description_builder_t* builder, u32 index);

RENDERER_API vulkan_shader_resource_description_t* vulkan_shader_resource_description_builder_get(vulkan_shader_resource_description_builder_t* builder);
RENDERER_API u32 vulkan_shader_resource_description_builder_get_count(vulkan_shader_resource_description_builder_t* builder);

RENDERER_API void vulkan_shader_resource_description_builder_create_opaque(vulkan_shader_resource_description_builder_t* builder, const char* name, glsl_type_t type, u32 set_number, u32 binding_number);
RENDERER_API struct_descriptor_t* vulkan_shader_resource_description_builder_create_uniform(vulkan_shader_resource_description_builder_t* builder, const char* name, u32 set_number, u32 binding_number);
RENDERER_API void vulkan_shader_resource_description_builder_end_uniform(vulkan_shader_resource_description_builder_t* builder);

END_CPP_COMPATIBLE

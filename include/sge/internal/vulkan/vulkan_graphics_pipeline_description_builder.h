/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_graphics_pipeline_description_builder.h is a part of VulkanRenderer

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
#include <vulkan/vulkan.h> // VkBool32
#include <sge/internal/vulkan/vulkan_types.h> // vulkan_shader_type_t
#include <bufferlib/buffer.h>  // buffer_t

typedef struct vulkan_graphics_pipeline_description_t vulkan_graphics_pipeline_description_t;

typedef struct vulkan_graphics_pipeline_description_builder_t
{
	memory_allocator_t* allocator;
	/* array of vulkan_graphics_pipeline_description_t objects */
	buffer_t description_array;
	u32 bind_index;
} vulkan_graphics_pipeline_description_builder_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SGE_API vulkan_graphics_pipeline_description_builder_t* vulkan_graphics_pipeline_description_builder_create(memory_allocator_t* allocator);
SGE_API vulkan_graphics_pipeline_description_builder_t* vulkan_graphics_pipeline_description_builder_create_inverse(memory_allocator_t* allocator, vulkan_graphics_pipeline_description_t* descriptions, u32 description_count);
SGE_API void vulkan_graphics_pipeline_description_builder_destroy(vulkan_graphics_pipeline_description_builder_t* builder);

SGE_API void vulkan_graphics_pipeline_description_builder_add(vulkan_graphics_pipeline_description_builder_t* builder, u32 count);
SGE_API void vulkan_graphics_pipeline_description_builder_bind(vulkan_graphics_pipeline_description_builder_t* builder, u32 index);

SGE_API vulkan_graphics_pipeline_description_t* vulkan_graphics_pipeline_description_builder_get(vulkan_graphics_pipeline_description_builder_t* builder);
SGE_API u32 vulkan_graphics_pipeline_description_builder_get_count(vulkan_graphics_pipeline_description_builder_t* builder);

SGE_API void vulkan_graphics_pipeline_description_builder_begin_pipeline(vulkan_graphics_pipeline_description_builder_t* builder);
SGE_API void vulkan_graphics_pipeline_description_builder_add_color_blend_state(vulkan_graphics_pipeline_description_builder_t* builder, VkBool32 is_blend_enable);
SGE_API void vulkan_graphics_pipeline_description_builder_add_shader(vulkan_graphics_pipeline_description_builder_t* builder, const char* file_path, vulkan_shader_type_t shader_type);
SGE_API void vulkan_graphics_pipeline_description_builder_set_depth_stencil(vulkan_graphics_pipeline_description_builder_t* builder, VkBool32 is_depth_write, VkBool32 is_depth_test);
SGE_API void vulkan_graphics_pipeline_description_builder_end_pipeline(vulkan_graphics_pipeline_description_builder_t* builder);

END_CPP_COMPATIBLE

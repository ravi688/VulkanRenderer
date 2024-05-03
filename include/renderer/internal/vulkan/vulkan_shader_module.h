/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_module.h is a part of VulkanRenderer

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

#include <vulkan/vulkan.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_types.h>
#include <renderer/internal/vulkan/vulkan_object.h>

typedef struct vulkan_shader_module_create_info_t
{
	/* ptr the spirv binary */
	void* spirv;
	/* length of the spirv binary */
	u32 length;
	/* type of the shader*/
	vulkan_shader_type_t type;

} vulkan_shader_module_create_info_t;

typedef struct vulkan_shader_module_load_info_t
{
	/* file path to load the spirv from */
	const char* file_path;
	/* type of the shader */
	vulkan_shader_type_t type;
	
} vulkan_shader_module_load_info_t;

/* vulkan_stage_shader_t renamed to vulkan_shader_module_t */
typedef struct vulkan_shader_module_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	VkShaderModule vo_module;
	VkPipelineShaderStageCreateInfo vo_stage;
} vulkan_shader_module_t;

#define VULKAN_SHADER_MODULE(typed_ptr) VULKAN_OBJECT_UP_CAST(vulkan_shader_module_t*, VULKAN_OBJECT_TYPE_SHADER_MODULE, typed_ptr)
#define VULKAN_SHADER_MODULE_CONST(typed_ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_shader_module_t*, VULKAN_OBJECT_TYPE_SHADER_MODULE, typed_ptr)

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_shader_module_t* vulkan_shader_module_new(memory_allocator_t* allocator);
RENDERER_API vulkan_shader_module_t* vulkan_shader_module_create(vulkan_renderer_t* renderer, vulkan_shader_module_create_info_t* create_info);
RENDERER_API void vulkan_shader_module_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_module_create_info_t* create_info, vulkan_shader_module_t OUT shader);
RENDERER_API vulkan_shader_module_t* vulkan_shader_module_load(vulkan_renderer_t* renderer, vulkan_shader_module_load_info_t* load_info);
RENDERER_API void vulkan_shader_module_load_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_module_load_info_t* load_info, vulkan_shader_module_t OUT shader);
RENDERER_API void vulkan_shader_module_destroy(vulkan_shader_module_t* shader);
RENDERER_API void vulkan_shader_module_release_resources(vulkan_shader_module_t* shader);

END_CPP_COMPATIBLE

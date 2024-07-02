/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_module.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_shader_module.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <disk_manager/file_reader.h>

#include <renderer/assert.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

static VkShaderModule get_shader_module(vulkan_renderer_t* renderer, void* spirv, u32 length);
static VkPipelineShaderStageCreateInfo get_pipeline_shader_stage_create_info(VkShaderModule shader_module, vulkan_shader_type_t vulkan_shader_type, const char* entry_point);


SGE_API vulkan_shader_module_t* vulkan_shader_module_new(memory_allocator_t* allocator)
{
	vulkan_shader_module_t* shader = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_MODULE, vulkan_shader_module_t);
	memzero(shader, vulkan_shader_module_t);
	VULKAN_OBJECT_INIT(shader, VULKAN_OBJECT_TYPE_SHADER_MODULE, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return shader;
}

SGE_API vulkan_shader_module_t* vulkan_shader_module_create(vulkan_renderer_t* renderer, vulkan_shader_module_create_info_t* create_info)
{
	vulkan_shader_module_t* shader = vulkan_shader_module_new(renderer->allocator);
	vulkan_shader_module_create_no_alloc(renderer, create_info, shader);
	return shader;
}

SGE_API void vulkan_shader_module_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_module_create_info_t* create_info, vulkan_shader_module_t* shader)
{
	VULKAN_OBJECT_MEMZERO(shader, vulkan_shader_module_t);

	shader->renderer = renderer;
	shader->vo_module = get_shader_module(renderer, create_info->spirv, create_info->length);
	shader->vo_stage = get_pipeline_shader_stage_create_info(shader->vo_module, create_info->type, "main");
}

SGE_API void vulkan_shader_module_load_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_module_load_info_t* load_info, vulkan_shader_module_t OUT shader)
{
	VULKAN_OBJECT_MEMZERO(shader, vulkan_shader_module_t);
	shader->renderer = renderer;
	BUFFER* shader_bytes = load_binary_from_file(load_info->file_path);
	shader->vo_module = get_shader_module(renderer, shader_bytes->bytes, shader_bytes->element_count);
	shader->vo_stage = get_pipeline_shader_stage_create_info(shader->vo_module, load_info->type, "main");
	buf_free(shader_bytes);
}

SGE_API vulkan_shader_module_t* vulkan_shader_module_load(vulkan_renderer_t* renderer, vulkan_shader_module_load_info_t* load_info)
{
	vulkan_shader_module_t* shader = vulkan_shader_module_new(renderer->allocator);
	vulkan_shader_module_load_no_alloc(renderer, load_info, shader);
	return shader;
}

SGE_API void vulkan_shader_module_destroy(vulkan_shader_module_t* shader)
{
	vkDestroyShaderModule(shader->renderer->logical_device->vo_handle, shader->vo_module, VULKAN_ALLOCATION_CALLBACKS(shader->renderer));
}

SGE_API void vulkan_shader_module_release_resources(vulkan_shader_module_t* shader)
{
	if(VULKAN_OBJECT_IS_INTERNAL(shader))
		memory_allocator_dealloc(shader->renderer->allocator, shader);
}

 static VkShaderModule get_shader_module(vulkan_renderer_t* renderer, void* spirv, u32 length)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = length;
	createInfo.pCode = spirv;
	VkShaderModule shader_module;
	vkCall(vkCreateShaderModule(renderer->logical_device->vo_handle, &createInfo, VULKAN_ALLOCATION_CALLBACKS(renderer), &shader_module));
	return shader_module;
}

static VkPipelineShaderStageCreateInfo get_pipeline_shader_stage_create_info(VkShaderModule shader_module, vulkan_shader_type_t vulkan_shader_type, const char* entry_point)
{
	VkPipelineShaderStageCreateInfo createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.pName = entry_point; 
	createInfo.module = shader_module; 
	switch(vulkan_shader_type)
	{
		case VULKAN_SHADER_TYPE_VERTEX:
			createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case VULKAN_SHADER_TYPE_FRAGMENT:
			createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case VULKAN_SHADER_TYPE_GEOMETRY:
			createInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			break;
		case VULKAN_SHADER_TYPE_TESSELLATION:
			createInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			break;
		default:
			LOG_FETAL_ERR("Unsupported shader type");
			break;
	}
	return createInfo;
}

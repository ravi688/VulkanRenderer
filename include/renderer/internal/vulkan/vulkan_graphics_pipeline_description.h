/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects
	such as toolchains written by the same author) is a software to render
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_graphics_pipeline_description.h is a part of VulkanRenderer

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
#include <vulkan/vulkan.h>
#include <renderer/internal/vulkan/vulkan_types.h> // vulkan_shader_type_t
#include <bufferlib/buffer.h>

/* NOTE: this must be in sync with vulkan_shader_module_create_info_t */
typedef struct vulkan_spirv_code_t
{
	/* ptr to the SPIRV binary */
	void* spirv;
	/* length of the SPIRV binary in bytes */
	u32 length;
	/* type of the shader module */
	vulkan_shader_type_t type;

} vulkan_spirv_code_t;

typedef struct vulkan_graphics_pipeline_settings_t
{
	VkPipelineInputAssemblyStateCreateInfo	inputassembly;
	VkPipelineTessellationStateCreateInfo	tessellation;
	VkPipelineViewportStateCreateInfo		viewport;
	VkPipelineRasterizationStateCreateInfo	rasterization;
	VkPipelineMultisampleStateCreateInfo	multisample;
	VkPipelineDepthStencilStateCreateInfo	depthstencil;
	VkPipelineColorBlendStateCreateInfo		colorblend;
	VkPipelineDynamicStateCreateInfo		dynamic;
} vulkan_graphics_pipeline_settings_t;

typedef struct vulkan_graphics_pipeline_description_t
{
	/* fixed function settings for this graphics pipeline */
	vulkan_graphics_pipeline_settings_t* settings;

	/* list of spirv shaders (just bytes and lengths) */
	vulkan_spirv_code_t* spirv_codes;
	u32 spirv_code_count;

} vulkan_graphics_pipeline_description_t;

#define VULKAN_GRAPHICS_PIPELINE_DESCRIPTION(ptr) CAST_TO(vulkan_graphics_pipeline_description_t*, ptr)

RENDERER_API void vulkan_graphics_pipeline_description_begin(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_description_t* description);
RENDERER_API void vulkan_graphics_pipeline_description_add_color_blend_state(vulkan_graphics_pipeline_description_t* description, VkBool32 blendEnable);
RENDERER_API void vulkan_graphics_pipeline_description_set_depth_stencil(vulkan_graphics_pipeline_description_t* description, VkBool32 depthWrite, VkBool32 depthTest);
RENDERER_API void vulkan_graphics_pipeline_description_set_depth_bias(vulkan_graphics_pipeline_description_t* description, float factor, float clamp, float slope_factor);
RENDERER_API void vulkan_graphics_pipeline_description_add_shader(vulkan_graphics_pipeline_description_t* description, const char* file_path, vulkan_shader_type_t type);
RENDERER_API void vulkan_graphics_pipeline_description_end(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_description_t* description);

RENDERER_API VkPipelineRasterizationStateCreateInfo* vulkan_graphics_pipeline_description_get_rasterization(vulkan_graphics_pipeline_description_t* description);

static VkPipelineRasterizationStateCreateInfo* get_rasterization(BUFFER* list)
{
	return vulkan_graphics_pipeline_description_get_rasterization(VULKAN_GRAPHICS_PIPELINE_DESCRIPTION(buf_peek_ptr(list)));
}

static void begin_pipeline(vulkan_renderer_t* renderer, BUFFER* list)
{
	vulkan_graphics_pipeline_description_begin(renderer, buf_create_element(list));
}

static void add_color_blend_state(BUFFER* list, VkBool32 blendEnable)
{
	vulkan_graphics_pipeline_description_add_color_blend_state(buf_peek_ptr(list), blendEnable);
}

static void set_depth_stencil(BUFFER* list, VkBool32 depthWrite, VkBool32 depthTest)
{
	vulkan_graphics_pipeline_description_set_depth_stencil(buf_peek_ptr(list), depthWrite, depthTest);
}

static void set_depth_bias(BUFFER* list, float factor, float clamp, float slope_factor)
{
	vulkan_graphics_pipeline_description_set_depth_bias(buf_peek_ptr(list), factor, clamp, slope_factor);
}

static void add_shader(BUFFER* list, const char* file_path, vulkan_shader_type_t type)
{
	vulkan_graphics_pipeline_description_add_shader(buf_peek_ptr(list), file_path, type);
}

static void end_pipeline(vulkan_renderer_t* renderer, BUFFER* list)
{
	vulkan_graphics_pipeline_description_end(renderer, buf_peek_ptr(list));
}

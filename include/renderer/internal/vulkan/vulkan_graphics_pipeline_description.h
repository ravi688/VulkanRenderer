
#pragma once

#include <renderer/defines.h>
#include <vulkan/vulkan.h>
#include <renderer/internal/vulkan/vulkan_types.h> // vulkan_shader_type_t

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


RENDERER_API void vulkan_graphics_pipeline_description_begin(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_description_t* description);
RENDERER_API void vulkan_graphics_pipeline_description_add_color_blend_state(vulkan_graphics_pipeline_description_t* description, VkBool32 blendEnable);
RENDERER_API void vulkan_graphics_pipeline_description_set_depth_stencil(vulkan_graphics_pipeline_description_t* description, VkBool32 depthWrite, VkBool32 depthTest);
RENDERER_API void vulkan_graphics_pipeline_description_set_depth_bias(vulkan_graphics_pipeline_description_t* description, float factor, float clamp, float slope_factor);
RENDERER_API void vulkan_graphics_pipeline_description_add_shader(vulkan_graphics_pipeline_description_t* description, const char* file_path, vulkan_shader_type_t type);
RENDERER_API void vulkan_graphics_pipeline_description_end(vulkan_graphics_pipeline_description_t* description);

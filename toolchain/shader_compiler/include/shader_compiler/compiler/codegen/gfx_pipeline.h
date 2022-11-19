#pragma once

#include <shader_compiler/compiler/compiler.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h>
#include <vulkan/vulkan_core.h>

/* graphics pipeline within a subpass definition
   i.e. GraphicsPipeline { ... }
 */
typedef struct gfx_pipeline_t
{
	VkPipelineInputAssemblyStateCreateInfo	inputassembly;
	VkPipelineTessellationStateCreateInfo	tessellation;
	VkPipelineViewportStateCreateInfo		viewport;
	VkPipelineRasterizationStateCreateInfo	rasterization;
	VkPipelineMultisampleStateCreateInfo	multisample;
	VkPipelineDepthStencilStateCreateInfo	depthstencil;
	VkPipelineColorBlendStateCreateInfo		colorblend;
	VkPipelineDynamicStateCreateInfo		dynamic;
} gfx_pipeline_t;

SC_API void write_gfx_pipeline(const char* start, const char* const end, codegen_buffer_t* buffer);

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

BEGIN_CPP_COMPATIBLE

/*	parses the string pointed by 'start', ends at 'end', and writes "GraphicsPipeline" block to the codegen_buffer_t object 
	start: pointer to the stsart of the string
	end: pointer to the end of the string (must point to null character)
	buffer: codgen buffer to which write gfx_pipeline_t object and its supplmentary data

	NOTE: the binary data is written in the following format (order):
		1. gfx_pipeline_t object	--> sizeof(gfx_pipeline_t) bytes
		2. VkPipelineColorBlendAttachmentState[] --> sizeof(VkPipelineColorBlendAttachmentState) * colorblend.attachmentCount
		3. VkRect2D[] --> sizeof(VkRect2D) * viewport.scissorCount
		4. VkViewport[] --> sizeof(VkViewport) * viewport.viewportCount
	see: //Documents/V3D/V3DShaderBinarySpec.docx

	NOTE: the expected string should in the format:
	-------------------------
	depthStencil { ... }
	colorBlend { ... }
	-------------------------
 */
SC_API void write_gfx_pipeline(const char* start, const char* const end, codegen_buffer_t* buffer);

END_CPP_COMPATIBLE

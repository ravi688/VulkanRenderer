
#pragma once

#include <calltrace/calltrace.h>
#include <bufferlib/buffer.h>
#include <shader_compiler/defines.h>
#include <vulkan/vulkan_core.h>

typedef struct GraphicsPipeline
{
	VkPipelineInputAssemblyStateCreateInfo	inputassembly;
	VkPipelineTessellationStateCreateInfo	tessellation;
	VkPipelineViewportStateCreateInfo		viewport;
	VkPipelineRasterizationStateCreateInfo	rasterization;
	VkPipelineMultisampleStateCreateInfo	multisample;
	VkPipelineDepthStencilStateCreateInfo	depthstencil;
	VkPipelineColorBlendStateCreateInfo		colorblend;
	VkPipelineDynamicStateCreateInfo		dynamic;
} GraphicsPipeline;

#define parse_settings(...) define_alias_function_macro(parse_settings, __VA_ARGS__)
function_signature(const char*, parse_settings, const char* _source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer);

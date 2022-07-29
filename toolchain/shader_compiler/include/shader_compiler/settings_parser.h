
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

typedef struct Properties
{
	bool castShadow;				// true when the object is able to cast shadow
	bool receiveShadow;				// true when the object is able to receive shadow
} Properties;

// typedef struct Settings
// {
// 	GraphicsPipeline graphicsPipeline;  // settings for the vulkan graphics pipeline
// 	Properties properties;				// high level configuration/properties of the material
// } Settings;

#define parse_settings(...) define_alias_function_macro(parse_settings, __VA_ARGS__)
function_signature(const char*, parse_settings, const char* _source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer);

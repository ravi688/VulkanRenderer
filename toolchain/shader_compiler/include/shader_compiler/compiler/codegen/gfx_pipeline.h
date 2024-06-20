/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: gfx_pipeline.h is a part of VulkanRenderer

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

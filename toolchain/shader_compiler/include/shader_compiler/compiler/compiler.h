#pragma once

#include <shader_compiler/defines.h>
#include <bufferlib/buffer.h>
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

/* compiler command category
   i.e. #sb version 2022
*/
typedef enum cmplr_cmd_cat_t
{
	/* command is related to shader binary generation */
	CMPLR_CMD_CAT_SB = 1UL,
	/* command is related to shader language compilation */
	CMPLR_CMD_CAT_SL
} cmplr_cmd_cat_t;

/* shader binary version
   i.e #sb version 2022
 */
typedef enum sb_version_t
{
	SB_VERSION_2021 = 2021UL,
	SB_VERSION_2022
} sb_version_t;

/* shader language version
   i.e #sl version 2022
*/
typedef enum sl_version_t
{
	SL_VERSION_2021 = 2021UL,
	SL_VERSION_2022
} sl_version_t;

// shader binary header
typedef struct sb_header_t
{
	// file identification header for shader binary
	const char* header_str;

	// shader language version
	sl_version_t sl_version;

	// shader binary version
	sb_version_t sb_version;
} sb_header_t;


/*
	Compiles given source code (string) of length 'length' (without null character)
	into shader binary and returns a BUFFER object ptr containing the compiled shader
	binary data.
 */
SC_API BUFFER* sc_compile(const char* source, u32 length);

/*
	Similar to sc_compile(...) but it also loads a given file from 'file_path' into the
	memory and calls sc_compile internally after that.
 */
SC_API BUFFER* sc_load_and_compile(const char* file_path);

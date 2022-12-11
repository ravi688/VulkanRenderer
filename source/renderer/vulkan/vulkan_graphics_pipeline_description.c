
#include <renderer/internal/vulkan/vulkan_graphics_pipeline_description.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>
#include <bufferlib/buffer.h>
#include <disk_manager/file_reader.h>

#define create_buffer(type) __create_buffer(sizeof(type))
static BUFFER* __create_buffer(u32 size)
{
	BUFFER* buffer = heap_new(BUFFER);
	*buffer = buf_create(size, 1, 0);
	return buffer;
}

RENDERER_API void vulkan_graphics_pipeline_description_begin(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_description_t* description)
{
	vulkan_graphics_pipeline_settings_t* settings = heap_new(vulkan_graphics_pipeline_settings_t);
	memzero(settings, vulkan_graphics_pipeline_settings_t);
	description->settings = settings;

	description->spirv_codes = CAST_TO(vulkan_spirv_code_t*, create_buffer(vulkan_spirv_code_t));

	settings->colorblend = (VkPipelineColorBlendStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.pAttachments = CAST_TO(VkPipelineColorBlendAttachmentState*, create_buffer(VkPipelineColorBlendAttachmentState))
	};

	settings->inputassembly = (VkPipelineInputAssemblyStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	settings->tessellation = (VkPipelineTessellationStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO
	};

	VkViewport* viewport = heap_new(VkViewport);
	viewport[0] = (VkViewport)
	{
		.x = 0,
		.y = 0,
		.width = renderer->swapchain->vo_image_extent.width,
		.height = renderer->swapchain->vo_image_extent.height,
		.minDepth = 0,
		.maxDepth = 1.0f
	};
	VkRect2D* scissor = heap_new(VkRect2D);
	scissor[0] = (VkRect2D)
	{
		.offset = { 0, 0 },
		.extent = renderer->swapchain->vo_image_extent
	};
	settings->viewport = (VkPipelineViewportStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = viewport,
		.scissorCount = 1,
		.pScissors = scissor
	};
	settings->rasterization = (VkPipelineRasterizationStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.lineWidth = 1.0f
	};
	settings->multisample = (VkPipelineMultisampleStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE
	};
	settings->depthstencil = (VkPipelineDepthStencilStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_FALSE,
		.depthWriteEnable = VK_FALSE,
		.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
		.stencilTestEnable = VK_FALSE,
		.depthBoundsTestEnable = VK_FALSE
	};

	settings->dynamic = (VkPipelineDynamicStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO
	};
}

RENDERER_API void vulkan_graphics_pipeline_description_add_color_blend_state(vulkan_graphics_pipeline_description_t* description, VkBool32 blendEnable)
{
	VkPipelineColorBlendAttachmentState state =
	{
		.blendEnable = blendEnable,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE	
	};
	VkPipelineColorBlendStateCreateInfo* info = &description->settings->colorblend;
	buf_push(CAST_TO(BUFFER*, info->pAttachments), &state);
}

RENDERER_API void vulkan_graphics_pipeline_description_set_depth_stencil(vulkan_graphics_pipeline_description_t* description, VkBool32 depthWrite, VkBool32 depthTest)
{
	VkPipelineDepthStencilStateCreateInfo* info = &description->settings->depthstencil;
	info->depthWriteEnable = depthWrite;
	info->depthTestEnable = depthTest;
}

RENDERER_API void vulkan_graphics_pipeline_description_set_depth_bias(vulkan_graphics_pipeline_description_t* description, float factor, float clamp, float slope_factor)
{
	VkPipelineRasterizationStateCreateInfo* info = &description->settings->rasterization;
	if(info->depthBiasEnable == VK_FALSE)
		info->depthBiasEnable = VK_TRUE;
	info->depthBiasConstantFactor = factor;
	info->depthBiasClamp = clamp;
	info->depthBiasSlopeFactor = slope_factor;
}

RENDERER_API void vulkan_graphics_pipeline_description_add_shader(vulkan_graphics_pipeline_description_t* description, const char* file_path, vulkan_shader_type_t type)
{
	BUFFER* data = load_binary_from_file(file_path);
	vulkan_spirv_code_t code = 
	{
		.type = type,
		.spirv = data->bytes,
		.length = data->element_count
	};
	buf_push(CAST_TO(BUFFER*, description->spirv_codes), &code);
}

RENDERER_API void vulkan_graphics_pipeline_description_end(vulkan_graphics_pipeline_description_t* description)
{
	VkPipelineColorBlendStateCreateInfo* info = &description->settings->colorblend;
	BUFFER* buffer = CAST_TO(BUFFER*, info->pAttachments);
	info->attachmentCount = buf_get_element_count(buffer);
	info->pAttachments = CAST_TO(VkPipelineColorBlendAttachmentState*, buf_get_ptr(buffer));
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, description->spirv_codes);
	description->spirv_code_count = buf_get_element_count(buffer);
	description->spirv_codes = buf_get_ptr(buffer);
	heap_free(buffer);
}

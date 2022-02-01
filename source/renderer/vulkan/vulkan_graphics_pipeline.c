
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/render_window.h>

#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>

vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_new()
{
	vulkan_graphics_pipeline_t* pipeline = heap_new(vulkan_graphics_pipeline_t);
	memset(pipeline, 0, sizeof(vulkan_graphics_pipeline_t));
	pipeline->pipeline_layout = vulkan_pipeline_layout_new();
	return pipeline;
}

void vulkan_graphics_pipeline_create_no_alloc(renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info, vulkan_graphics_pipeline_t* pipeline)
{
	ASSERT(renderer->logical_device->handle != VK_NULL_HANDLE, "renderer->logical_device->handle == VK_NULL_HANDLE\n");
	ASSERT(renderer->vk_render_pass != VK_NULL_HANDLE, "renderer->vk_render_pass == VK_NULL_HANDLE\n");
	ASSERT(renderer->window != NULL, "renderer->window == NULL\n");

	vulkan_pipeline_layout_create_info_t layout_create_info = 
	{ 
		.vk_set_layout = create_info->shader->vk_set_layout,
		.push_constant_ranges = create_info->push_constant_ranges,
		.push_constant_range_count = create_info->push_constant_range_count
	};
	vulkan_pipeline_layout_create_no_alloc(renderer, &layout_create_info, pipeline->pipeline_layout);

	VkPipelineShaderStageCreateInfo* shader_stages = stack_newv(VkPipelineShaderStageCreateInfo, create_info->shader->stage_count);
	for(u32 i = 0; i < create_info->shader->stage_count; i++)
		ref(VkPipelineShaderStageCreateInfo, shader_stages, i) = (*refp(vulkan_stage_shader_t*, create_info->shader->stage_shaders, i))->stage;

	vertex_attribute_binding_info_t* vertex_attribute_infos = stack_alloc(sizeof(vertex_attribute_binding_info_t) * create_info->vertex_info_count);
	memset(vertex_attribute_infos, 0, sizeof(vertex_attribute_binding_info_t) * create_info->vertex_info_count);
	uint32_t* strides = stack_alloc(sizeof(uint32_t) * create_info->vertex_info_count);
	VkVertexInputRate* input_rates = stack_newv(VkVertexInputRate, create_info->vertex_info_count);
	for(uint32_t i = 0; i < create_info->vertex_info_count; i++)
	{
		vertex_attribute_infos[i].attribute_count = create_info->vertex_infos[i].attribute_count;
		vertex_attribute_infos[i].attribute_offsets = create_info->vertex_infos[i].attribute_offsets;
		vertex_attribute_infos[i].attribute_formats = create_info->vertex_infos[i].attribute_formats;
		strides[i] = create_info->vertex_infos[i].size;
		input_rates[i] = create_info->vertex_infos[i].input_rate;
	}
	VkPipelineVertexInputStateCreateInfo vertex_info_info = vk_get_pipeline_vertex_input_state_create_info(create_info->vertex_info_count, strides, input_rates, vertex_attribute_infos);
	VkPipelineInputAssemblyStateCreateInfo input_assembly = vk_get_pipeline_input_assembly_state_create_info();
	VkPipelineRasterizationStateCreateInfo rasterizer = vk_get_pipeline_rasterization_state_create_info();
	VkPipelineViewportStateCreateInfo viewport_state = vk_get_pipeline_viewport_state_create_info(renderer->window->width, renderer->window->height);
	VkPipelineMultisampleStateCreateInfo multi_sampling = vk_get_pipeline_multisample_state_create_info();
	VkPipelineColorBlendAttachmentState color_blend_attachmentment_state;
	if(!create_info->blend_enabled)
		color_blend_attachmentment_state = vk_get_pipeline_color_blend_attachment_state();
	else 
	{
		VkPipelineColorBlendAttachmentState state = 
		{
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD
		};
		color_blend_attachmentment_state = state;
	}
	VkPipelineColorBlendStateCreateInfo color_blending = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY, // Optional
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachmentment_state,
		.blendConstants[0] = 0.0f, // Optional
		.blendConstants[1] = 0.0f, // Optional
		.blendConstants[2] = 0.0f, // Optional
		.blendConstants[3] = 0.0f // Optional
	};
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state = vk_get_pipeline_depth_stencil_state_create_info();
	pipeline->pipeline = vk_get_graphics_pipeline(	renderer->logical_device->handle, pipeline->pipeline_layout->handle, renderer->vk_render_pass,
														shader_stages,
														&vertex_info_info,
														&input_assembly,
														&viewport_state,
														&rasterizer,
														&multi_sampling,
														&color_blending,
														&depth_stencil_state
													);
	stack_free(shader_stages);
	stack_free(vertex_attribute_infos);
	stack_free(strides);
	stack_free(input_rates);
}

vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_create(renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info)
{
	vulkan_graphics_pipeline_t* pipeline = vulkan_graphics_pipeline_new();
	vulkan_graphics_pipeline_create_no_alloc(renderer, create_info, pipeline);
	return pipeline;
}


void vulkan_graphics_pipeline_bind(vulkan_graphics_pipeline_t* pipeline, renderer_t* renderer)
{
	ASSERT(renderer->logical_device->handle != VK_NULL_HANDLE, "renderer->logical_device->handle == VK_NULL_HANDLE\n");
	ASSERT(renderer->swapchain != NULL, "renderer->swapchain == NULL\n");

	u32 image_index = renderer->swapchain->current_image_index;
	vkCmdBindPipeline(renderer->vk_command_buffers.value2[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
}

void vulkan_graphics_pipeline_destroy(vulkan_graphics_pipeline_t* pipeline, renderer_t* renderer)
{
	ASSERT(renderer->logical_device->handle != VK_NULL_HANDLE, "renderer->logical_device->handle == VK_NULL_HANDLE\n");
	vulkan_pipeline_layout_destroy(pipeline->pipeline_layout, renderer);
	vkDestroyPipeline(renderer->logical_device->handle, pipeline->pipeline, NULL);
}

void vulkan_graphics_pipeline_release_resources(vulkan_graphics_pipeline_t* pipeline)
{
	vulkan_pipeline_layout_release_resources(pipeline->pipeline_layout);
	heap_free(pipeline);
}

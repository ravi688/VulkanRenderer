#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/render_window.h>

#include <renderer/assert.h>
#include <renderer/memory_allocator.h>
#include <memory.h>

RENDERER_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_new()
{
	vulkan_graphics_pipeline_t* pipeline = heap_new(vulkan_graphics_pipeline_t);
	memset(pipeline, 0, sizeof(vulkan_graphics_pipeline_t));
	pipeline->pipeline_layout = vulkan_pipeline_layout_new();
	return pipeline;
}

RENDERER_API void vulkan_graphics_pipeline_create_no_alloc(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info, vulkan_graphics_pipeline_t* pipeline)
{
	assert(renderer->logical_device->handle != VK_NULL_HANDLE);
	assert(renderer->render_pass->handle != VK_NULL_HANDLE);
	assert(renderer->window != NULL);

	// create pipeline layout
	vulkan_pipeline_layout_create_info_t layout_create_info = 
	{ 
		.vk_set_layout = create_info->shader->vk_set_layout,
		.push_constant_ranges = create_info->push_constant_ranges,
		.push_constant_range_count = create_info->push_constant_range_count
	};
	vulkan_pipeline_layout_create_no_alloc(renderer, &layout_create_info, pipeline->pipeline_layout);

	// copy the shader stage create info to a continuous array of VkPipelineShaderStageCreateInfo
	VkPipelineShaderStageCreateInfo shader_stages[create_info->shader->stage_count];
	for(u32 i = 0; i < create_info->shader->stage_count; i++)
		shader_stages[i] = create_info->shader->stage_shaders[i]->stage;

	// setup vertex input binding and descriptions
	VkVertexInputBindingDescription binding_descriptions[create_info->vertex_info_count];
	BUFFER attribute_descriptions = buf_create(sizeof(VkVertexInputAttributeDescription), 0, 0);

	for(u32 i = 0; i < create_info->vertex_info_count; i++)
	{
		binding_descriptions[i].binding = i;
		binding_descriptions[i].stride = create_info->vertex_infos[i].size;
		binding_descriptions[i].inputRate = create_info->vertex_infos[i].input_rate;

		for(u32 j = 0; j < create_info->vertex_infos[i].attribute_count; j++)
		{
			VkVertexInputAttributeDescription description =
			{
				.binding = create_info->vertex_infos[i].binding,
				.location = create_info->vertex_infos[i].attribute_locations[j],
				.format = create_info->vertex_infos[i].attribute_formats[j],
				.offset = create_info->vertex_infos[i].attribute_offsets[j]
			};
			buf_push(&attribute_descriptions, &description);
		}
	}

	// setup vertex input state
	VkPipelineVertexInputStateCreateInfo vertex_input_state = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, 
		.vertexBindingDescriptionCount = create_info->vertex_info_count, 
		.pVertexBindingDescriptions = binding_descriptions, 
		.vertexAttributeDescriptionCount = attribute_descriptions.element_count, 
		.pVertexAttributeDescriptions = attribute_descriptions.bytes
	};

	// setup input assembly state
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
		.primitiveRestartEnable = VK_FALSE
	};

	// setup rasterization state
	VkPipelineRasterizationStateCreateInfo rasterization_state = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.lineWidth = 1.0f,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f, // Optional
		.depthBiasClamp = 0.0f, // Optional
		.depthBiasSlopeFactor = 0.0f // Optional
	};

	// setup viewport state
	VkRect2D scissor = 
	{ 
		.offset = { 0, 0 }, 
		.extent = renderer->swapchain->image_extent 
	};
	VkViewport viewport = 
	{
		.x = 0,
		.y = 0, 
		.width = renderer->swapchain->image_extent.width, 
		.height = renderer->swapchain->image_extent.height, 
		.minDepth = 0.0f, 
		.maxDepth = 1.0f
	};
	VkPipelineViewportStateCreateInfo viewport_state =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1, 
		.pScissors = &scissor
	};

	// setup multiple sample state
	VkPipelineMultisampleStateCreateInfo multi_sampling_state =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.sampleShadingEnable = VK_FALSE,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.minSampleShading = 1.0f, // Optional
		.pSampleMask = NULL, // Optional
		.alphaToCoverageEnable = VK_FALSE, // Optional
		.alphaToOneEnable = VK_FALSE // Optional
	};

	// setup color blend attachment state
	VkPipelineColorBlendAttachmentState color_blend_attachmentment_state = 
	{
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		.blendEnable = (create_info->blend_enabled == true) ? VK_TRUE : VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD
	};
	
	// setup color blend state
	VkPipelineColorBlendStateCreateInfo color_blend_state = 
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

	// setup depth stencil state
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
		.depthBoundsTestEnable = VK_FALSE,
		.minDepthBounds = 0.0f, 	// optional
		.maxDepthBounds = 1.0f,		// optional
		.stencilTestEnable = VK_FALSE,
		.front = (VkStencilOpState) { }, // optional
		.back = (VkStencilOpState) { } 	 // optional
	};

	// create graphics pipeline
	VkGraphicsPipelineCreateInfo pipeline_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		//TODO: Make this customizable, for more shader stages in the pipeline
		.stageCount = 2,
		.pStages = shader_stages,
		.pVertexInputState = &vertex_input_state,
		.pInputAssemblyState = &input_assembly_state,
		.pViewportState = &viewport_state,
		.pRasterizationState = &rasterization_state,
		.pMultisampleState = &multi_sampling_state,
		.pDepthStencilState = &depth_stencil_state,
		.pColorBlendState = &color_blend_state,
		.pDynamicState = NULL, // Optional
		.layout = pipeline->pipeline_layout->handle,
		.renderPass = renderer->render_pass->handle,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE, // Optional
		.basePipelineIndex = -1 // Optional
	};
	vkCall(vkCreateGraphicsPipelines(renderer->logical_device->handle, VK_NULL_HANDLE, 1, &pipeline_create_info, NULL, &pipeline->pipeline));

	buf_free(&attribute_descriptions);
}

RENDERER_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_create(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info)
{
	vulkan_graphics_pipeline_t* pipeline = vulkan_graphics_pipeline_new();
	vulkan_graphics_pipeline_create_no_alloc(renderer, create_info, pipeline);
	return pipeline;
}


RENDERER_API void vulkan_graphics_pipeline_bind(vulkan_graphics_pipeline_t* pipeline, vulkan_renderer_t* renderer)
{
	assert(renderer->logical_device->handle != VK_NULL_HANDLE);
	assert(renderer->swapchain != NULL);

	u32 image_index = renderer->swapchain->current_image_index;
	vkCmdBindPipeline(renderer->vk_command_buffers[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
}

RENDERER_API void vulkan_graphics_pipeline_destroy(vulkan_graphics_pipeline_t* pipeline, vulkan_renderer_t* renderer)
{
	assert(renderer->logical_device->handle != VK_NULL_HANDLE);
	vulkan_pipeline_layout_destroy(pipeline->pipeline_layout, renderer);
	vkDestroyPipeline(renderer->logical_device->handle, pipeline->pipeline, NULL);
}

RENDERER_API void vulkan_graphics_pipeline_release_resources(vulkan_graphics_pipeline_t* pipeline)
{
	vulkan_pipeline_layout_release_resources(pipeline->pipeline_layout);
	heap_free(pipeline);
}

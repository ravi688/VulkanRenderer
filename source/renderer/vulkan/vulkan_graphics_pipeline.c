#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_shader_module.h>
#include <shader_compiler/settings_parser.h>
#include <renderer/render_window.h>

#include <renderer/memory_allocator.h>

RENDERER_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_new()
{
	vulkan_graphics_pipeline_t* pipeline = heap_new(vulkan_graphics_pipeline_t);
	memset(pipeline, 0, sizeof(vulkan_graphics_pipeline_t));
	return pipeline;
}

RENDERER_API void vulkan_graphics_pipeline_create_no_alloc(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info, vulkan_graphics_pipeline_t OUT pipeline)
{
	pipeline->renderer;
	// copy the shader stage create info to a continuous array of VkPipelineShaderStageCreateInfo
	VkPipelineShaderStageCreateInfo* shader_stages = heap_newv(VkPipelineShaderStageCreateInfo, create_info->shader_module_count);
	for(u32 i = 0; i < create_info->shader_module_count; i++)
		shader_stages[i] = create_info->shader_modules[i].vo_stage;

	// setup vertex input binding and descriptions
	VkVertexInputBindingDescription* binding_descriptions = heap_newv(VkVertexInputBindingDescription, create_info->vertex_attribute_binding_count);
	BUFFER attribute_descriptions = buf_create(sizeof(VkVertexInputAttributeDescription), 0, 0);

	for(u32 i = 0; i < create_info->vertex_attribute_binding_count; i++)
	{
		binding_descriptions[i].binding = create_info->vertex_attribute_bindings[i].binding;
		binding_descriptions[i].stride = create_info->vertex_attribute_bindings[i].size;
		binding_descriptions[i].inputRate = create_info->vertex_attribute_bindings[i].input_rate;

		for(u32 j = 0; j < create_info->vertex_attribute_bindings[i].attribute_count; j++)
		{
			VkVertexInputAttributeDescription description =
			{
				.binding = create_info->vertex_attribute_bindings[i].binding,
				.location = create_info->vertex_attribute_bindings[i].attribute_locations[j],
				.format = create_info->vertex_attribute_bindings[i].attribute_formats[j],
				.offset = create_info->vertex_attribute_bindings[i].attribute_offsets[j]
			};
			buf_push(&attribute_descriptions, &description);
		}
	}

	// setup vertex input state
	VkPipelineVertexInputStateCreateInfo vertex_input_state = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, 
		.vertexBindingDescriptionCount = create_info->vertex_attribute_binding_count, 
		.pVertexBindingDescriptions = binding_descriptions, 
		.vertexAttributeDescriptionCount = attribute_descriptions.element_count, 
		.pVertexAttributeDescriptions = attribute_descriptions.bytes
	};

	// setup the viewport and scissor
	VkPipelineViewportStateCreateInfo* viewport_state = &create_info->settings->viewport;
	if(viewport_state->viewportCount > 0)
	{
		VkViewport* viewports = (void*)viewport_state->pViewports;
		for(int i = 0; i < viewport_state->viewportCount; i++)
		{
			if(viewports[i].width == 0)
				viewports[i].width = renderer->swapchain->vo_image_extent.width;
			if(viewports[i].height == 0)
				viewports[i].height = renderer->swapchain->vo_image_extent.height;
		}
	}
	if(viewport_state->scissorCount > 0)
	{
		VkRect2D* scissors = (void*)viewport_state->pScissors;
		for(int i = 0; i < viewport_state->scissorCount; i++)
		{
			if(scissors[i].extent.width == 0)
				scissors[i].extent.width = renderer->swapchain->vo_image_extent.width;
			if(scissors[i].extent.height == 0)
				scissors[i].extent.height = renderer->swapchain->vo_image_extent.height;
		}
	}

	// create graphics pipeline
	VkGraphicsPipelineCreateInfo pipeline_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = create_info->shader_module_count,
		.pStages = shader_stages,
		.pVertexInputState = &vertex_input_state,
		.pInputAssemblyState = &create_info->settings->inputassembly,
		.pViewportState = viewport_state,
		.pRasterizationState = &create_info->settings->rasterization,
		.pMultisampleState = &create_info->settings->multisample,
		.pDepthStencilState = &create_info->settings->depthstencil,
		.pColorBlendState = &create_info->settings->colorblend,
		.layout = create_info->layout->vo_handle,
		.renderPass = create_info->render_pass->vo_handle,
		.subpass = create_info->subpass_index,
		.basePipelineHandle = VK_NULL_HANDLE, // Optional
		.basePipelineIndex = -1 // Optional
	};
	vkCall(vkCreateGraphicsPipelines(renderer->logical_device->vo_handle, VK_NULL_HANDLE, 1, &pipeline_create_info, NULL, &pipeline->vo_handle));

	buf_free(&attribute_descriptions);
	heap_free(binding_descriptions);
	heap_free(shader_stages);
}

RENDERER_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_create(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info)
{
	vulkan_graphics_pipeline_t* pipeline = vulkan_graphics_pipeline_new();
	vulkan_graphics_pipeline_create_no_alloc(renderer, create_info, pipeline);
	return pipeline;
}

RENDERER_API void vulkan_graphics_pipeline_bind(vulkan_graphics_pipeline_t* pipeline)
{
	u32 image_index = pipeline->renderer->swapchain->current_image_index;
	vkCmdBindPipeline(pipeline->renderer->vo_command_buffers[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vo_handle);
}

RENDERER_API void vulkan_graphics_pipeline_destroy(vulkan_graphics_pipeline_t* pipeline)
{
	vkDestroyPipeline(pipeline->renderer->logical_device->vo_handle, pipeline->vo_handle, NULL);
}

RENDERER_API void vulkan_graphics_pipeline_release_resources(vulkan_graphics_pipeline_t* pipeline)
{
	heap_free(pipeline);
}

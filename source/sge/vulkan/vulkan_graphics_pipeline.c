/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_graphics_pipeline.c is a part of VulkanRenderer

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

#include <sge/internal/vulkan/vulkan_graphics_pipeline.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_defines.h>
#include <sge/internal/vulkan/vulkan_pipeline_layout.h>
#include <sge/internal/vulkan/vulkan_swapchain.h>
#include <sge/internal/vulkan/vulkan_shader_module.h>
#include <sge/internal/vulkan/vulkan_graphics_pipeline_description.h>
#include <sge/internal/vulkan/vulkan_allocator.h>
#include <sge/render_window.h>
#include <sge/memory_allocator.h>
#include <sge/alloc.h>
#include <disk_manager/file_reader.h>

SGE_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_new(memory_allocator_t* allocator)
{
	vulkan_graphics_pipeline_t* pipeline = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_GRAPHICS_PIPELINE, vulkan_graphics_pipeline_t);
	memzero(pipeline, vulkan_graphics_pipeline_t);
	VULKAN_OBJECT_INIT(pipeline, VULKAN_OBJECT_TYPE_GRAPHICS_PIPELINE, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return pipeline;
}

static u32 calculate_attribute_description_count(vulkan_vertex_buffer_layout_description_t* binding_descriptions, u32 binding_description_count)
{
	u32 count = 0;
	for(u32 i = 0; i < binding_description_count; i++)
		count += binding_descriptions[i].attribute_count;
	return count;
}

SGE_API void vulkan_graphics_pipeline_create_no_alloc(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info, vulkan_graphics_pipeline_t OUT pipeline)
{
	_debug_assert__(create_info->spirv_code_count > 0);

	VULKAN_OBJECT_MEMZERO(pipeline, vulkan_graphics_pipeline_t);

	pipeline->renderer = renderer;

	pipeline->shader_module_count = create_info->spirv_code_count;
	/* list of shader modules to be created from the spirv codes */
	pipeline->shader_modules = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_MODULE_ARRAY, vulkan_shader_module_t, pipeline->shader_module_count);
	// copy the shader stage create info to a continuous array of VkPipelineShaderStageCreateInfo
	pipeline->vo_shader_stages = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_SHADER_STAGE_CREATE_INFO_ARRAY, VkPipelineShaderStageCreateInfo, pipeline->shader_module_count);
	for(u32 i = 0; i < pipeline->shader_module_count; i++)
	{
		vulkan_shader_module_create_info_t module_create_info = REINTERPRET_TO(vulkan_shader_module_create_info_t, create_info->spirv_codes[i]);
		VULKAN_OBJECT_INIT(&pipeline->shader_modules[i], VULKAN_OBJECT_TYPE_SHADER_MODULE, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
		vulkan_shader_module_create_no_alloc(renderer, &module_create_info, &pipeline->shader_modules[i]);
		pipeline->vo_shader_stages[i] = pipeline->shader_modules[i].vo_stage;
	}

	// setup vertex input binding and descriptions
	pipeline->binding_description_count = create_info->vertex_attribute_binding_count;
	pipeline->vo_binding_descriptions = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_VERTEX_INPUT_BINDING_DESCRIPTION_ARRAY, VkVertexInputBindingDescription, pipeline->binding_description_count);
	pipeline->attribute_description_count = calculate_attribute_description_count(create_info->vertex_attribute_bindings, create_info->vertex_attribute_binding_count);
	pipeline->vo_attribute_descriptions = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_ARRAY, VkVertexInputAttributeDescription, pipeline->attribute_description_count);

	for(u32 i = 0, k = 0; i < create_info->vertex_attribute_binding_count; i++)
	{
		pipeline->vo_binding_descriptions[i].binding = create_info->vertex_attribute_bindings[i].binding;
		pipeline->vo_binding_descriptions[i].stride = create_info->vertex_attribute_bindings[i].size;
		pipeline->vo_binding_descriptions[i].inputRate = create_info->vertex_attribute_bindings[i].input_rate;

		for(u32 j = 0; j < create_info->vertex_attribute_bindings[i].attribute_count; j++, k++)
		{
			_debug_assert__(k < pipeline->attribute_description_count);

			pipeline->vo_attribute_descriptions[k] = (VkVertexInputAttributeDescription)
			{
				.binding = create_info->vertex_attribute_bindings[i].binding,
				.location = create_info->vertex_attribute_bindings[i].attribute_locations[j],
				.format = create_info->vertex_attribute_bindings[i].attribute_formats[j],
				.offset = create_info->vertex_attribute_bindings[i].attribute_offsets[j]
			};
		}
	}

	// setup vertex input state
	VkPipelineVertexInputStateCreateInfo vertex_input_state = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, 
		.vertexBindingDescriptionCount = pipeline->binding_description_count, 
		.pVertexBindingDescriptions = pipeline->vo_binding_descriptions, 
		.vertexAttributeDescriptionCount = pipeline->attribute_description_count, 
		.pVertexAttributeDescriptions = pipeline->vo_attribute_descriptions
	};

	// setup the viewport and scissor
	pipeline->viewport_count = create_info->settings->viewport.viewportCount;
	pipeline->vo_viewports = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_VIEWPORT_ARRAY, VkViewport, pipeline->viewport_count);
	pipeline->vo_user_defined_viewports = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_VIEWPORT_ARRAY, VkViewport, pipeline->viewport_count);
	pipeline->is_user_defined_viewport = false;
	for(int i = 0; i < pipeline->viewport_count; i++)
	{
		VkViewport viewport = create_info->settings->viewport.pViewports[i];
		
		if((!pipeline->is_user_defined_viewport) && (viewport.width != 0) && (viewport.height != 0))
			pipeline->is_user_defined_viewport = true;

		pipeline->vo_viewports[i] = (VkViewport)
		{
			.x = 0,
			.y = 0,
			.width = (viewport.width == 0) ? renderer->swapchain->vo_image_extent.width : viewport.width,
			.height = (viewport.height == 0) ? renderer->swapchain->vo_image_extent.height : viewport.height,
			.minDepth = 0,
			.maxDepth = 1
		};
		pipeline->vo_user_defined_viewports[i] = viewport;
	}
	pipeline->scissor_count = create_info->settings->viewport.scissorCount;
	pipeline->vo_scissors = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_RECT2D_ARRAY, VkRect2D, pipeline->scissor_count);
	pipeline->vo_user_defined_scissors = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_RECT2D_ARRAY, VkRect2D, pipeline->scissor_count);
	pipeline->is_user_defined_scissor = false;
	for(int i = 0; i < pipeline->scissor_count; i++)
	{
		VkRect2D scissor = create_info->settings->viewport.pScissors[i];
		
		if((!pipeline->is_user_defined_scissor) && (scissor.extent.width != 0) && (scissor.extent.height != 0))
			pipeline->is_user_defined_scissor = true;

		pipeline->vo_scissors[i] = (VkRect2D)
		{
			.offset = (VkOffset2D)
			{
				.x = scissor.offset.x,
				.y = scissor.offset.y
			},
			.extent = (VkExtent2D)
			{
				.width = (scissor.extent.width == 0) ? renderer->swapchain->vo_image_extent.width : scissor.extent.width,
				.height = (scissor.extent.height == 0) ? renderer->swapchain->vo_image_extent.height : scissor.extent.height
			}
		};
		pipeline->vo_user_defined_scissors[i] = scissor;
	}

	VkPipelineViewportStateCreateInfo viewport_state = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = pipeline->viewport_count,
		.pViewports = pipeline->vo_viewports,
		.scissorCount = pipeline->scissor_count,
		.pScissors = pipeline->vo_scissors
	};

	BUFFER dynamic_states = memory_allocator_buf_new(renderer->allocator, VkDynamicState);
	if(!pipeline->is_user_defined_viewport)
		buf_push_auto(&dynamic_states, VK_DYNAMIC_STATE_VIEWPORT);
	if(!pipeline->is_user_defined_scissor)
		buf_push_auto(&dynamic_states, VK_DYNAMIC_STATE_SCISSOR);

	/* cache result as that would be needed at the time of pipeline refresh */
	pipeline->dynamic_state_count = CAST_TO(u32, buf_get_element_count(&dynamic_states));
	pipeline->vo_dynamic_states = (pipeline->dynamic_state_count == 0) ? NULL : CAST_TO(VkDynamicState*, buf_get_ptr(&dynamic_states));

	VkPipelineDynamicStateCreateInfo dynamic_state_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = pipeline->dynamic_state_count,
		.pDynamicStates = pipeline->vo_dynamic_states
	};

	// create graphics pipeline
	VkGraphicsPipelineCreateInfo pipeline_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = pipeline->shader_module_count,
		.pStages = pipeline->vo_shader_stages,
		.pVertexInputState = &vertex_input_state,
		.pInputAssemblyState = &create_info->settings->inputassembly,
		.pViewportState = &viewport_state,
		.pRasterizationState = &create_info->settings->rasterization,
		.pMultisampleState = &create_info->settings->multisample,
		.pDepthStencilState = &create_info->settings->depthstencil,
		.pColorBlendState = &create_info->settings->colorblend,
		.pDynamicState = &dynamic_state_create_info,//&create_info->settings->dynamic TODO: most of pipeline setup shall go offline (shader_compiler),
		.layout = create_info->layout->vo_handle,
		.renderPass = create_info->render_pass->vo_handle,
		.subpass = create_info->subpass_index,
		.basePipelineHandle = VK_NULL_HANDLE, // Optional
		.basePipelineIndex = -1 // Optional
	};
	vkCall(vkCreateGraphicsPipelines(renderer->logical_device->vo_handle, VK_NULL_HANDLE, 1, &pipeline_create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &pipeline->vo_handle));

	pipeline->settings = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_GRAPHICS_PIPELINE_SETTINGS, vulkan_graphics_pipeline_settings_t);
	memcopy(pipeline->settings, create_info->settings, vulkan_graphics_pipeline_settings_t);
	if(pipeline->settings->colorblend.attachmentCount > 0)
		pipeline->settings->colorblend.pAttachments = memory_allocator_duplicate_array(renderer->allocator, 
																				MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_COLOR_BLEND_ATTACHMENT_STATE_ARRAY,
																				VkPipelineColorBlendAttachmentState,
																				create_info->settings->colorblend.pAttachments, 
																				create_info->settings->colorblend.attachmentCount);
	pipeline->layout = create_info->layout;
	pipeline->render_pass = create_info->render_pass;
	pipeline->subpass_index = create_info->subpass_index;
}

SGE_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_create(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info)
{
	vulkan_graphics_pipeline_t* pipeline = vulkan_graphics_pipeline_new(renderer->allocator);
	vulkan_graphics_pipeline_create_no_alloc(renderer, create_info, pipeline);
	return pipeline;
}

SGE_API VkRect2D vulkan_graphics_pipeline_get_scissor_rect(vulkan_graphics_pipeline_t* pipeline)
{
	/* if no user defined scissor exists, then return the current render area */
	if(!pipeline->is_user_defined_scissor)
		return pipeline->render_pass->vo_current_render_area;
	/* else return the first scissor from the list of user defined scissors.
	 * NOTE: we may support more than one scissors in future. */
	else
	{
		_debug_assert__(pipeline->scissor_count > 0);
		return pipeline->vo_user_defined_scissors[0];
	}
}

SGE_API void vulkan_graphics_pipeline_bind(vulkan_graphics_pipeline_t* pipeline)
{
	AUTO command_buffer = pipeline->renderer->vo_command_buffers[pipeline->renderer->current_frame_index];
	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vo_handle);

	debug_assert__(!pipeline->is_user_defined_viewport, "For now let's focus on full render area of the render target\n");

	/* setup the viewport if there is no user defined viewports in v3d shader */
	if(!pipeline->is_user_defined_viewport)
	{
		VkViewport viewport = 
		{
			.x = pipeline->render_pass->vo_current_render_area.offset.x,
			.y = pipeline->render_pass->vo_current_render_area.offset.y,
			.width = pipeline->render_pass->vo_current_render_area.extent.width,
			.height = pipeline->render_pass->vo_current_render_area.extent.height,
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		vkCmdSetViewport(command_buffer, 0, 1, &viewport);
	}
	
	/* setup the scissor if there is no user defined scissors in v3d shader */
	if(!pipeline->is_user_defined_scissor)
		vkCmdSetScissor(command_buffer, 0, 1, &pipeline->render_pass->vo_current_render_area);
}

SGE_API void vulkan_graphics_pipeline_refresh(vulkan_graphics_pipeline_t* pipeline, vulkan_graphics_pipeline_refresh_info_t* info)
{
	/* if there is nothing to update, we don't need to recreate the graphics pipeline unneccessarily */
	if((!pipeline->is_user_defined_scissor) && (!pipeline->is_user_defined_viewport))
		return;

	_debug_assert__(pipeline->vo_handle != VK_NULL_HANDLE);

	/* destroy the outdated pipeline */
	vkDestroyPipeline(pipeline->renderer->logical_device->vo_handle, pipeline->vo_handle, VULKAN_ALLOCATION_CALLBACKS(pipeline->renderer));

	/* recreate the pipeline */

	VkPipelineVertexInputStateCreateInfo vertex_input_state = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, 
		.vertexBindingDescriptionCount = pipeline->binding_description_count, 
		.pVertexBindingDescriptions = pipeline->vo_binding_descriptions, 
		.vertexAttributeDescriptionCount = pipeline->attribute_description_count, 
		.pVertexAttributeDescriptions = pipeline->vo_attribute_descriptions
	};

	_debug_assert__(pipeline->viewport_count == 1);
	for(int i = 0; i < pipeline->viewport_count; i++)
	{
		if(pipeline->is_user_defined_viewport)
		{
			pipeline->vo_viewports[i].width = pipeline->vo_user_defined_viewports[i].width;
			pipeline->vo_viewports[i].height = pipeline->vo_user_defined_viewports[i].height;
		}
		else
		{
			pipeline->vo_viewports[i].width = info->width;
			pipeline->vo_viewports[i].height = info->height;
		}
	}

	_debug_assert__(pipeline->scissor_count == 1);
	for(int i = 0; i < pipeline->scissor_count; i++)
	{
		if(pipeline->is_user_defined_scissor)
		{
			pipeline->vo_scissors[i].extent.width = pipeline->vo_user_defined_scissors[i].extent.width;
			pipeline->vo_scissors[i].extent.height = pipeline->vo_user_defined_scissors[i].extent.height;
		}
		else
		{
			pipeline->vo_scissors[i].extent.width = info->width;
			pipeline->vo_scissors[i].extent.height = info->height;
		}
	}

	VkPipelineViewportStateCreateInfo viewport_state = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = pipeline->viewport_count,
		.pViewports = pipeline->vo_viewports,
		.scissorCount = pipeline->scissor_count,
		.pScissors = pipeline->vo_scissors
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = pipeline->dynamic_state_count,
		.pDynamicStates = pipeline->vo_dynamic_states
	};

	// create graphics pipeline
	VkGraphicsPipelineCreateInfo pipeline_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = pipeline->shader_module_count,
		.pStages = pipeline->vo_shader_stages,
		.pVertexInputState = &vertex_input_state,
		.pInputAssemblyState = &pipeline->settings->inputassembly,
		.pViewportState = &viewport_state,
		.pRasterizationState = &pipeline->settings->rasterization,
		.pMultisampleState = &pipeline->settings->multisample,
		.pDepthStencilState = &pipeline->settings->depthstencil,
		.pColorBlendState = &pipeline->settings->colorblend,
		.pDynamicState = &dynamic_state_create_info,
		.layout = pipeline->layout->vo_handle,
		.renderPass = pipeline->render_pass->vo_handle,
		.subpass = pipeline->subpass_index,
		.basePipelineHandle = VK_NULL_HANDLE, // Optional
		.basePipelineIndex = -1 // Optional
	};
	vkCall(vkCreateGraphicsPipelines(pipeline->renderer->logical_device->vo_handle, VK_NULL_HANDLE, 1, &pipeline_create_info, VULKAN_ALLOCATION_CALLBACKS(pipeline->renderer), &pipeline->vo_handle));
}

SGE_API void vulkan_graphics_pipeline_destroy(vulkan_graphics_pipeline_t* pipeline)
{
	vkDestroyPipeline(pipeline->renderer->logical_device->vo_handle, pipeline->vo_handle, VULKAN_ALLOCATION_CALLBACKS(pipeline->renderer));
	pipeline->vo_handle = VK_NULL_HANDLE;

	/* destroy shader modules */
	for(u32 i = 0; i < pipeline->shader_module_count; i++)
		vulkan_shader_module_destroy(&pipeline->shader_modules[i]);
}

SGE_API void vulkan_graphics_pipeline_release_resources(vulkan_graphics_pipeline_t* pipeline)
{
	/* destroy shader modules */
	for(u32 i = 0; i < pipeline->shader_module_count; i++)
		vulkan_shader_module_release_resources(&pipeline->shader_modules[i]);

	/* destroy dynamic state objects */
	if(pipeline->dynamic_state_count > 0)
		memory_allocator_dealloc(pipeline->renderer->allocator, pipeline->vo_dynamic_states);

	memory_allocator_dealloc(pipeline->renderer->allocator, pipeline->shader_modules);
	memory_allocator_dealloc(pipeline->renderer->allocator, pipeline->vo_shader_stages);
	memory_allocator_dealloc(pipeline->renderer->allocator, pipeline->vo_binding_descriptions);
	memory_allocator_dealloc(pipeline->renderer->allocator, pipeline->vo_attribute_descriptions);
	memory_allocator_dealloc(pipeline->renderer->allocator, pipeline->vo_viewports);
	memory_allocator_dealloc(pipeline->renderer->allocator, pipeline->vo_user_defined_viewports);
	memory_allocator_dealloc(pipeline->renderer->allocator, pipeline->vo_scissors);
	memory_allocator_dealloc(pipeline->renderer->allocator, pipeline->vo_user_defined_scissors);
	memory_allocator_dealloc(pipeline->renderer->allocator, CAST_TO(void*, pipeline->settings->colorblend.pAttachments));
	memory_allocator_dealloc(pipeline->renderer->allocator, pipeline->settings);
	if(VULKAN_OBJECT_IS_INTERNAL(pipeline))
		memory_allocator_dealloc(pipeline->renderer->allocator, pipeline);
}

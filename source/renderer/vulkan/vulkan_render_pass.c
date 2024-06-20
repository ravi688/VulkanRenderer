/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass.c is a part of VulkanRenderer

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


#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>
#include <renderer/internal/vulkan/vulkan_framebuffer.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/render_window.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/debug.h>

vulkan_render_pass_create_info_t* vulkan_render_pass_create_info_new(memory_allocator_t* allocator)
{
	vulkan_render_pass_create_info_t* create_info = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_CREATE_INFO, vulkan_render_pass_create_info_t);
	memzero(create_info, vulkan_render_pass_create_info_t);
	return create_info;
}

RENDERER_API vulkan_render_pass_t* vulkan_render_pass_new(memory_allocator_t* allocator)
{
	vulkan_render_pass_t* render_pass = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS, vulkan_render_pass_t);
	memzero(render_pass, vulkan_render_pass_t);
	VULKAN_OBJECT_INIT(render_pass, VULKAN_OBJECT_TYPE_RENDER_PASS, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return render_pass;
}

RENDERER_API vulkan_render_pass_t* vulkan_render_pass_create(vulkan_renderer_t* renderer, vulkan_render_pass_create_info_t* create_info)
{
	vulkan_render_pass_t* render_pass = vulkan_render_pass_new(renderer->allocator);
	vulkan_render_pass_create_no_alloc(renderer, create_info, render_pass);
	return render_pass;
}

static VkClearValue get_clear_value_from_format(VkFormat format)
{
	switch(format)
	{
		case VK_FORMAT_B8G8R8A8_SRGB:
			return (VkClearValue) { .color = { .float32 = { 0.1f, 0.1f, 0.1f, 0.1f } } };
		case VK_FORMAT_D32_SFLOAT:
			return (VkClearValue) { .depthStencil = { 1.0f, 0UL } };
		default:
			LOG_FETAL_ERR("Unsupported format %u for clear value\n", format);
	}
	return (VkClearValue) { };
}

UNUSED_FUNCTION static VkImageLayout get_attachment_layout(VkFormat format)
{
	switch(format)
	{
		case VK_FORMAT_B8G8R8A8_SRGB:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case VK_FORMAT_D32_SFLOAT:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		default:
			LOG_FETAL_ERR("Unsupported format %u for attachment layout\n", format);
	}
	return VK_IMAGE_LAYOUT_UNDEFINED;
}

RENDERER_API void vulkan_render_pass_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_create_info_t* create_info, vulkan_render_pass_t OUT render_pass)
{
	VULKAN_OBJECT_MEMZERO(render_pass, vulkan_render_pass_t);

	render_pass->renderer = renderer;
	render_pass->subpass_count = create_info->subpass_count;
	render_pass->current_subpass_index = 0;
	render_pass->handle = VULKAN_RENDER_PASS_HANDLE_INVALID;
	render_pass->required_framebuffer_count = create_info->framebuffer_count;

	_debug_assert__(create_info->subpass_count > 0);
	VkSubpassDescription* subpasses = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SUBPASS_DESCRIPTION_ARRAY, VkSubpassDescription, create_info->subpass_count);
	for(u32 i = 0; i < create_info->subpass_count; i++)
	{
		subpasses[i] = (VkSubpassDescription)
		{
			.pipelineBindPoint = create_info->subpasses[i].pipeline_bind_point,
			.pColorAttachments = create_info->subpasses[i].color_attachments,
			.colorAttachmentCount = create_info->subpasses[i].color_attachment_count,
			.inputAttachmentCount = create_info->subpasses[i].input_attachment_count,
			.pInputAttachments = create_info->subpasses[i].input_attachments,
			.pDepthStencilAttachment = create_info->subpasses[i].depth_stencil_attachment,
			.preserveAttachmentCount = create_info->subpasses[i].preserve_attachment_count,
			.pPreserveAttachments = create_info->subpasses[i].preserve_attachments
		};
	}

	_debug_assert__(create_info->framebuffer_layout_description.attachment_description_count > 0);
	
	VkRenderPassCreateInfo render_pass_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pAttachments = create_info->framebuffer_layout_description.attachment_descriptions,
		.attachmentCount = create_info->framebuffer_layout_description.attachment_description_count,
		.subpassCount = create_info->subpass_count,
		.pSubpasses = subpasses,
		.dependencyCount = create_info->subpass_dependency_count,
		.pDependencies = create_info->subpass_dependencies
	};
	vkCall(vkCreateRenderPass(renderer->logical_device->vo_handle, &render_pass_create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &render_pass->vo_handle));
	log_ptr(render_pass->vo_handle);
	memory_allocator_dealloc(renderer->allocator, subpasses);

	render_pass->attachment_count = create_info->framebuffer_layout_description.attachment_description_count;

	// create clear values for each attachment in this render pass
	render_pass->vo_clear_values = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_CLEAR_VALUE_ARRAY, VkClearValue, render_pass->attachment_count);
	safe_memzerov(render_pass->vo_clear_values, VkClearValue, render_pass->attachment_count);
	render_pass->vo_formats = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_FORMAT_ARRAY, VkFormat, render_pass->attachment_count);
	for(u32 i = 0; i < render_pass->attachment_count; i++)
	{
		render_pass->vo_formats[i] = create_info->framebuffer_layout_description.attachment_descriptions[i].format;
		render_pass->vo_clear_values[i] = get_clear_value_from_format(create_info->framebuffer_layout_description.attachment_descriptions[i].format);
	}

	/* create render set layout */
	VULKAN_OBJECT_INIT(&render_pass->render_set_layout, VULKAN_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(renderer, create_info->render_set_bindings, create_info->render_set_binding_count, &render_pass->render_set_layout);

	// create sub render set layouts
	render_pass->sub_render_set_layouts = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_DESCRIPTOR_SET_LAYOUT_ARRAY, vulkan_descriptor_set_layout_t, create_info->subpass_count);
	for(int i = 0; i < create_info->subpass_count; i++)
	{
		VULKAN_OBJECT_INIT(&render_pass->sub_render_set_layouts[i], VULKAN_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
		vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(renderer, create_info->subpasses[i].sub_render_set_bindings, create_info->subpasses[i].sub_render_set_binding_count, &render_pass->sub_render_set_layouts[i]);
	}
}

RENDERER_API void vulkan_render_pass_destroy(vulkan_render_pass_t* render_pass)
{
	// unregister this render pass from the all the cameras
	// TODO

	// destory the vulkan render pass object
	vkDestroyRenderPass(render_pass->renderer->logical_device->vo_handle, render_pass->vo_handle, VULKAN_ALLOCATION_CALLBACKS(render_pass->renderer));

	// destroy the vulkan descriptor set layouts (sub render set layouts)
	for(u32 i = 0; i < render_pass->subpass_count; i++)
		vulkan_descriptor_set_layout_destroy(&render_pass->sub_render_set_layouts[i]);

	// destroy the vulkan descriptor set layout (render set layout)
	vulkan_descriptor_set_layout_destroy(&render_pass->render_set_layout);
}

RENDERER_API void vulkan_render_pass_release_resources(vulkan_render_pass_t* render_pass)
{
	for(u32 i = 0; i < render_pass->subpass_count; i++)
		vulkan_descriptor_set_layout_release_resources(&render_pass->sub_render_set_layouts[i]);
	if(render_pass->subpass_count > 0)
		memory_allocator_dealloc(render_pass->renderer->allocator, render_pass->sub_render_set_layouts);

	vulkan_descriptor_set_layout_release_resources(&render_pass->render_set_layout);

	memory_allocator_dealloc(render_pass->renderer->allocator, render_pass->vo_clear_values);
	memory_allocator_dealloc(render_pass->renderer->allocator, render_pass->vo_formats);
	if(VULKAN_OBJECT_IS_INTERNAL(render_pass))
		memory_allocator_dealloc(render_pass->renderer->allocator, render_pass);
}

RENDERER_API void vulkan_render_pass_set_clear_indirect(vulkan_render_pass_t* render_pass, color_t color, float depth, VkClearValue* indirect_buffer)
{
	for(u32 i = 0; i < render_pass->attachment_count; i++)
	{
		switch(render_pass->vo_formats[i])
		{
			case VK_FORMAT_D32_SFLOAT:
				indirect_buffer[i] = (VkClearValue) { .depthStencil = { depth, 0UL } };
				break;
			case VK_FORMAT_B8G8R8A8_SRGB:
				indirect_buffer[i] = (VkClearValue) { .color = { .float32 = 
					{ 
						color.r,
						color.g,
						color.b,
						color.a
					} } };
			break;
			default:
				LOG_FETAL_ERR("Unsupported VkFormat %u for clear value\n", render_pass->vo_formats[i]);
		}
	}	
}

RENDERER_API void vulkan_render_pass_set_clear(vulkan_render_pass_t* render_pass, color_t color, float depth)
{
	vulkan_render_pass_set_clear_indirect(render_pass, color, depth, render_pass->vo_clear_values);
}

RENDERER_API void vulkan_render_pass_begin(vulkan_render_pass_t* render_pass, u32 framebuffer_index, vulkan_camera_t* camera)
{
	framebuffer_index = (framebuffer_index == VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN) ? render_pass->renderer->swapchain->current_image_index : framebuffer_index;
	framebuffer_index = min(render_pass->required_framebuffer_count - 1, framebuffer_index);

	vulkan_framebuffer_t* framebuffers = vulkan_camera_get_framebuffer_list(camera, render_pass->handle);

	_debug_assert__(framebuffers[framebuffer_index].render_pass == render_pass);
	
	render_pass->vo_current_render_area = (VkRect2D)
	{
		.offset = { camera->render_area.offset.x, camera->render_area.offset.y },
		.extent = { camera->render_area.extent.width, camera->render_area.extent.height }
	};

	// begin the render pass
	VkRenderPassBeginInfo render_pass_begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderArea = render_pass->vo_current_render_area,
		.framebuffer = framebuffers[framebuffer_index].vo_handle,
		.renderPass = render_pass->vo_handle,
		.clearValueCount = render_pass->clear_value_count,
		.pClearValues = render_pass->vo_clear_values
	};
	vkCmdBeginRenderPass(render_pass->renderer->vo_command_buffers[render_pass->renderer->current_frame_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	render_pass->current_subpass_index = 0;
}

RENDERER_API void vulkan_render_pass_end(vulkan_render_pass_t* render_pass)
{
	vkCmdEndRenderPass(render_pass->renderer->vo_command_buffers[render_pass->renderer->current_frame_index]);
}

RENDERER_API void vulkan_render_pass_next(vulkan_render_pass_t* render_pass)
{
	if(render_pass->current_subpass_index >= render_pass->subpass_count)
	{
		log_wrn("There are only %u subpasses but you're trying to exceed it\n", render_pass->subpass_count);
		return;
	}
	vkCmdNextSubpass(render_pass->renderer->vo_command_buffers[render_pass->renderer->current_frame_index], VK_SUBPASS_CONTENTS_INLINE);
	++(render_pass->current_subpass_index);
}

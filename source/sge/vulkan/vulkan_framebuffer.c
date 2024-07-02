/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_framebuffer.c is a part of VulkanRenderer

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


#include <sge/internal/vulkan/vulkan_framebuffer.h>
#include <sge/internal/vulkan/vulkan_render_pass.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_defines.h>
#include <sge/internal/vulkan/vulkan_allocator.h>
#include <sge/render_window.h>
#include <sge/memory_allocator.h>
#include <sge/alloc.h>

SGE_API vulkan_framebuffer_t* vulkan_framebuffer_new(memory_allocator_t* allocator)
{
	vulkan_framebuffer_t* framebuffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_FRAMEBUFFER, vulkan_framebuffer_t);
	memzero(framebuffer, vulkan_framebuffer_t);
	VULKAN_OBJECT_INIT(framebuffer, VULKAN_OBJECT_TYPE_FRAMEBUFFER, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return framebuffer;
}

SGE_API vulkan_framebuffer_t* vulkan_framebuffer_create(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info)
{
	vulkan_framebuffer_t* framebuffer = vulkan_framebuffer_new(renderer->allocator);
	vulkan_framebuffer_create_no_alloc(renderer, create_info, framebuffer);
	return framebuffer;
}

SGE_API void vulkan_framebuffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info, vulkan_framebuffer_t OUT framebuffer)
{
	VULKAN_OBJECT_MEMZERO(framebuffer, vulkan_framebuffer_t);
	framebuffer->renderer = renderer;
	framebuffer->vo_handle = VK_NULL_HANDLE;
	framebuffer->id = create_info->id;
	framebuffer->width = create_info->width;
	framebuffer->height = create_info->height;
	framebuffer->render_pass = create_info->render_pass;
	
	VkFramebufferCreateInfo framebuffer_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = create_info->render_pass->vo_handle,
		.attachmentCount = create_info->attachment_count,
		.pAttachments = create_info->attachments,
		.width = create_info->width,
		.height = create_info->height,
		.layers = 1
	};
	vkCall(vkCreateFramebuffer(framebuffer->renderer->logical_device->vo_handle, &framebuffer_create_info, VULKAN_ALLOCATION_CALLBACKS(framebuffer->renderer), &framebuffer->vo_handle));

	/* update the width */
	framebuffer->width = create_info->width;
	/* update the height */
	framebuffer->height = create_info->height;
}

SGE_API void vulkan_framebuffer_create_no_alloc_ext(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info, vulkan_framebuffer_t OUT framebuffer)
{
	VULKAN_OBJECT_INIT(framebuffer, VULKAN_OBJECT_TYPE_FRAMEBUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_framebuffer_create_no_alloc(renderer, create_info, framebuffer);
}

SGE_API void vulkan_framebuffer_destroy(vulkan_framebuffer_t* framebuffer)
{
	vkDestroyFramebuffer(framebuffer->renderer->logical_device->vo_handle, framebuffer->vo_handle, VULKAN_ALLOCATION_CALLBACKS(framebuffer->renderer));
	framebuffer->vo_handle = VK_NULL_HANDLE;
}

SGE_API void vulkan_framebuffer_release_resources(vulkan_framebuffer_t* framebuffer)
{
	if(VULKAN_OBJECT_IS_INTERNAL(framebuffer))
		memory_allocator_dealloc(framebuffer->renderer->allocator, framebuffer);
}

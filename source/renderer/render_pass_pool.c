/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_pass_pool.c is a part of VulkanRenderer

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


#include <renderer/render_pass_pool.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/alloc.h>
#include <renderer/debug.h>

/* constructors & destructors */
RENDERER_API render_pass_pool_t* render_pass_pool_new(memory_allocator_t* allocator)
{
	return vulkan_render_pass_pool_new(allocator);
}

RENDERER_API render_pass_pool_t* render_pass_pool_create(renderer_t* renderer)
{
	return vulkan_render_pass_pool_create(renderer->vulkan_handle);
}

RENDERER_API void render_pass_pool_destroy(render_pass_pool_t* pool)
{
	vulkan_render_pass_pool_destroy(pool);
}

RENDERER_API void render_pass_pool_release_resources(render_pass_pool_t* pool)
{
	vulkan_render_pass_pool_release_resources(pool);
}


/* logic functions */

static vulkan_render_pass_create_info_t* build_shadow_map_render_pass_create_info(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_create_info_t* create_info = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_CREATE_INFO, vulkan_render_pass_create_info_t);
	memzero(create_info, vulkan_render_pass_create_info_t);
	
	create_info->framebuffer_count = 1;

	/* setup framebuffer attachments layout description */
	create_info->framebuffer_layout_description = (vulkan_framebuffer_attachments_layout_description_t)
	{
		.attachment_count = 1,
		.attachments = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_DESCRIPTION, VkAttachmentDescription),
		.attachment_usages = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_NEXT_PASS_USAGE, vulkan_attachment_next_pass_usage_t)
	};
	create_info->framebuffer_layout_description.attachments[0] = (VkAttachmentDescription)
	{
		.format = VK_FORMAT_D32_SFLOAT,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL
	};
	create_info->framebuffer_layout_description.attachment_usages[0] = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED;

	// create_info->supplementary_attachment_count = 1;
	// create_info->supplementary_attachments = renderer->swapchain->vo_image_views;

	VkAttachmentReference* depth_stencil_attachment = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_REFERENCE, VkAttachmentReference);
	depth_stencil_attachment[0] = (VkAttachmentReference)
	{
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.attachment = 0
	};
	create_info->subpass_count = 1;
	create_info->subpasses = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_CREATE_INFO, vulkan_subpass_create_info_t);
	create_info->subpasses[0] = (vulkan_subpass_create_info_t)
	{
		.pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.depth_stencil_attachment = depth_stencil_attachment
	};

	return create_info;
}

static vulkan_render_pass_create_info_t* build_swapchain_color_render_pass_create_info(vulkan_renderer_t* renderer, VkAttachmentLoadOp loadOp)
{
	vulkan_render_pass_create_info_t* create_info = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_CREATE_INFO, vulkan_render_pass_create_info_t);
	memzero(create_info, vulkan_render_pass_create_info_t);
	
	create_info->framebuffer_count = renderer->swapchain->image_count;

	/* setup framebuffer attachments layout description */
	create_info->framebuffer_layout_description = (vulkan_framebuffer_attachments_layout_description_t)
	{
		.attachment_count = 1,
		.attachments = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_DESCRIPTION, VkAttachmentDescription),
		.attachment_usages = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_NEXT_PASS_USAGE, vulkan_attachment_next_pass_usage_t),
		.supplementary_attachment_bucket_depth = renderer->swapchain->image_count,
		.supplementary_attachment_bucket_count = 1,
		/* supplementary_attachment_count = supplementary_attachment_bucket_count * supplementary_attachment_bucket_depth */
		.supplementary_attachment_count = 1 * renderer->swapchain->image_count,
		.vo_supplementary_attachments = renderer->swapchain->vo_image_views
	};
	create_info->framebuffer_layout_description.attachments[0] = (VkAttachmentDescription)
	{
		.format = renderer->swapchain->vo_image_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = loadOp,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};
	create_info->framebuffer_layout_description.attachment_usages[0] = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_PRESENT;

	VkAttachmentReference* color_attachments = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_REFERENCE, VkAttachmentReference);
	color_attachments[0] = (VkAttachmentReference)
	{
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.attachment = 0
	};
	create_info->subpass_count = 1;
	create_info->subpasses = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_CREATE_INFO, vulkan_subpass_create_info_t);
	create_info->subpasses[0] = (vulkan_subpass_create_info_t)
	{
		.pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.color_attachments = color_attachments,
		.color_attachment_count = 1
	};

	return create_info;
}

CAN_BE_UNUSED_FUNCTION static const char* preset_to_string(render_pass_pool_pass_preset_t preset)
{
	switch(preset)
	{
		case RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN: return "RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN";
		case RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN_CLEAR: return "RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN_CLEAR";
		case RENDER_PASS_POOL_PASS_PRESET_SHADOW_MAP: return "RENDER_PASS_POOL_PASS_PRESET_SHADOW_MAP";
		default: return "<UNKOWN PRESET>";
	}
}

RENDERER_API render_pass_handle_t render_pass_pool_create_pass_from_preset(render_pass_pool_t* pool, render_pass_pool_pass_preset_t preset)
{
	vulkan_render_pass_create_info_t* create_info;
	switch(preset)
	{
		case RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN:
			create_info = build_swapchain_color_render_pass_create_info(CAST_TO(vulkan_render_pass_t*, pool)->renderer, VK_ATTACHMENT_LOAD_OP_LOAD);
			break;
		case RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN_CLEAR:
			create_info = build_swapchain_color_render_pass_create_info(CAST_TO(vulkan_render_pass_t*, pool)->renderer, VK_ATTACHMENT_LOAD_OP_CLEAR);
			break;	
		case RENDER_PASS_POOL_PASS_PRESET_SHADOW_MAP:
			create_info = build_shadow_map_render_pass_create_info(CAST_TO(vulkan_render_pass_t*, pool)->renderer);
			break;
		default:
			return RENDER_PASS_HANDLE_INVALID;
	}
	vulkan_render_pass_input_info_t input = 
	{
		.input_attachment_count = 0,
		.input_attachments = NULL,
		.subpass_inputs = memory_allocator_alloc_obj(pool->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_INPUT_INFO, vulkan_subpass_input_info_t)
	};

	memzero(input.subpass_inputs, vulkan_subpass_input_info_t);

	AUTO handle = vulkan_render_pass_pool_create_pass(pool, create_info, &input);
	memory_allocator_dealloc(CAST_TO(vulkan_render_pass_t*, pool)->renderer->allocator, create_info);
	log_msg("Render pass has been created or fetched from pool with preset: %s\n", preset_to_string(preset));
	return handle;
}

RENDERER_API render_pass_handle_t render_pass_pool_create_pass(render_pass_pool_t* pool, render_pass_create_info_t* create_info, render_pass_input_info_t* input)
{
	return vulkan_render_pass_pool_create_pass(pool, create_info, input);
}

RENDERER_API render_pass_t* render_pass_pool_getH(render_pass_pool_t* pool, render_pass_handle_t handle)
{
	return vulkan_render_pass_pool_getH(pool, handle);
}


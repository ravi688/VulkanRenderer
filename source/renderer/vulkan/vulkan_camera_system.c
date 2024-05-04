/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_camera_system.c is a part of VulkanRenderer

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


#include <renderer/internal/vulkan/vulkan_camera_system.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>


RENDERER_API vulkan_camera_system_t* vulkan_camera_system_new(memory_allocator_t* allocator)
{
	vulkan_camera_system_t* system = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_CAMERA_SYSTEM, vulkan_camera_system_t);
	memzero(system, vulkan_camera_system_t);
	VULKAN_OBJECT_INIT(system, VULKAN_OBJECT_TYPE_CAMERA_SYSTEM, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return system;
}

RENDERER_API vulkan_camera_system_t* vulkan_camera_system_create(vulkan_renderer_t* renderer)
{
	vulkan_camera_system_t* system = vulkan_camera_system_new(renderer->allocator);
	system->renderer = renderer;
	library_create_no_alloc(renderer->allocator, &system->lib);
	return system;
}

RENDERER_API void vulkan_camera_system_destroy(vulkan_camera_system_t* system)
{
	buf_ucount_t count = library_get_count(&system->lib);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		vulkan_camera_t* camera = CAST_TO(vulkan_camera_t*, library_get_at(&system->lib, i));
		vulkan_camera_destroy(camera);
		vulkan_camera_release_resources(camera);
	}
	library_destroy(&system->lib);
}

RENDERER_API void vulkan_camera_system_release_resources(vulkan_camera_system_t* system)
{
	library_release_resources(&system->lib);
	if(VULKAN_OBJECT_IS_INTERNAL(system))
		memory_allocator_dealloc(system->renderer->allocator, system);
}


static vulkan_render_pass_create_info_t* build_swapchain_depth_clear_pass_create_info(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_create_info_t* create_info = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_CREATE_INFO, vulkan_render_pass_create_info_t);
	memzero(create_info, vulkan_render_pass_create_info_t);
	
	create_info->framebuffer_count = renderer->swapchain->image_count;
	
	create_info->framebuffer_layout_description = (vulkan_framebuffer_attachments_layout_description_t)
	{
		.attachment_count = 2,
		.attachments = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_DESCRIPTION_ARRAY, VkAttachmentDescription, 2),
		.attachment_usages = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_NEXT_PASS_USAGE_ARRAY, vulkan_attachment_next_pass_usage_t, 2),
		.supplementary_attachment_bucket_depth = renderer->swapchain->image_count,
		.supplementary_attachment_bucket_count = 1,
		.supplementary_attachment_count = 1 * renderer->swapchain->image_count,
		.vo_supplementary_attachments = renderer->swapchain->vo_image_views
	};
	
	/* swapchain color attachment must be first */
	create_info->framebuffer_layout_description.attachments[0] = (VkAttachmentDescription)
	{
		.format = renderer->swapchain->vo_image_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	/* depth attachment must be at the last */
	create_info->framebuffer_layout_description.attachments[1] = (VkAttachmentDescription)
	{
		.format = VK_FORMAT_D32_SFLOAT,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
	create_info->framebuffer_layout_description.attachment_usages[0] = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_PRESENT;
	create_info->framebuffer_layout_description.attachment_usages[1] = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_NONE;

	VkAttachmentReference* color_attachment_ref = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_REFERENCE, VkAttachmentReference);
	color_attachment_ref[0] = (VkAttachmentReference)
	{
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.attachment = 0
	};
	VkAttachmentReference* depth_attachment_ref = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_REFERENCE, VkAttachmentReference);
	/* NOTE: we can't allocate VkAttachmentReference object for depth stencil attachment on the stack
	 * as we are passing on the create info struct outside of this function! */
	depth_attachment_ref[0] = (VkAttachmentReference)
	{
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.attachment = 1
	};
	create_info->subpass_count = 1;
	create_info->subpasses = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_CREATE_INFO, vulkan_subpass_create_info_t);
	memzero(create_info->subpasses, vulkan_subpass_create_info_t);
	create_info->subpasses[0] = (vulkan_subpass_create_info_t)
	{
		.pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.color_attachments = color_attachment_ref,
		.color_attachment_count = 1,
		.depth_stencil_attachment = depth_attachment_ref
	};

	create_info->subpass_dependency_count = 2;
	VkSubpassDependency* dependencies = create_info->subpass_dependencies = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SUBPASS_DEPENDENCY_ARRAY, VkSubpassDependency, create_info->subpass_dependency_count);
	dependencies[0] = (VkSubpassDependency)
	{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
	};

	dependencies[1] = (VkSubpassDependency)
	{
		.srcSubpass = 0,
		.dstSubpass = VK_SUBPASS_EXTERNAL,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT,
		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
	};

	return create_info;
}

static vulkan_render_pass_create_info_t* build_depth_clear_pass_create_info(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_create_info_t* create_info = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_CREATE_INFO, vulkan_render_pass_create_info_t);
	memzero(create_info, vulkan_render_pass_create_info_t);
	
	create_info->framebuffer_count = 1;
	
	create_info->framebuffer_layout_description = (vulkan_framebuffer_attachments_layout_description_t)
	{
		.attachment_count = 1,
		.attachments = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_DESCRIPTION_ARRAY, VkAttachmentDescription, 1),
		.attachment_usages = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_NEXT_PASS_USAGE_ARRAY, vulkan_attachment_next_pass_usage_t, 1),
	};
	
	/* depth attachment must be at the last */
	create_info->framebuffer_layout_description.attachments[0] = (VkAttachmentDescription)
	{
		.format = VK_FORMAT_D32_SFLOAT,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
	create_info->framebuffer_layout_description.attachment_usages[0] = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_NONE;

	VkAttachmentReference* attachment_refs = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_REFERENCE_ARRAY, VkAttachmentReference, 1);
	/* NOTE: we can't allocate VkAttachmentReference object for depth stencil attachment on the stack
	 * as we are passing on the create info struct outside of this function! */
	attachment_refs[0] = (VkAttachmentReference)
	{
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.attachment = 0
	};
	create_info->subpass_count = 1;
	create_info->subpasses = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_CREATE_INFO, vulkan_subpass_create_info_t);
	create_info->subpasses[0] = (vulkan_subpass_create_info_t)
	{
		.pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.depth_stencil_attachment = attachment_refs
	};

	create_info->subpass_dependency_count = 1;
	VkSubpassDependency* dependencies = create_info->subpass_dependencies = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SUBPASS_DEPENDENCY_ARRAY, VkSubpassDependency, create_info->subpass_dependency_count);
	dependencies[0] = (VkSubpassDependency)
	{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
	};

	return create_info;
}

static vulkan_render_pass_create_info_t* build_swapchain_clear_pass_create_info(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_create_info_t* create_info = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_CREATE_INFO, vulkan_render_pass_create_info_t);
	memzero(create_info, vulkan_render_pass_create_info_t);
	
	create_info->framebuffer_count = renderer->swapchain->image_count;
	
	create_info->framebuffer_layout_description = (vulkan_framebuffer_attachments_layout_description_t)
	{
		.attachment_count = 1,
		.attachments = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_DESCRIPTION_ARRAY, VkAttachmentDescription, 1),
		.attachment_usages = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_NEXT_PASS_USAGE_ARRAY, vulkan_attachment_next_pass_usage_t, 1),
		.supplementary_attachment_bucket_depth = renderer->swapchain->image_count,
		.supplementary_attachment_bucket_count = 1,
		.supplementary_attachment_count = 1 * renderer->swapchain->image_count,
		.vo_supplementary_attachments = renderer->swapchain->vo_image_views
	};
	
	/* swapchain color attachment must be first */
	create_info->framebuffer_layout_description.attachments[0] = (VkAttachmentDescription)
	{
		.format = renderer->swapchain->vo_image_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	create_info->framebuffer_layout_description.attachment_usages[0] = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_PRESENT;

	VkAttachmentReference* attachment_refs = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_REFERENCE_ARRAY, VkAttachmentReference, 1);
	attachment_refs[0] = (VkAttachmentReference)
	{
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.attachment = 0
	};
	create_info->subpass_count = 1;
	create_info->subpasses = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_CREATE_INFO, vulkan_subpass_create_info_t);
	create_info->subpasses[0] = (vulkan_subpass_create_info_t)
	{
		.pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.color_attachments = attachment_refs,
		.color_attachment_count = 1,
	};

	create_info->subpass_dependency_count = 2;
	VkSubpassDependency* dependencies = create_info->subpass_dependencies = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SUBPASS_DEPENDENCY_ARRAY, VkSubpassDependency, create_info->subpass_dependency_count);
	dependencies[0] = (VkSubpassDependency)
	{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
	};

	dependencies[1] = (VkSubpassDependency)
	{
		.srcSubpass = 0,
		.dstSubpass = VK_SUBPASS_EXTERNAL,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT,
		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
	};

	return create_info;
}

static void destroy_vulkan_render_pass_create_info(vulkan_renderer_t* renderer, vulkan_render_pass_create_info_t* create_info)
{
	for(u32 i = 0; i < create_info->subpass_count; i++)
	{
		if(create_info->subpasses[i].color_attachment_count > 0)
			memory_allocator_dealloc(renderer->allocator, create_info->subpasses[i].color_attachments);
		if(create_info->subpasses[i].depth_stencil_attachment != NULL)
			memory_allocator_dealloc(renderer->allocator, create_info->subpasses[i].depth_stencil_attachment);
	}
	if(create_info->subpass_dependency_count > 0)
		memory_allocator_dealloc(renderer->allocator, create_info->subpass_dependencies);
	if(create_info->subpass_count > 0)
		memory_allocator_dealloc(renderer->allocator, create_info->subpasses);
	if(create_info->framebuffer_layout_description.attachment_count > 0)
	{
		memory_allocator_dealloc(renderer->allocator, create_info->framebuffer_layout_description.attachments);
		memory_allocator_dealloc(renderer->allocator, create_info->framebuffer_layout_description.attachment_usages);
	}
	/* NOTE: find a robust way to create and destroy vulkan_render_pass_create_info_t objects as there are three duplicates of 
	 * destroy_vulkan_render_pass_create_info() function in the codebase */
	// if(create_info->framebuffer_layout_description.supplementary_attachment_count > 0)
		// memory_allocator_dealloc(renderer->allocator, create_info->framebuffer_layout_description.vo_supplementary_attachments);
	memory_allocator_dealloc(renderer->allocator, create_info);
}

static vulkan_render_pass_handle_t create_swapchain_depth_clear_pass(vulkan_renderer_t* renderer, vulkan_render_pass_input_info_t* input)
{
	vulkan_render_pass_pool_create_path(renderer->render_pass_pool);
	AUTO create_info = build_swapchain_depth_clear_pass_create_info(renderer);
	AUTO handle = vulkan_render_pass_pool_create_pass(renderer->render_pass_pool, create_info, input);
	destroy_vulkan_render_pass_create_info(renderer, create_info);
	return handle;
}

static vulkan_render_pass_handle_t create_depth_clear_pass(vulkan_renderer_t* renderer, vulkan_render_pass_input_info_t* input)
{
	vulkan_render_pass_pool_create_path(renderer->render_pass_pool);
	AUTO create_info = build_depth_clear_pass_create_info(renderer);
	AUTO handle = vulkan_render_pass_pool_create_pass(renderer->render_pass_pool, create_info, input);
	destroy_vulkan_render_pass_create_info(renderer, create_info);
	return handle;
}

static vulkan_render_pass_handle_t create_swapchain_clear_pass(vulkan_renderer_t* renderer, vulkan_render_pass_input_info_t* input)
{
	vulkan_render_pass_pool_create_path(renderer->render_pass_pool);
	AUTO create_info = build_swapchain_clear_pass_create_info(renderer);
 	AUTO handle = vulkan_render_pass_pool_create_pass(renderer->render_pass_pool, create_info, input);
	destroy_vulkan_render_pass_create_info(renderer, create_info);
 	return handle;
}

RENDERER_API vulkan_camera_handle_t vulkan_camera_system_create_camera(vulkan_camera_system_t* system, vulkan_camera_projection_type_t projection_type)
{
	vulkan_render_pass_input_info_t input = 
	{
		.input_attachments = NULL,
		.input_attachment_count = 0,
		.subpass_inputs = memory_allocator_alloc_obj(system->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_INPUT_INFO, vulkan_subpass_input_info_t)
	};

	memzero(input.subpass_inputs, vulkan_subpass_input_info_t);

	vulkan_render_pass_pool_create_path(system->renderer->render_pass_pool);

	vulkan_camera_create_info_t create_info = 
	{
		.projection_type = projection_type,
		.swapchain_depth_clear_pass = create_swapchain_depth_clear_pass(system->renderer, &input),
		.depth_clear_pass = create_depth_clear_pass(system->renderer, &input),
		.swapchain_clear_pass = create_swapchain_clear_pass(system->renderer, &input)
	};
	AUTO handle = library_create_slot(&system->lib, vulkan_camera_create(system->renderer, &create_info));
	memory_allocator_dealloc(system->renderer->allocator, input.subpass_inputs);
	return handle;
}

RENDERER_API vulkan_camera_t* vulkan_camera_system_getH(vulkan_camera_system_t* system, vulkan_camera_handle_t handle)
{
	return CAST_TO(vulkan_camera_t*, library_getH(&system->lib, handle));
}

RENDERER_API void vulkan_camera_system_destroy_cameraH(vulkan_camera_system_t* system, vulkan_camera_handle_t handle)
{
	vulkan_camera_t* camera = vulkan_camera_system_getH(system, handle);
	vulkan_camera_destroy(camera);
	vulkan_camera_release_resources(camera);
	library_destroy_slotH(&system->lib, handle);
}

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
#include <renderer/internal/vulkan/vulkan_render_pass_create_info_builder.h>
#include <renderer/internal/vulkan/vulkan_subpass_create_info_builder.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>


SGE_API vulkan_camera_system_t* vulkan_camera_system_new(memory_allocator_t* allocator)
{
	vulkan_camera_system_t* system = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_CAMERA_SYSTEM, vulkan_camera_system_t);
	memzero(system, vulkan_camera_system_t);
	VULKAN_OBJECT_INIT(system, VULKAN_OBJECT_TYPE_CAMERA_SYSTEM, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return system;
}

SGE_API vulkan_camera_system_t* vulkan_camera_system_create(vulkan_renderer_t* renderer)
{
	vulkan_camera_system_t* system = vulkan_camera_system_new(renderer->allocator);
	system->renderer = renderer;
	library_create_no_alloc(renderer->allocator, &system->lib);
	return system;
}

SGE_API void vulkan_camera_system_destroy(vulkan_camera_system_t* system)
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

SGE_API void vulkan_camera_system_release_resources(vulkan_camera_system_t* system)
{
	library_release_resources(&system->lib);
	if(VULKAN_OBJECT_IS_INTERNAL(system))
		memory_allocator_dealloc(system->renderer->allocator, system);
}


static vulkan_render_pass_create_info_builder_t* build_swapchain_depth_clear_pass_create_info(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_create_info_builder_t* builder = vulkan_render_pass_create_info_builder_create(renderer->allocator);
	vulkan_render_pass_create_info_builder_add(builder, 1);
	vulkan_render_pass_create_info_builder_bind(builder, 0);

	vulkan_render_pass_create_info_builder_set_framebuffer_count(builder, renderer->swapchain->image_count);
	vulkan_render_pass_create_info_builder_set_supplementary_attachment_bucket(builder, 1, renderer->swapchain->image_count);
	vulkan_render_pass_create_info_builder_set_supplementary_attachments(builder, 0, renderer->swapchain->vo_image_views, renderer->swapchain->image_count);
	
	/* swapchain color attachment must be first */
	VkAttachmentDescription swapchain_color_attachment =
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
	vulkan_render_pass_create_info_builder_add_attachment_descriptions(builder, &swapchain_color_attachment, 1);

	/* depth attachment must be at the last */
	VkAttachmentDescription depth_attachment =
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
	vulkan_render_pass_create_info_builder_add_attachment_descriptions(builder, &depth_attachment, 1);
	vulkan_attachment_next_pass_usage_t attachment_usages[2] = 
	{
		VULKAN_ATTACHMENT_NEXT_PASS_USAGE_PRESENT,
		VULKAN_ATTACHMENT_NEXT_PASS_USAGE_NONE
	};
	vulkan_render_pass_create_info_builder_add_attachment_usages(builder, attachment_usages, 2);

	vulkan_subpass_create_info_builder_t* subpass_builder = vulkan_subpass_create_info_builder_create(renderer->allocator);
	vulkan_subpass_create_info_builder_add(subpass_builder, 1);
	vulkan_subpass_create_info_builder_bind(subpass_builder, 0);


	vulkan_subpass_create_info_builder_set_bind_point(subpass_builder, VK_PIPELINE_BIND_POINT_GRAPHICS);
	VkAttachmentReference color_attachment_reference =
	{
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.attachment = 0
	};
	vulkan_subpass_create_info_builder_add_color_attachments(subpass_builder, &color_attachment_reference, 1);
	vulkan_subpass_create_info_builder_set_depth_stencil_attachment(subpass_builder, (VkAttachmentReference)
																					{
																						.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
																						.attachment = 1
																					});
	vulkan_render_pass_create_info_builder_set_subpasses_builder(builder, subpass_builder, true);
	VkSubpassDependency dependencies[] = 
	{
		{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.dstSubpass = 0,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
		},
		{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.dstSubpass = 0,
			.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
		},
		{
			.srcSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstSubpass = VK_SUBPASS_EXTERNAL,
			.dstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
		},
		{
			.srcSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.dstSubpass = VK_SUBPASS_EXTERNAL,
			.dstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
		}
	};

	vulkan_render_pass_create_info_builder_add_dependencies(builder, dependencies, SIZEOF_ARRAY(dependencies));

	return builder;
}

static vulkan_render_pass_create_info_builder_t* build_depth_clear_pass_create_info(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_create_info_builder_t* builder = vulkan_render_pass_create_info_builder_create(renderer->allocator);
	vulkan_render_pass_create_info_builder_add(builder, 1);
	vulkan_render_pass_create_info_builder_bind(builder, 0);

	vulkan_render_pass_create_info_builder_set_framebuffer_count(builder, 1);
	
	/* depth attachment must be at the last */
	VkAttachmentDescription depth_attachment =
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
	vulkan_render_pass_create_info_builder_add_attachment_descriptions(builder, &depth_attachment, 1);

	vulkan_attachment_next_pass_usage_t depth_attachment_usage = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_NONE;
	vulkan_render_pass_create_info_builder_add_attachment_usages(builder, &depth_attachment_usage, 1);

	vulkan_subpass_create_info_builder_t* subpass_builder = vulkan_subpass_create_info_builder_create(renderer->allocator);
	vulkan_subpass_create_info_builder_add(subpass_builder, 1);
	vulkan_subpass_create_info_builder_bind(subpass_builder, 0);


	vulkan_subpass_create_info_builder_set_bind_point(subpass_builder, VK_PIPELINE_BIND_POINT_GRAPHICS);
	vulkan_subpass_create_info_builder_set_depth_stencil_attachment(subpass_builder, (VkAttachmentReference)
																					{
																						.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
																						.attachment = 0
																					});
	vulkan_render_pass_create_info_builder_set_subpasses_builder(builder, subpass_builder, true);
	VkSubpassDependency dependencies[] =
	{
		{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.dstSubpass = 0,
			.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
		},
		{
			.srcSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.dstSubpass = VK_SUBPASS_EXTERNAL,
			.dstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
		}
	};
	vulkan_render_pass_create_info_builder_add_dependencies(builder, dependencies, SIZEOF_ARRAY(dependencies));

	return builder;
}

static vulkan_render_pass_create_info_builder_t* build_swapchain_clear_pass_create_info(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_create_info_builder_t* builder = vulkan_render_pass_create_info_builder_create(renderer->allocator);
	vulkan_render_pass_create_info_builder_add(builder, 1);
	vulkan_render_pass_create_info_builder_bind(builder, 0);

	vulkan_render_pass_create_info_builder_set_framebuffer_count(builder, renderer->swapchain->image_count);
	vulkan_render_pass_create_info_builder_set_supplementary_attachment_bucket(builder, 1, renderer->swapchain->image_count);
	vulkan_render_pass_create_info_builder_set_supplementary_attachments(builder, 0, renderer->swapchain->vo_image_views, renderer->swapchain->image_count);
	
	/* swapchain color attachment must be first */
	VkAttachmentDescription swapchain_color_attachment =
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
	vulkan_render_pass_create_info_builder_add_attachment_descriptions(builder, &swapchain_color_attachment, 1);

	vulkan_attachment_next_pass_usage_t color_attachment_usage = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_PRESENT;
	vulkan_render_pass_create_info_builder_add_attachment_usages(builder, &color_attachment_usage, 1);

	vulkan_subpass_create_info_builder_t* subpass_builder = vulkan_subpass_create_info_builder_create(renderer->allocator);
	vulkan_subpass_create_info_builder_add(subpass_builder, 1);
	vulkan_subpass_create_info_builder_bind(subpass_builder, 0);


	vulkan_subpass_create_info_builder_set_bind_point(subpass_builder, VK_PIPELINE_BIND_POINT_GRAPHICS);
	VkAttachmentReference color_attachment_reference =
	{
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.attachment = 0
	};
	vulkan_subpass_create_info_builder_add_color_attachments(subpass_builder, &color_attachment_reference, 1);

	vulkan_render_pass_create_info_builder_set_subpasses_builder(builder, subpass_builder, true);
	VkSubpassDependency dependencies[] = 
	{
		{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.dstSubpass = 0,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
		},
		{
			.srcSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstSubpass = VK_SUBPASS_EXTERNAL,
			.dstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
		}
	};

	vulkan_render_pass_create_info_builder_add_dependencies(builder, dependencies, SIZEOF_ARRAY(dependencies));

	return builder;
}

static vulkan_render_pass_handle_t create_swapchain_depth_clear_pass(vulkan_renderer_t* renderer, vulkan_render_pass_input_info_t* input)
{
	vulkan_render_pass_pool_create_path(renderer->render_pass_pool);
	AUTO create_info_builder = build_swapchain_depth_clear_pass_create_info(renderer);
	AUTO handle = vulkan_render_pass_pool_create_pass(renderer->render_pass_pool, vulkan_render_pass_create_info_builder_get(create_info_builder), input);
	vulkan_render_pass_create_info_builder_destroy(create_info_builder);
	return handle;
}

static vulkan_render_pass_handle_t create_depth_clear_pass(vulkan_renderer_t* renderer, vulkan_render_pass_input_info_t* input)
{
	vulkan_render_pass_pool_create_path(renderer->render_pass_pool);
	AUTO create_info_builder = build_depth_clear_pass_create_info(renderer);
	AUTO handle = vulkan_render_pass_pool_create_pass(renderer->render_pass_pool, vulkan_render_pass_create_info_builder_get(create_info_builder), input);
	vulkan_render_pass_create_info_builder_destroy(create_info_builder);
	return handle;
}

static vulkan_render_pass_handle_t create_swapchain_clear_pass(vulkan_renderer_t* renderer, vulkan_render_pass_input_info_t* input)
{
	vulkan_render_pass_pool_create_path(renderer->render_pass_pool);
	AUTO create_info_builder = build_swapchain_clear_pass_create_info(renderer);
 	AUTO handle = vulkan_render_pass_pool_create_pass(renderer->render_pass_pool, vulkan_render_pass_create_info_builder_get(create_info_builder), input);
	vulkan_render_pass_create_info_builder_destroy(create_info_builder);
 	return handle;
}

SGE_API vulkan_camera_handle_t vulkan_camera_system_create_camera(vulkan_camera_system_t* system, vulkan_camera_projection_type_t projection_type)
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

SGE_API vulkan_camera_t* vulkan_camera_system_getH(vulkan_camera_system_t* system, vulkan_camera_handle_t handle)
{
	return CAST_TO(vulkan_camera_t*, library_getH(&system->lib, handle));
}

SGE_API void vulkan_camera_system_destroy_cameraH(vulkan_camera_system_t* system, vulkan_camera_handle_t handle)
{
	vulkan_camera_t* camera = vulkan_camera_system_getH(system, handle);
	vulkan_camera_destroy(camera);
	vulkan_camera_release_resources(camera);
	library_destroy_slotH(&system->lib, handle);
}

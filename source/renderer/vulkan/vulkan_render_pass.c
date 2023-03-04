
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>
#include <renderer/internal/vulkan/vulkan_framebuffer.h>
#include <renderer/internal/vulkan/vulkan_camera_system.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/render_window.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/debug.h>

RENDERER_API vulkan_render_pass_t* vulkan_render_pass_new(memory_allocator_t* allocator)
{
	vulkan_render_pass_t* render_pass = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS, vulkan_render_pass_t);
	memzero(render_pass, vulkan_render_pass_t);
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

static VkImageLayout get_attachment_layout(VkFormat format)
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

static void recreate_allocated_attachments(void* _window, void* user_data)
{
	AUTO window = CAST_TO(render_window_t*, _window);
	vulkan_attachment_refresh_info_t info = 
	{
		.width = window->width,
		.height = window->height
	};

	AUTO render_pass = CAST_TO(vulkan_render_pass_t*, user_data);
	for(u32 i = 0; i < render_pass->allocated_attachment_count; i++)
		vulkan_attachment_refresh(&render_pass->allocated_attachments[i], &info);
	debug_log_info("%lu Allocated attachment recreate success", render_pass->allocated_attachment_count);
}

RENDERER_API void vulkan_render_pass_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_create_info_t* create_info, vulkan_render_pass_t OUT render_pass)
{
	memzero(render_pass, vulkan_render_pass_t);

	render_pass->renderer = renderer;
	render_pass->instance_count = 0;
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

	_debug_assert__(create_info->attachment_description_count > 0);
	// VkAttachmentDescription* attachment_descriptions = heap_newv(VkAttachmentDescription, create_info->attachment_description_count);
	// memcopyv(attachment_descriptions, create_info->attachment_descriptions, VkAttachmentDescription, create_info->attachment_description_count);
	
	VkRenderPassCreateInfo render_pass_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pAttachments = create_info->attachment_descriptions,
		.attachmentCount = create_info->attachment_description_count,
		.subpassCount = create_info->subpass_count,
		.pSubpasses = subpasses,
		.dependencyCount = create_info->subpass_dependency_count,
		.pDependencies = create_info->subpass_dependencies
	};
	vkCall(vkCreateRenderPass(renderer->logical_device->vo_handle, &render_pass_create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &render_pass->vo_handle));
	log_ptr(render_pass->vo_handle);
	// heap_free(attachment_descriptions);
	memory_allocator_dealloc(renderer->allocator, subpasses);

	// create attachments
	render_pass->supplementary_attachment_bucket_count = create_info->supplementary_attachment_bucket_count;
	render_pass->supplementary_attachment_bucket_depth = create_info->supplementary_attachment_bucket_depth;
	render_pass->supplementary_attachment_count = create_info->supplementary_attachment_count;
	_debug_assert__((render_pass->supplementary_attachment_bucket_count * render_pass->supplementary_attachment_bucket_depth) == render_pass->supplementary_attachment_count);

	render_pass->vo_supplementary_attachments = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW_ARRAY, VkImageView, render_pass->supplementary_attachment_count);
	memcopyv(render_pass->vo_supplementary_attachments, create_info->vo_supplementary_attachments, VkImageView, render_pass->supplementary_attachment_count);
	
	render_pass->attachment_count = create_info->attachment_description_count;
	render_pass->allocated_attachment_count = CAST_TO(s32, render_pass->attachment_count) - CAST_TO(s32, render_pass->supplementary_attachment_bucket_count);
	_debug_assert__(render_pass->allocated_attachment_count >= 0);
	render_pass->allocated_attachments = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_ARRAY, vulkan_attachment_t, render_pass->allocated_attachment_count);
	if(render_pass->allocated_attachment_count >= 1)
		safe_memzerov(render_pass->allocated_attachments, vulkan_attachment_t, render_pass->allocated_attachment_count);
	for(u32 i = 0; i < render_pass->allocated_attachment_count; i++)
	{
		vulkan_attachment_create_info_t attachment_create_info = 
		{
			.format = create_info->attachment_descriptions[i + render_pass->supplementary_attachment_bucket_count].format,
			.width = renderer->window->width,
			.height = renderer->window->height,
			.next_pass_usage = create_info->attachment_usages[i + render_pass->supplementary_attachment_bucket_count]
		};
		vulkan_attachment_create_no_alloc(renderer, &attachment_create_info, &render_pass->allocated_attachments[i]);
	}

	// create clear values for each attachment in this render pass
	render_pass->vo_clear_values = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_CLEAR_VALUE_ARRAY, VkClearValue, render_pass->attachment_count);
	safe_memzerov(render_pass->vo_clear_values, VkClearValue, render_pass->attachment_count);
	render_pass->vo_formats = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_FORMAT_ARRAY, VkFormat, render_pass->attachment_count);
	for(u32 i = 0; i < render_pass->attachment_count; i++)
	{
		render_pass->vo_formats[i] = create_info->attachment_descriptions[i].format;
		render_pass->vo_clear_values[i] = get_clear_value_from_format(create_info->attachment_descriptions[i].format);
	}

	// create render set layout & render set
	vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(renderer, create_info->render_set_bindings, create_info->render_set_binding_count, &render_pass->render_set_layout);
	vulkan_descriptor_set_create_info_t set_create_info = 
	{
		.vo_pool = renderer->vo_descriptor_pool,
		.layout = &render_pass->render_set_layout
	};

	// create sub render set layouts & subb render sets
	render_pass->sub_render_set_layouts = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_DESCRIPTOR_SET_LAYOUT_ARRAY, vulkan_descriptor_set_layout_t, create_info->subpass_count);
	render_pass->sub_render_sets = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_DESCRIPTOR_SET_ARRAY, vulkan_descriptor_set_t, create_info->subpass_count);
	for(int i = 0; i < create_info->subpass_count; i++)
	{
		vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(renderer, create_info->subpasses[i].sub_render_set_bindings, create_info->subpasses[i].sub_render_set_binding_count, &render_pass->sub_render_set_layouts[i]);
		vulkan_descriptor_set_create_info_t set_create_info = 
		{
			.vo_pool = renderer->vo_descriptor_pool,
			.layout = &render_pass->sub_render_set_layouts[i]
		};
		vulkan_descriptor_set_create_no_alloc(renderer, &set_create_info, &render_pass->sub_render_sets[i]);
	}

	// register this render pass to all the cameras
	vulkan_camera_system_t* camera_system = renderer->camera_system;
	u32 camera_count = vulkan_camera_system_get_count(camera_system);
	for(u32 i = 0; i < camera_count; i++)
		render_pass->framebuffer_list_handle = vulkan_camera_register_render_pass(vulkan_camera_system_get_at(camera_system, i), render_pass);

	event_subscription_create_info_t subscription =
	{
		.handler = EVENT_HANDLER(recreate_allocated_attachments),
		.handler_data = (void*)render_pass,
		.wait_for = SIGNAL_NOTHING_BIT,
		.signal = SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT
	};
	render_pass->attachment_recreate_handle = event_subscribe(renderer->window->on_resize_event, &subscription);
}

RENDERER_API void vulkan_render_pass_destroy(vulkan_render_pass_t* render_pass)
{
	event_unsubscribe(render_pass->renderer->window->on_resize_event, render_pass->attachment_recreate_handle);

	// unregister this render pass from the all the cameras
	// TODO

	// destory the vulkan render pass object
	vkDestroyRenderPass(render_pass->renderer->logical_device->vo_handle, render_pass->vo_handle, VULKAN_ALLOCATION_CALLBACKS(render_pass->renderer));
	
	// destroy the vulkan attachments
	for(u32 i = 0; i < render_pass->allocated_attachment_count; i++)
		vulkan_attachment_destroy(&render_pass->allocated_attachments[i]);

	// destroy the vulkan descriptor sets and layouts (sub render sets and layouts)
	for(u32 i = 0; i < render_pass->subpass_count; i++)
	{
		vulkan_descriptor_set_destroy(&render_pass->sub_render_sets[i]);
		vulkan_descriptor_set_layout_destroy(&render_pass->sub_render_set_layouts[i]);
	}

	// destroy the vulkan descriptor sets and layouts (render set and layout)
	vulkan_descriptor_set_layout_destroy(&render_pass->render_set_layout);
}

RENDERER_API void vulkan_render_pass_release_resources(vulkan_render_pass_t* render_pass)
{
	memory_allocator_dealloc(render_pass->renderer->allocator, render_pass->vo_supplementary_attachments);
	memory_allocator_dealloc(render_pass->renderer->allocator, render_pass->vo_clear_values);
	memory_allocator_dealloc(render_pass->renderer->allocator, render_pass->vo_formats);
	// TODO
	// heap_free(render_pass);
}

static INLINE u32 min(u32 v1, u32 v2) { return (v1 > v2) ? v2 : v1; }

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

	vulkan_framebuffer_t* framebuffers = vulkan_camera_get_framebuffer_list(camera, render_pass->framebuffer_list_handle);
	// begin the render pass
	VkRenderPassBeginInfo render_pass_begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderArea.offset = (VkOffset2D) { 0, 0 },
		.renderArea.extent = (VkExtent2D) { render_pass->renderer->window->width, render_pass->renderer->window->height },
		.framebuffer = framebuffers[framebuffer_index].vo_handle,
		.renderPass = render_pass->vo_handle,
		.clearValueCount = render_pass->clear_value_count,
		.pClearValues = render_pass->vo_clear_values
	};
	vkCmdBeginRenderPass(render_pass->renderer->vo_command_buffers[render_pass->renderer->swapchain->current_image_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	render_pass->current_subpass_index = 0;
}

RENDERER_API void vulkan_render_pass_end(vulkan_render_pass_t* render_pass)
{
	vkCmdEndRenderPass(render_pass->renderer->vo_command_buffers[render_pass->renderer->swapchain->current_image_index]);
}

RENDERER_API void vulkan_render_pass_next(vulkan_render_pass_t* render_pass)
{
	if(render_pass->current_subpass_index >= render_pass->subpass_count)
	{
		log_wrn("There are only %u subpasses but you're trying to exceed it\n", render_pass->subpass_count);
		return;
	}
	vkCmdNextSubpass(render_pass->renderer->vo_command_buffers[render_pass->renderer->swapchain->current_image_index], VK_SUBPASS_CONTENTS_INLINE);
	++(render_pass->current_subpass_index);
}

RENDERER_API void vulkan_render_pass_refresh(vulkan_render_pass_t* render_pass, vulkan_render_pass_refresh_info_t* info)
{
	if((info->supplementary_attachment_bucket_count != render_pass->supplementary_attachment_bucket_count)
		|| (info->supplementary_attachment_bucket_depth != render_pass->supplementary_attachment_bucket_depth))
	{
		memory_allocator_dealloc(render_pass->renderer->allocator, render_pass->vo_supplementary_attachments);
		render_pass->supplementary_attachment_bucket_count = info->supplementary_attachment_bucket_count;
		render_pass->supplementary_attachment_bucket_depth = info->supplementary_attachment_bucket_depth;
		render_pass->supplementary_attachment_count = info->supplementary_attachment_count;
		_debug_assert__((render_pass->supplementary_attachment_bucket_count * render_pass->supplementary_attachment_bucket_depth) == render_pass->supplementary_attachment_count);
		render_pass->vo_supplementary_attachments = memory_allocator_alloc_obj_array(render_pass->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW_ARRAY, VkImageView, render_pass->supplementary_attachment_count);
	}
	_debug_assert__((render_pass->supplementary_attachment_bucket_count * render_pass->supplementary_attachment_bucket_depth) == render_pass->supplementary_attachment_count);
	memcopyv(render_pass->vo_supplementary_attachments, info->vo_supplementary_attachments, VkImageView, render_pass->supplementary_attachment_count);
}

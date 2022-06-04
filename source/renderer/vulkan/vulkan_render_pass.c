
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>
#include <renderer/render_window.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>
#include <renderer/debug.h>

RENDERER_API vulkan_render_pass_t* vulkan_render_pass_new()
{
	vulkan_render_pass_t* render_pass = heap_new(vulkan_render_pass_t);
	memset(render_pass, 0, sizeof(vulkan_render_pass_t));
	return render_pass;
}

RENDERER_API vulkan_render_pass_t* vulkan_render_pass_create(vulkan_renderer_t* renderer, vulkan_render_pass_create_info_t* create_info)
{
	vulkan_render_pass_t* render_pass = vulkan_render_pass_new();
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

RENDERER_API void vulkan_render_pass_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_create_info_t* create_info, vulkan_render_pass_t OUT render_pass)
{
	memzero(render_pass, vulkan_render_pass_t);

	render_pass->renderer = renderer;
	render_pass->subpass_count = create_info->subpass_count;
	render_pass->current_subpass_index = 0;
	render_pass->handle = VULKAN_RENDER_PASS_HANDLE_INVALID;

	assert(create_info->subpass_count > 0);
	VkSubpassDescription* subpasses = heap_newv(VkSubpassDescription, create_info->subpass_count);
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

	assert(create_info->attachment_description_count > 0);
	// VkAttachmentDescription* attachment_descriptions = heap_newv(VkAttachmentDescription, create_info->attachment_description_count);
	// memcpy(attachment_descriptions, create_info->attachment_descriptions, sizeof(VkAttachmentDescription) * create_info->attachment_description_count);
	
	VkRenderPassCreateInfo render_pass_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pAttachments = create_info->attachment_descriptions,
		.attachmentCount = create_info->attachment_description_count,
		.subpassCount = create_info->subpass_count,
		.pSubpasses = subpasses
	};
	vkCall(vkCreateRenderPass(renderer->logical_device->vo_handle, &render_pass_create_info, NULL, &render_pass->vo_handle));
	// heap_free(attachment_descriptions);
	heap_free(subpasses);

	// create attachments
	render_pass->attachment_count = create_info->attachment_description_count;
	render_pass->supplementary_attachment_count = create_info->supplementary_attachment_count;
	render_pass->supplementary_attachments = create_info->supplementary_attachments;
	s32 num_attachment_to_be_created = CAST_TO(s32, render_pass->attachment_count) - CAST_TO(s32, render_pass->supplementary_attachment_count);
	assert(num_attachment_to_be_created >= 0);
	render_pass->attachments = heap_newv(vulkan_attachment_t, num_attachment_to_be_created);
	memzerov(render_pass->attachments, vulkan_attachment_t, num_attachment_to_be_created);
	for(u32 i = 0; i < num_attachment_to_be_created; i++)
	{
		vulkan_attachment_create_info_t attachment_create_info = 
		{
			.format = create_info->attachment_descriptions[i + render_pass->supplementary_attachment_count].format,
			.width = renderer->window->width,
			.height = renderer->window->height,
			.next_pass_usage = create_info->attachment_usages[i + render_pass->supplementary_attachment_count]
		};
		vulkan_attachment_create_no_alloc(renderer, &attachment_create_info, &render_pass->attachments[i]);
	}

	// create framebuffers
	render_pass->framebuffer_count = create_info->framebuffer_count;
	render_pass->vo_framebuffers = heap_newv(VkFramebuffer, render_pass->framebuffer_count);
	VkImageView* image_views = heap_newv(VkImageView, render_pass->attachment_count);
	s32 num_created_image_views = num_attachment_to_be_created;
	for(u32 i = 0; i < num_created_image_views; i++)
		image_views[i + render_pass->supplementary_attachment_count] = render_pass->attachments[i].image_view.vo_handle;

	for(u32 i = 0; i < render_pass->framebuffer_count; i++)
	{
		for(u32 j = 0; j < render_pass->supplementary_attachment_count; j++)
			image_views[j] = render_pass->supplementary_attachments[j + i * render_pass->supplementary_attachment_count];

		VkFramebufferCreateInfo framebuffer_create_info = 
		{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = render_pass->vo_handle,
			.attachmentCount = render_pass->attachment_count,
			.pAttachments = image_views,
			.width = renderer->window->width,
			.height = renderer->window->height,
			.layers = 1
		};
		vkCall(vkCreateFramebuffer(renderer->logical_device->vo_handle, &framebuffer_create_info, NULL, &render_pass->vo_framebuffers[i]));
	}
	heap_free(image_views);

	// create clear values for each attachment in this render pass
	render_pass->vo_clear_values = heap_newv(VkClearValue, render_pass->attachment_count);
	memzerov(render_pass->vo_clear_values, VkClearValue, render_pass->attachment_count);
	render_pass->vo_formats = heap_newv(VkFormat, render_pass->attachment_count);
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
	vulkan_descriptor_set_create_no_alloc(renderer, &set_create_info, &render_pass->render_set);

	// create sub render set layouts & subb render sets
	render_pass->sub_render_set_layouts = heap_newv(vulkan_descriptor_set_layout_t, create_info->subpass_count);
	render_pass->sub_render_sets = heap_newv(vulkan_descriptor_set_t, create_info->subpass_count);
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
}

RENDERER_API void vulkan_render_pass_destroy(vulkan_render_pass_t* render_pass)
{

	// destory the vulkan render pass object
	vkDestroyRenderPass(render_pass->renderer->logical_device->vo_handle, render_pass->vo_handle, NULL);
	
	// destroy the vulkan attachments
	s32 num_attachment_to_be_destroyed = CAST_TO(s32, render_pass->attachment_count) - CAST_TO(s32, render_pass->supplementary_attachment_count);
	for(u32 i = 0; i < num_attachment_to_be_destroyed; i++)
		vulkan_attachment_destroy(&render_pass->attachments[i]);

	// destroy the vulkan framebuffers
	for(u32 i = 0; i < render_pass->framebuffer_count; i++)
		vkDestroyFramebuffer(render_pass->renderer->logical_device->vo_handle, render_pass->vo_framebuffers[i], NULL);

	// destroy the vulkan descriptor sets and layouts (sub render sets and layouts)
	for(u32 i = 0; i < render_pass->subpass_count; i++)
	{
		vulkan_descriptor_set_destroy(&render_pass->sub_render_sets[i]);
		vulkan_descriptor_set_layout_destroy(&render_pass->sub_render_set_layouts[i]);
	}

	// destroy the vulkan descriptor sets and layouts (render set and layout)
	vulkan_descriptor_set_destroy(&render_pass->render_set);
	vulkan_descriptor_set_layout_destroy(&render_pass->render_set_layout);
}

RENDERER_API void vulkan_render_pass_release_resources(vulkan_render_pass_t* render_pass)
{
	heap_free(render_pass->vo_clear_values);
	heap_free(render_pass->vo_formats);
	// TODO
	// heap_free(render_pass);
}

static INLINE u32 min(u32 v1, u32 v2) { return (v1 > v2) ? v2 : v1; }

RENDERER_API void vulkan_render_pass_set_clear(vulkan_render_pass_t* render_pass, color_t color, float depth)
{
	for(u32 i = 0; i < render_pass->attachment_count; i++)
	{
		switch(render_pass->vo_formats[i])
		{
			case VK_FORMAT_D32_SFLOAT:
				render_pass->vo_clear_values[i] = (VkClearValue) { .depthStencil = { depth, 0UL } };
				break;
			case VK_FORMAT_B8G8R8A8_SRGB:
				render_pass->vo_clear_values[i] = (VkClearValue) { .color = { .float32 = 
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

RENDERER_API void vulkan_render_pass_begin(vulkan_render_pass_t* render_pass, u32 framebuffer_index)
{
	framebuffer_index = (framebuffer_index == VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN) ? render_pass->renderer->swapchain->current_image_index : framebuffer_index;
	framebuffer_index = min(render_pass->framebuffer_count - 1, framebuffer_index);
	// begin the render pass
	VkRenderPassBeginInfo render_pass_begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderArea.offset = (VkOffset2D) { 0, 0 },
		.renderArea.extent = (VkExtent2D) { render_pass->renderer->window->width, render_pass->renderer->window->height },
		.framebuffer = render_pass->vo_framebuffers[framebuffer_index],
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

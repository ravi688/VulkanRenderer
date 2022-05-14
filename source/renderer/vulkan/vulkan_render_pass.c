#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>

#ifdef GLOBAL_DEBUG
	static void check_pre_condition(vulkan_render_pass_t*);
#else
#	define check_pre_condition(render_pass)
#endif // GLOBAL_DEBUG

RENDERER_API vulkan_render_pass_t* vulkan_render_pass_new()
{
	vulkan_render_pass_t* render_pass = heap_new(vulkan_render_pass_t);
	memset(render_pass, 0, sizeof(vulkan_render_pass_t));
	return render_pass;
}

RENDERER_API vulkan_render_pass_t* vulkan_render_pass_create(vulkan_renderer_t* context, vulkan_render_pass_create_info_t* create_info)
{
	vulkan_render_pass_t* render_pass = vulkan_render_pass_new();
	render_pass->context = context;
	render_pass->subpass_count = create_info->subpass_count;
	render_pass->current_subpass_index = 0;

	VkSubpasssDescription* subpasses = heap_newv(VkSubpassDescription, create_info->subpass_count);
	for(u32 i = 0; i < create_info->subpass_count; i++)
	{
		subpasses[i] = (VkSubpassDescription)
		{
			.pipelineBindPoint = create_info->subpasses[i].pipeline_bind_point,
			.inputAttachmentCount = create_info->subpasses[i].input_attachment_count,
			.pInputAttachments = create_info->subpasses[i].input_attachments,
			.pDepthStencilAttachment = create_info->subpasses[i].depth_stencil_attachment,
			.preserveAttachmentCount = create_info->subpasses[i].preserve_attachment_count,
			.pPreserveAttachments = create_info->subpasses[i].preserve_attachments
		};
	}

	VkRenderPassCreateInfo render_pass_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pAttachments = create_info->attachment_descriptions,
		.attachmentCount = create_info->attachment_description_count,
		.subpassCount = create_info->subpass_count
		.pSubpasses = subpasses
	}
	vkCall(vkCreateRenderPass(context->logical_device->handle, &render_pass_create_info, NULL, &render_pass->handle));
	heap_free(subpasses);

	// create descriptor sets (render set for each sub pass)
	render_pass->sets = heap_newv(vulkan_descriptor_set_t, create_info->subpass_count);
	memset(render_pass->sets, 0, sizeof(vulkan_descriptor_set_t) * create_info->subpass_count);
	for(int i = 0; i < create_info->subpass_count; i++)
		vulkan_descriptor_set_create_no_alloc(context, &create_info->subpasses[i], &render_pass->sets[i]);

	return render_pass;
}

RENDERER_API void vulkan_render_pass_destroy(vulkan_render_pass_t* render_pass)
{
	check_pre_condition(render_pass);
	vkDestroyRenderPass(render_pass->context->logical_device->handle, render_pass->handle, NULL);
}

RENDERER_API void vulkan_render_pass_release_resources(vulkan_render_pass_t* render_pass)
{
	check_pre_condition(render_pass);
	heap_free(render_pass);
}

RENDERER_API void vulkan_render_pass_begin(vulkan_render_pass_t* render_pass, vulkan_render_pass_begin_info_t* begin_info)
{
	check_pre_condition(render_pass);

	// calculate the clear value count
	u32 clear_count = begin_info->clear_color_count + (begin_info->is_depth ? 1 : 0);
	assert(render_pass->attachment_count == clear_count);

	// create the clear values array
	VkClearValue clear_values[clear_count];
	u32 i = 0;
	if(begin_info->is_depth)
	{
		clear_values[i].depthStencil.depth = begin_info->clear_depth;
		clear_values[i].depthStencil.stencil = 0.0f;
		i++;
	}
	for(int offset = i; i < clear_count; i++)
	{
		clear_values[i].color.float32[0] = begin_info->clear_colors[i - offset].r;
		clear_values[i].color.float32[1] = begin_info->clear_colors[i - offset].g;
		clear_values[i].color.float32[2] = begin_info->clear_colors[i - offset].b;
		clear_values[i].color.float32[3] = begin_info->clear_colors[i - offset].a;
	}

	// begin the render pass
	VkRenderPassBeginInfo render_pass_begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderArea.offset = (VkOffset2D) { 0, 0 },
		.renderArea.extent = (VkExtent2D) { begin_info->width, begin_info->height },
		.framebuffer = begin_info->framebuffer,
		.renderPass = render_pass->handle,
		.clearValueCount = clear_count,
		.pClearValues = &clear_values[0]
	};
	vkCmdBeginRenderPass(render_pass->context->vk_command_buffers[render_pass->context->swapchain->current_image_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

RENDERER_API void vulkan_render_pass_end(vulkan_render_pass_t* render_pass)
{
	check_pre_condition(render_pass);
	vkCmdEndRenderPass(render_pass->context->vk_command_buffers[render_pass->context->swapchain->current_image_index]);
}

RENDERER_API void vulkan_render_pass_next(vulkan_render_pass_t* render_pass)
{
	check_pre_condition(render_pass);
	if(render_pass->current_subpass_index >= render_pass->subpass_count)
	{
		log_wrn("There are only %u subpasses but you're trying to exceed it\n", render_pass->subpass_count);
		return;
	}
	vkCmdNextSubpass(render_pass->context->vk_command_buffers[render_pass->context->swapchain->current_image_index], VK_SUBPASS_CONTENTS_INLINE);
	++(render_pass->current_subpass_index);
}

RENDERER_API u32 vulkan_render_pass_get_subpass_count(vulkan_render_pass_t* render_pass)
{
	check_pre_condition(render_pass);
	return render_pass->subpass_count;
}

RENDERER_API void vulkan_render_pass_bind_set(vulkan_render_pass_t* render_pass, u32 subpass_index, vulkan_pipeline_layout_t* pipeline_layout)
{
	check_pre_condition(render_pass);
	assert(subpass_index < render_pass->subpass_count);
	switch(subpass_index)
	{
		case VULKAN_SUBPASS_INDEX_CURRENT:
			subpass_index = render_pass->current_subpass_index;
			break;
	}
	vulkan_descriptor_set_bind(render_pass->sets[subpass_index], render_pass->context, VULKAN_DESCRIPTOR_SET_RENDER, pipeline_layout->handle);
}

RENDERER_API vulkan_descriptor_set_t* vulkan_render_pass_get_set(vulkan_render_pass_t* render_pass, u32 subpass_index)
{
	check_pre_condition(render_pass);
	if(subpass_index >= render_pass->subpass_count)
	{
		log_wrn("subpass_index >= render_pass->subpass_count, returning NULL\n");
		return NULL;
	}
	return render_pass->sets[subpass_index];
}

RENDERER_API u64 vulkan_render_pass_create_info_get_hash(vulkan_render_pass_create_info_t* create_info)
{
	NOT_IMPLEMENTED_FUNCTION();
}

#ifdef GLOBAL_DEBUG

static void check_pre_condition(vulkan_render_pass_t* render_pass)
{
	assert(render_pass != NULL);
	assert(render_pass->context != NULL);
}

#endif // GLOBAL_DEBUG

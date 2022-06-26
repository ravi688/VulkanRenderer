
#include <renderer/internal/vulkan/vulkan_framebuffer.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/render_window.h>

#include <renderer/memory_allocator.h>

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_new()
{
	vulkan_framebuffer_t* framebuffer = heap_new(vulkan_framebuffer_t);
	memzero(framebuffer, vulkan_framebuffer_t);
	return framebuffer;
}

static void vulkan_framebuffer_recreate(vulkan_framebuffer_t* framebuffer, VkImageView override_view)
{
	if(framebuffer->vo_handle != VK_NULL_HANDLE)
		vulkan_framebuffer_destroy(framebuffer);
	
	AUTO render_pass = framebuffer->pass;
	AUTO image_views = framebuffer->image_views;

	s32 num_created_image_views = CAST_TO(s32, render_pass->attachment_count) - CAST_TO(s32, render_pass->supplementary_attachment_count);;
	for(u32 i = 0; i < num_created_image_views; i++)
		image_views[i + render_pass->supplementary_attachment_count] = render_pass->attachments[i].image_view.vo_handle;

	assert(render_pass->supplementary_attachment_count <= 1);
	for(u32 j = 0; j < render_pass->supplementary_attachment_count; j++)
	{
		if(override_view != VK_NULL_HANDLE)
			image_views[0] = override_view;
		else
			image_views[j] = render_pass->supplementary_attachments[j + framebuffer->id * render_pass->supplementary_attachment_count];
	}

	VkFramebufferCreateInfo framebuffer_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = render_pass->vo_handle,
		.attachmentCount = render_pass->attachment_count,
		.pAttachments = image_views,
		.width = framebuffer->renderer->window->width,
		.height = framebuffer->renderer->window->height,
		.layers = 1
	};
	vkCall(vkCreateFramebuffer(framebuffer->renderer->logical_device->vo_handle, &framebuffer_create_info, NULL, &framebuffer->vo_handle));
}

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_create(vulkan_renderer_t* renderer, vulkan_render_pass_t* render_pass, u32 id)
{
	vulkan_framebuffer_t* framebuffer = vulkan_framebuffer_new();
	vulkan_framebuffer_create_no_alloc(renderer, render_pass, id, framebuffer);
	return framebuffer;
}

RENDERER_API void vulkan_framebuffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_t* render_pass, u32 id, vulkan_framebuffer_t OUT framebuffer)
{
	memzero(framebuffer, vulkan_framebuffer_t);
	framebuffer->renderer = renderer;
	framebuffer->vo_handle = VK_NULL_HANDLE;
	framebuffer->pass = render_pass;
	framebuffer->is_supplementary_supported = (render_pass->supplementary_attachments != NULL)
											&& (render_pass->supplementary_attachment_count != 0);
	framebuffer->id = id;
	framebuffer->image_views = heap_newv(VkImageView, render_pass->attachment_count);
	vulkan_framebuffer_recreate(framebuffer, VK_NULL_HANDLE);
}

RENDERER_API void vulkan_framebuffer_destroy(vulkan_framebuffer_t* framebuffer)
{
	vkDestroyFramebuffer(framebuffer->renderer->logical_device->vo_handle, framebuffer->vo_handle, NULL);
}

RENDERER_API void vulkan_framebuffer_release_resources(vulkan_framebuffer_t* framebuffer)
{
	heap_free(framebuffer->image_views);
	// heap_free(framebuffer);
}

RENDERER_API void vulkan_framebuffer_set_supplementary(vulkan_framebuffer_t* framebuffer, vulkan_attachment_t* attachment)
{
	if(!framebuffer->is_supplementary_supported) return;
	vulkan_framebuffer_recreate(framebuffer, attachment->image_view.vo_handle);
}

RENDERER_API void vulkan_framebuffer_restore_supplementary(vulkan_framebuffer_t* framebuffer)
{
	if(!framebuffer->is_supplementary_supported) return;
	vulkan_framebuffer_recreate(framebuffer, VK_NULL_HANDLE);
}


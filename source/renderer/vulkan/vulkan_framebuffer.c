
#include <renderer/internal/vulkan/vulkan_framebuffer.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>
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

static void vulkan_framebuffer_recreate(vulkan_framebuffer_t* framebuffer)
{
	if(framebuffer->vo_handle != VK_NULL_HANDLE)
		vulkan_framebuffer_destroy(framebuffer);
	
	AUTO render_pass = framebuffer->pass;
	AUTO image_views = framebuffer->image_views;

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
	framebuffer->id = id;
	framebuffer->image_views = heap_newv(VkImageView, render_pass->attachment_count);

	framebuffer->is_supplementary_supported = (render_pass->supplementary_attachments != NULL)
											&& (render_pass->supplementary_attachment_count != 0);
    framebuffer->depth_index = U32_MAX;
    framebuffer->is_depth_supported = false;

    // for now we must not touch the depth only passes because they are being sampled in other passes
    // or if(render_pass->type == VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET)
   	

    // TODO: Revert this change after implementing and testing the point light shadows
   	// if(render_pass->required_framebuffer_count == renderer->swapchain->image_count)
   	// {
		u32 attachment_count = render_pass->attachment_count;
#ifdef GLOBAL_DEBUG
		u32 depth_count = 0;
#endif
		for(u32 i = 0; i < attachment_count; i++)
		{
			if(render_pass->attachments[i].image.vo_format == VK_FORMAT_D32_SFLOAT)
			{
#ifdef GLOBAL_DEBUG
				depth_count++;
#endif
				framebuffer->is_depth_supported = true;
				framebuffer->depth_index = i + render_pass->supplementary_attachment_count;
			}
		}

#ifdef GLOBAL_DEBUG
		assert_wrn(depth_count <= 1);
#endif
	// }

	assert(render_pass->supplementary_attachment_count <= 1);
	for(u32 j = 0; j < render_pass->supplementary_attachment_count; j++)
		framebuffer->image_views[j] = render_pass->supplementary_attachments[j + framebuffer->id * render_pass->supplementary_attachment_count];

	s32 num_created_image_views = CAST_TO(s32, render_pass->attachment_count) - CAST_TO(s32, render_pass->supplementary_attachment_count);;
	for(s32 i = 0; i < num_created_image_views; i++)
		framebuffer->image_views[i + render_pass->supplementary_attachment_count] = render_pass->attachments[i].image_view.vo_handle;

	vulkan_framebuffer_recreate(framebuffer);
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

RENDERER_API void vulkan_framebuffer_set_supplementary(vulkan_framebuffer_t* framebuffer, vulkan_image_view_t* view)
{
	if(!framebuffer->is_supplementary_supported) return;

	// for now supplementary attachment count must be one; just for swapchain color attachment
	assert(framebuffer->pass->supplementary_attachment_count == 1);
	
	framebuffer->image_views[0] = view->vo_handle;
	vulkan_framebuffer_recreate(framebuffer);
}

RENDERER_API void vulkan_framebuffer_restore_supplementary(vulkan_framebuffer_t* framebuffer)
{
	if(!framebuffer->is_supplementary_supported) return;
	vulkan_render_pass_t* render_pass = framebuffer->pass;

	// for now supplementary attachment count must be one; just for swapchain color attachment
	assert(render_pass->supplementary_attachment_count == 1);
	
	framebuffer->image_views[0] = render_pass->supplementary_attachments[framebuffer->id * render_pass->supplementary_attachment_count];
	vulkan_framebuffer_recreate(framebuffer);
}

RENDERER_API void vulkan_framebuffer_set_depth(vulkan_framebuffer_t* framebuffer, vulkan_image_view_t* view)
{
	if(!framebuffer->is_depth_supported) return;

	assert(framebuffer->depth_index != U32_MAX);
	framebuffer->image_views[framebuffer->depth_index] = view->vo_handle;
	vulkan_framebuffer_recreate(framebuffer);
}

RENDERER_API void vulkan_framebuffer_restore_depth(vulkan_framebuffer_t* framebuffer)
{
	if(!framebuffer->is_depth_supported) return;

	assert(framebuffer->depth_index != U32_MAX);

	u32 index = framebuffer->depth_index;
	framebuffer->image_views[index] = framebuffer->pass->attachments[CAST_TO(s32, index) - 1].image_view.vo_handle;
	vulkan_framebuffer_recreate(framebuffer);
}

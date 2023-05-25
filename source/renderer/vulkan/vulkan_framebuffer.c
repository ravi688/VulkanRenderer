
#include <renderer/internal/vulkan/vulkan_framebuffer.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/render_window.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_new(memory_allocator_t* allocator)
{
	vulkan_framebuffer_t* framebuffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_FRAMEBUFFER, vulkan_framebuffer_t);
	memzero(framebuffer, vulkan_framebuffer_t);
	return framebuffer;
}

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_create(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info)
{
	vulkan_framebuffer_t* framebuffer = vulkan_framebuffer_new(renderer->allocator);
	vulkan_framebuffer_create_no_alloc(renderer, create_info, framebuffer);
	return framebuffer;
}

RENDERER_API void vulkan_framebuffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info, vulkan_framebuffer_t OUT framebuffer)
{
	memzero(framebuffer, vulkan_framebuffer_t);
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

RENDERER_API void vulkan_framebuffer_destroy(vulkan_framebuffer_t* framebuffer)
{
	vkDestroyFramebuffer(framebuffer->renderer->logical_device->vo_handle, framebuffer->vo_handle, VULKAN_ALLOCATION_CALLBACKS(framebuffer->renderer));
	framebuffer->vo_handle = VK_NULL_HANDLE;
}

RENDERER_API void vulkan_framebuffer_release_resources(vulkan_framebuffer_t* framebuffer)
{
	// heap_free(framebuffer);
}

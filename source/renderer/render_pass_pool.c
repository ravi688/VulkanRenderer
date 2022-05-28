
#include <renderer/render_pass_pool.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/memory_allocator.h>
#include <renderer/debug.h>

/* constructors & destructors */
RENDERER_API render_pass_pool_t* render_pass_pool_new()
{
	return vulkan_render_pass_pool_new();
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

static vulkan_render_pass_create_info_t* build_swapchain_color_render_pass_create_info(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_create_info_t* create_info = heap_new(vulkan_render_pass_create_info_t);
	memzero(create_info, vulkan_render_pass_create_info_t);
	
	create_info->framebuffer_count = renderer->swapchain->image_count;
	create_info->attachment_count = 1;
	create_info->attachments = heap_new(VkAttachmentDescription);
	create_info->attachments[0] = (VkAttachmentDescription)
	{
		.format = renderer->swapchain->vo_image_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};
	create_info->attachment_usages = heap_new(vulkan_attachment_next_pass_usage_t);
	create_info->attachment_usages[0] = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_NONE;

	create_info->supplementary_attachment_count = 1;
	create_info->supplementary_attachments = renderer->swapchain->vo_image_views;

	VkAttachmentReference* color_attachments = heap_new(VkAttachmentReference);
	color_attachments[0] = (VkAttachmentReference)
	{
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.attachment = 0
	};
	create_info->subpass_count = 1;
	create_info->subpasses = heap_new(vulkan_subpass_create_info_t);
	create_info->subpasses[0] = (vulkan_subpass_create_info_t)
	{
		.pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.color_attachments = color_attachments,
		.color_attachment_count = 1
	};

	return create_info;
}

RENDERER_API render_pass_handle_t render_pass_pool_create_pass_from_preset(render_pass_pool_t* pool, render_pass_pool_pass_preset_t preset)
{
	vulkan_render_pass_create_info_t* create_info;
	switch(preset)
	{
		case RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN:
			create_info = build_swapchain_color_render_pass_create_info(CAST_TO(vulkan_render_pass_t*, pool)->renderer);
			break;
		default:
			return RENDER_PASS_HANDLE_INVALID;
	}
	AUTO handle = vulkan_render_pass_pool_create_pass(pool, create_info);
	heap_free(create_info);
	log_msg("Vulkan render pass has been created from a preset successfully\n");
	return handle;
}

RENDERER_API render_pass_handle_t render_pass_pool_create_pass(render_pass_pool_t* pool, render_pass_create_info_t* create_info)
{
	return vulkan_render_pass_pool_create_pass(pool, create_info);
}

RENDERER_API render_pass_t* render_pass_pool_getH(render_pass_pool_t* pool, render_pass_handle_t handle)
{
	return vulkan_render_pass_pool_getH(pool, handle);
}


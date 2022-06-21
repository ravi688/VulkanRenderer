
#include <renderer/internal/vulkan/vulkan_camera_system.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/memory_allocator.h>


RENDERER_API vulkan_camera_system_t* vulkan_camera_system_new()
{
	vulkan_camera_system_t* system = heap_new(vulkan_camera_system_t);
	memzero(system, vulkan_camera_system_t);
	return system;
}

RENDERER_API vulkan_camera_system_t* vulkan_camera_system_create(vulkan_renderer_t* renderer)
{
	vulkan_camera_system_t* system = vulkan_camera_system_new();
	system->renderer = renderer;
	library_create_no_alloc(&system->lib);
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
	// heap_free(system);
}


static vulkan_render_pass_create_info_t* build_swapchain_color_render_pass_create_info_clear(vulkan_renderer_t* renderer)
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
		.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	create_info->attachment_usages = heap_new(vulkan_attachment_next_pass_usage_t);
	create_info->attachment_usages[0] = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_PRESENT;

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


RENDERER_API vulkan_camera_handle_t vulkan_camera_system_create_camera(vulkan_camera_system_t* system, vulkan_camera_projection_type_t projection_type)
{
	vulkan_camera_create_info_t create_info = 
	{
		.projection_type = projection_type,
		.default_render_pass = vulkan_render_pass_pool_create_pass(system->renderer->render_pass_pool,
			build_swapchain_color_render_pass_create_info_clear(system->renderer))
	};
	return library_create_slot(&system->lib, vulkan_camera_create(system->renderer, &create_info));
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

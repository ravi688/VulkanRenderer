#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>

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

RENDERER_API vulkan_render_pass_t* vulkan_render_pass_create(vulkan_renderer_t* renderer, vulkan_render_pass_create_info_t* create_info)
{
	vulkan_render_pass_t* render_pass = vulkan_render_pass_new();

	render_pass->renderer = renderer;

	// setup color attachment description
	VkAttachmentDescription color_attachment = 
	{
		.format = create_info->color_attachment_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	// setup depth attachment description
	VkAttachmentDescription depth_attachment = 
	{
		.format = create_info->depth_attachment_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	// setup subpass description

	VkAttachmentReference color_attachment_reference =
	{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	VkAttachmentReference depth_attachment_reference = 
	{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
	VkSubpassDescription subpass =
	{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_reference,
		.pDepthStencilAttachment = &depth_attachment_reference
	};

	// setup subpass dependency
	VkSubpassDependency dependency = 
	{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
	};

	// create vulkan render pass
	VkAttachmentDescription attachments[2] = { color_attachment, depth_attachment };
	VkRenderPass renderPass;
 	VkRenderPassCreateInfo render_pass_create_info =
 	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 2,
		.pAttachments = &attachments[0],
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};
	vkCall(vkCreateRenderPass(renderer->logical_device->handle, &render_pass_create_info, NULL, &render_pass->handle));
	return render_pass;
}

RENDERER_API void vulkan_render_pass_destroy(vulkan_render_pass_t* render_pass)
{
	check_pre_condition(render_pass);
	vkDestroyRenderPass(render_pass->renderer->logical_device->handle, render_pass->handle, NULL);
}

RENDERER_API void vulkan_render_pass_release_resources(vulkan_render_pass_t* render_pass)
{
	check_pre_condition(render_pass);
	heap_free(render_pass);
}

RENDERER_API void vulkan_render_pass_begin(vulkan_render_pass_t* render_pass, vulkan_render_pass_begin_info_t* begin_info)
{
	check_pre_condition(render_pass);
	VkClearValue clear_values[2] = 
	{
		{ .color = { .float32 = { begin_info->color.r, begin_info->color.g, begin_info->color.b, begin_info->color.a } } },
		{ .depthStencil = { .depth = begin_info->depth, .stencil = begin_info->stencil } }
	};
	VkRenderPassBeginInfo render_pass_begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderArea.offset = (VkOffset2D) { 0, 0 },
		.renderArea.extent = (VkExtent2D) { begin_info->width, begin_info->height },
		.framebuffer = begin_info->framebuffer,
		.renderPass = render_pass->handle,
		.clearValueCount = 2,
		.pClearValues = &clear_values[0]
	};
	vkCmdBeginRenderPass(render_pass->renderer->vk_command_buffers[render_pass->renderer->swapchain->current_image_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

RENDERER_API void vulkan_render_pass_end(vulkan_render_pass_t* render_pass)
{
	check_pre_condition(render_pass);
	vkCmdEndRenderPass(render_pass->renderer->vk_command_buffers[render_pass->renderer->swapchain->current_image_index]);
}

#ifdef GLOBAL_DEBUG

static void check_pre_condition(vulkan_render_pass_t* render_pass)
{
	assert(render_pass != NULL);
	assert(render_pass->renderer != NULL);
}

#endif // GLOBAL_DEBUG
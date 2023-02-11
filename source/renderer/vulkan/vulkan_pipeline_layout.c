#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/assert.h>

RENDERER_API vulkan_pipeline_layout_t* vulkan_pipeline_layout_new(memory_allocator_t* allocator)
{
	vulkan_pipeline_layout_t* layout = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_PIPELINE_LAYOUT, vulkan_pipeline_layout_t);
	memzero(layout, vulkan_pipeline_layout_t);
	return layout;
}

RENDERER_API void vulkan_pipeline_layout_create_no_alloc(vulkan_renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info, vulkan_pipeline_layout_t* layout)
{
	memzero(layout, vulkan_pipeline_layout_t);

	layout->renderer = renderer;
	VkPipelineLayoutCreateInfo pipeline_layout_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = create_info->set_layout_count,
		.pSetLayouts = create_info->vo_set_layouts,
		.pushConstantRangeCount = create_info->push_constant_range_count,
		.pPushConstantRanges = create_info->vo_push_constant_ranges
	};
	vkCall(vkCreatePipelineLayout(renderer->logical_device->vo_handle, &pipeline_layout_info, NULL, &layout->vo_handle));
}

RENDERER_API vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(vulkan_renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info)
{
	vulkan_pipeline_layout_t* layout = vulkan_pipeline_layout_new(renderer->allocator);
	vulkan_pipeline_layout_create_no_alloc(renderer, create_info, layout);
	return layout;
}


RENDERER_API void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* layout)
{
	vkDestroyPipelineLayout(layout->renderer->logical_device->vo_handle, layout->vo_handle, NULL);
}

RENDERER_API void vulkan_pipeline_layout_release_resources(vulkan_pipeline_layout_t* layout)
{
	heap_free(layout);
}


RENDERER_API void vulkan_pipeline_layout_push_constants(vulkan_pipeline_layout_t* layout, VkShaderStageFlagBits stage_flags, u32 offset, u32 size, void* bytes)
{
	VkCommandBuffer command_buffer = layout->renderer->vo_command_buffers[layout->renderer->swapchain->current_image_index];
	vkCmdPushConstants(command_buffer, layout->vo_handle, stage_flags, offset, size, bytes);
}

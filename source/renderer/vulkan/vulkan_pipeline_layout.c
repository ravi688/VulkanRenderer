#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>
#include <renderer/assert.h>
#include <renderer/debug.h>

RENDERER_API vulkan_pipeline_layout_t* vulkan_pipeline_layout_new()
{
	vulkan_pipeline_layout_t* layout = heap_new(vulkan_pipeline_layout_t);
	memset(layout, 0, sizeof(vulkan_pipeline_layout_t));
	return layout;
}

RENDERER_API void vulkan_pipeline_layout_create_no_alloc(vulkan_renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info, vulkan_pipeline_layout_t* pipeline_layout)
{
	assert(create_info != NULL);
	u32 set_layout_count = create_info->vk_set_layout == VK_NULL_HANDLE ? 0 : 1;
	VkPipelineLayoutCreateInfo pipeline_layout_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = set_layout_count,
		.pSetLayouts = &create_info->vk_set_layout,
		.pushConstantRangeCount = create_info->push_constant_range_count,
		.pPushConstantRanges = create_info->push_constant_ranges
	};
	vkCall(vkCreatePipelineLayout(renderer->logical_device->handle, &pipeline_layout_info, NULL, &pipeline_layout->handle));
}

RENDERER_API vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(vulkan_renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info)
{
	vulkan_pipeline_layout_t* pipeline_layout = vulkan_pipeline_layout_new();
	vulkan_pipeline_layout_create_no_alloc(renderer, create_info, pipeline_layout);
	return pipeline_layout;
}


RENDERER_API void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* pipeline_layout, vulkan_renderer_t* renderer)
{
	vkDestroyPipelineLayout(renderer->logical_device->handle, pipeline_layout->handle, NULL);
}

RENDERER_API void vulkan_pipeline_layout_release_resources(vulkan_pipeline_layout_t* pipeline_layout)
{
	heap_free(pipeline_layout);
}


RENDERER_API void vulkan_pipeline_layout_push_constants(vulkan_pipeline_layout_t* pipeline_layout, vulkan_renderer_t* renderer, VkShaderStageFlagBits stage_flags, u32 offset, u32 size, void* bytes)
{
	VkCommandBuffer command_buffer = renderer->vk_command_buffers[renderer->swapchain->current_image_index];
	vkCmdPushConstants(command_buffer, pipeline_layout->handle, stage_flags, offset, size, bytes);
}

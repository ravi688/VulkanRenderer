
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>

vulkan_pipeline_layout_t* vulkan_pipeline_layout_new()
{
	vulkan_pipeline_layout_t* layout = heap_new(vulkan_pipeline_layout_t);
	memset(layout, 0, sizeof(vulkan_pipeline_layout_t));
	return layout;
}

void vulkan_pipeline_layout_create_no_alloc(renderer_t* renderer, vulkan_pipeline_layout_t* layout)
{
	VkPushConstantRange* push_constant_range = stack_new(VkPushConstantRange);
	push_constant_range->size = sizeof(float) * 16;
	push_constant_range->offset = 0;
	push_constant_range->stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	refp(vulkan_pipeline_layout_t, layout, 0)->layout = vk_get_pipeline_layout(renderer->vk_device, 1, push_constant_range);
	stack_free(push_constant_range);
}

vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(renderer_t* renderer)
{
	vulkan_pipeline_layout_t* layout = vulkan_pipeline_layout_new();
	vulkan_pipeline_layout_create_no_alloc(renderer, refp(vulkan_pipeline_layout_t, layout, 0));
	return layout;
}


void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* layout, renderer_t* renderer)
{
	vkDestroyPipelineLayout(renderer->vk_device, refp(vulkan_pipeline_layout_t, layout, 0)->layout, NULL);
}

void vulkan_pipeline_release_resources(vulkan_pipeline_layout_t* layout)
{
	heap_free(layout);
}


void vulkan_pipeline_layout_push_constants(vulkan_pipeline_layout_t* layout, renderer_t* renderer, void* bytes)
{
	vkCmdPushConstants(ref(VkCommandBuffer, renderer->vk_command_buffers.value2, renderer->swapchain->current_image_index), layout->layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float) * 16, bytes);
}
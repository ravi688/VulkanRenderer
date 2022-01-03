
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>
#include <renderer/assert.h>
#include <renderer/debug.h>

vulkan_pipeline_layout_t* vulkan_pipeline_layout_new()
{
	vulkan_pipeline_layout_t* layout = heap_new(vulkan_pipeline_layout_t);
	memset(layout, 0, sizeof(vulkan_pipeline_layout_t));
	return layout;
}

void vulkan_pipeline_layout_create_no_alloc(renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info, vulkan_pipeline_layout_t* pipeline_layout)
{
	assert(create_info != NULL);
	VkPushConstantRange* push_constant_range = stack_new(VkPushConstantRange);
	/*TODO: push_constant_range->size = sizeof(mat4_t(float));*/
	push_constant_range->size = sizeof(float) * 16;
	push_constant_range->offset = 0;
	push_constant_range->stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	u32 set_layout_count = create_info->vk_set_layout == VK_NULL_HANDLE ? 0 : 1;
	pipeline_layout->handle = vk_get_pipeline_layout(renderer->vk_device, set_layout_count, &create_info->vk_set_layout, 1, push_constant_range);
	stack_free(push_constant_range);
}

vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info)
{
	vulkan_pipeline_layout_t* pipeline_layout = vulkan_pipeline_layout_new();
	vulkan_pipeline_layout_create_no_alloc(renderer, create_info, pipeline_layout);
	return pipeline_layout;
}


void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* pipeline_layout, renderer_t* renderer)
{
	vkDestroyPipelineLayout(renderer->vk_device, pipeline_layout->handle, NULL);
}

void vulkan_pipeline_layout_release_resources(vulkan_pipeline_layout_t* pipeline_layout)
{
	heap_free(pipeline_layout);
}


void vulkan_pipeline_layout_push_constants(vulkan_pipeline_layout_t* pipeline_layout, renderer_t* renderer, void* bytes)
{
	vkCmdPushConstants(ref(VkCommandBuffer, renderer->vk_command_buffers.value2, renderer->swapchain->current_image_index), pipeline_layout->handle, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float) * 16, bytes);
}
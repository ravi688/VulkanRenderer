
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>

vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(renderer_t* renderer)
{
	vulkan_pipeline_layout_t* layout = heap_new(vulkan_pipeline_layout_t);
	memset(layout, 0, sizeof(vulkan_pipeline_layout_t));
	layout->layout = vk_get_pipeline_layout(renderer->vk_device);
	return layout;
}


void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* layout, renderer_t* renderer)
{
	vkDestroyPipelineLayout(renderer->vk_device, layout->layout, NULL);
	heap_free(layout);
}

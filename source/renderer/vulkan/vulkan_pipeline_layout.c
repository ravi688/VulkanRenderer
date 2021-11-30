
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>

vulkan_pipeline_layout_t* vulkan_pipeline_layout_new()
{
	vulkan_pipeline_layout_t* layout = heap_new(vulkan_pipeline_layout_t);
	memset(layout, 0, sizeof(vulkan_pipeline_layout_t));
	return layout;
}

void vulkan_pipeline_layout_create_non_alloc(renderer_t* renderer, vulkan_pipeline_layout_t* layout)
{
	refp(vulkan_pipeline_layout_t, layout, 0)->layout = vk_get_pipeline_layout(renderer->vk_device);
}

vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(renderer_t* renderer)
{
	vulkan_pipeline_layout_t* layout = vulkan_pipeline_layout_new();
	vulkan_pipeline_layout_create_non_alloc(renderer, refp(vulkan_pipeline_layout_t, layout, 0));
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

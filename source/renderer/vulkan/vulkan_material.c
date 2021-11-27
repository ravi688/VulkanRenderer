
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>
#include <stdio.h>

vulkan_material_t* vulkan_material_create(renderer_t* renderer, vulkan_material_create_info_t* create_info)
{
	vulkan_material_t* material = heap_new(vulkan_material_t);
	memset(material, 0, sizeof(vulkan_material_t));
	vulkan_graphics_pipeline_create_info_t pipeline_create_info =
	{
		.shaders = create_info->shaders,
		.shader_count = create_info->shader_count,
		.vertex_info = create_info->vertex_info
	};
	material->pipeline = vulkan_graphics_pipeline_create(renderer, &pipeline_create_info);
	return material;
}

void vulkan_material_bind(vulkan_material_t* material, renderer_t* renderer)
{
	vulkan_graphics_pipeline_bind(material->pipeline, renderer);
}

void vulkan_material_destroy(vulkan_material_t* material, renderer_t* renderer)
{
	vulkan_graphics_pipeline_destroy(material->pipeline, renderer);
	heap_free(material);
}
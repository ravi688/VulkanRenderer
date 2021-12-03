
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/render_window.h>
#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>
#include <stdio.h>

static void recreate_material(render_window_t* window, void* user_data);

vulkan_material_t* vulkan_material_new()
{
	vulkan_material_t* material = heap_new(vulkan_material_t);
	memset(material, 0, sizeof(vulkan_material_t));
	material->pipeline = vulkan_graphics_pipeline_new();
	material->self_reference = material;
	return material;
}

void vulkan_material_create_no_alloc(renderer_t* renderer, vulkan_material_create_info_t* create_info, vulkan_material_t* material)
{
	ASSERT(renderer->window != NULL, "renderer->window == NULL\n");
	vulkan_graphics_pipeline_create_info_t pipeline_create_info =
	{
		.shaders = create_info->shaders,
		.shader_count = create_info->shader_count,
		.vertex_info_count = create_info->vertex_info_count,
		.vertex_infos = create_info->vertex_infos
	};
	vulkan_graphics_pipeline_create_no_alloc(renderer, &pipeline_create_info, material->pipeline);
	render_window_subscribe_on_resize(renderer->window, recreate_material, material);
}

vulkan_material_t* vulkan_material_create(renderer_t* renderer, vulkan_material_create_info_t* create_info)
{
	vulkan_material_t* material = vulkan_material_new();
	vulkan_material_create_no_alloc(renderer, create_info, material);
	material->create_info = *create_info;
	material->create_info.vertex_infos = heap_newv(vulkan_vertex_info_t, create_info->vertex_info_count);
	//TODO: This should be like safe_memcpy()
	memcpy(material->create_info.vertex_infos, create_info->vertex_infos, sizeof(vulkan_vertex_info_t) * create_info->vertex_info_count);
	for(u32 i = 0; i < create_info->vertex_info_count; i++)
	{
		vulkan_vertex_info_t* src_info = refp(vulkan_vertex_info_t, create_info->vertex_infos, i);
		vulkan_vertex_info_t* dst_info = refp(vulkan_vertex_info_t, material->create_info.vertex_infos, i);
		VkFormat* attribute_formats = heap_newv(VkFormat, src_info->attribute_count);
		u32* attribute_offsets = heap_newv(u32, src_info->attribute_count);
		//TODO: This should be like safe_memcpy()
		memcpy(attribute_formats, src_info->attribute_formats, sizeof(VkFormat) * src_info->attribute_count);
		memcpy(attribute_offsets, src_info->attribute_offsets, sizeof(u32) * src_info->attribute_count);
		dst_info->attribute_offsets = attribute_offsets;
		dst_info->attribute_formats = attribute_formats;
	}
	material->renderer = renderer;
	return material;
}


void vulkan_material_bind(vulkan_material_t* material, renderer_t* renderer)
{
	vulkan_graphics_pipeline_bind(material->pipeline, renderer);
}

void vulkan_material_destroy(vulkan_material_t* material, renderer_t* renderer)
{
	vulkan_graphics_pipeline_destroy(material->pipeline, renderer);
	render_window_unsubscribe_on_resize(renderer->window, recreate_material);
}

void vulkan_material_release_resources(vulkan_material_t* material)
{
	vulkan_graphics_pipeline_release_resources(material->pipeline);
	u32 vertex_info_count = material->create_info.vertex_info_count;
	for(u32 i = 0; i < vertex_info_count; i++)
	{
		vulkan_vertex_info_t* info = refp(vulkan_vertex_info_t, material->create_info.vertex_infos, i);
		heap_free(info->attribute_formats);
		heap_free(info->attribute_offsets);
	}
	heap_free(material->create_info.vertex_infos);
	heap_free(material);
}

static void recreate_material(render_window_t* window, void* user_data)
{
	vulkan_material_t* material = user_data;
	vulkan_material_destroy(material, material->renderer);
	vulkan_material_create_no_alloc(material->renderer, &(material->create_info), material);
}


#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
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
	material->graphics_pipeline = vulkan_graphics_pipeline_new();
	material->self_reference = material;
	return material;
}

void vulkan_material_create_no_alloc(renderer_t* renderer, vulkan_material_create_info_t* create_info, vulkan_material_t* material)
{
	ASSERT(renderer->window != NULL, "renderer->window == NULL\n");
	assert(create_info->vertex_info_count != 0);
	assert(create_info->vertex_infos != NULL);
	material->renderer = renderer;
	material->shader = create_info->shader;
	material->is_transparent = create_info->is_transparent;
	vulkan_graphics_pipeline_create_info_t pipeline_create_info =
	{
		.shader = create_info->shader,
		.vertex_info_count = create_info->vertex_info_count,
		.vertex_infos = create_info->vertex_infos,
		.blend_enabled = create_info->is_transparent
	};
	vulkan_graphics_pipeline_create_no_alloc(renderer, &pipeline_create_info, material->graphics_pipeline);
}

vulkan_material_t* vulkan_material_create(renderer_t* renderer, vulkan_material_create_info_t* create_info)
{
	vulkan_material_t* material = vulkan_material_new();
	vulkan_material_create_no_alloc(renderer, create_info, material);
	material->vertex_infos = heap_newv(vulkan_vertex_info_t, create_info->vertex_info_count);
	material->vertex_info_count = create_info->vertex_info_count;
	//TODO: This should be like safe_memcpy()
	memcpy(material->vertex_infos, create_info->vertex_infos, sizeof(vulkan_vertex_info_t) * create_info->vertex_info_count);
	for(u32 i = 0; i < create_info->vertex_info_count; i++)
	{
		vulkan_vertex_info_t* src_info = refp(vulkan_vertex_info_t, create_info->vertex_infos, i);
		vulkan_vertex_info_t* dst_info = refp(vulkan_vertex_info_t, material->vertex_infos, i);
		VkFormat* attribute_formats = heap_newv(VkFormat, src_info->attribute_count);
		u32* attribute_offsets = heap_newv(u32, src_info->attribute_count);
		//TODO: This should be like safe_memcpy()
		memcpy(attribute_formats, src_info->attribute_formats, sizeof(VkFormat) * src_info->attribute_count);
		memcpy(attribute_offsets, src_info->attribute_offsets, sizeof(u32) * src_info->attribute_count);
		dst_info->attribute_offsets = attribute_offsets;
		dst_info->attribute_formats = attribute_formats;
	}
	render_window_subscribe_on_resize(renderer->window, recreate_material, material);
	return material;
}

void vulkan_material_destroy(vulkan_material_t* material, renderer_t* renderer)
{
	vulkan_graphics_pipeline_destroy(material->graphics_pipeline, renderer);
	render_window_unsubscribe_on_resize(renderer->window, recreate_material);
	material->shader = NULL;
}

void vulkan_material_release_resources(vulkan_material_t* material)
{
	vulkan_graphics_pipeline_release_resources(material->graphics_pipeline);
	u32 vertex_info_count = material->vertex_info_count;
	for(u32 i = 0; i < vertex_info_count; i++)
	{
		vulkan_vertex_info_t* info = refp(vulkan_vertex_info_t, material->vertex_infos, i);
		heap_free(info->attribute_formats);
		heap_free(info->attribute_offsets);
	}
	heap_free(material->vertex_infos);
	heap_free(material);
}

void vulkan_material_bind(vulkan_material_t* material, renderer_t* renderer)
{
	u32 image_index = renderer->swapchain->current_image_index;
	vulkan_graphics_pipeline_bind(material->graphics_pipeline, renderer);
	if(material->shader->vk_set != NULL)
		vulkan_descriptor_set_bind(material->shader->vk_set, renderer, material->graphics_pipeline->pipeline_layout);
}

void vulkan_material_push_constants(vulkan_material_t* material, renderer_t* renderer, void* bytes)
{
	vulkan_pipeline_layout_push_constants(material->graphics_pipeline->pipeline_layout, renderer, bytes);
}

void vulkan_material_set_texture(vulkan_material_t* material, renderer_t* renderer, u32 binding_index, vulkan_texture_t* texture)
{
	if(material->shader->vk_set == NULL)
	{
		assert_wrn(material->shader->vk_set != NULL);
		return;
	}
	vulkan_descriptor_set_write_texture(material->shader->vk_set, renderer, binding_index, texture);
}

void vulkan_material_set_uniform_buffer(vulkan_material_t* material, renderer_t* renderer, u32 binding_index, vulkan_buffer_t* buffer)
{
	if(material->shader->vk_set == NULL)
	{
		assert_wrn(material->shader->vk_set != NULL);
		return;
	}
	vulkan_descriptor_set_write_uniform_buffer(material->shader->vk_set, renderer, binding_index, buffer);
}

static void recreate_material(render_window_t* window, void* user_data)
{
	vulkan_material_t* material = user_data;
	vulkan_graphics_pipeline_destroy(material->graphics_pipeline, material->renderer);
	vulkan_graphics_pipeline_create_info_t pipeline_create_info =
	{
		.shader = material->shader,
		.vertex_info_count = material->vertex_info_count,
		.vertex_infos = material->vertex_infos,
		.blend_enabled = material->is_transparent
	};
	vulkan_graphics_pipeline_create_no_alloc(material->renderer, &pipeline_create_info, material->graphics_pipeline);
}

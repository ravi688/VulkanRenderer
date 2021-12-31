
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
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
	// material->descriptor_set = vulkan_descriptor_set_new();
	material->self_reference = material;
	return material;
}

void vulkan_material_create_no_alloc(renderer_t* renderer, vulkan_material_create_info_t* create_info, vulkan_material_t* material)
{
	ASSERT(renderer->window != NULL, "renderer->window == NULL\n");
	assert(create_info->vertex_info_count != 0);
	assert(create_info->vertex_infos != NULL);
	material->renderer = renderer;
	vulkan_graphics_pipeline_create_info_t pipeline_create_info =
	{
		.shaders = create_info->shaders,
		.shader_count = create_info->shader_count,
		.vertex_info_count = create_info->vertex_info_count,
		.vertex_infos = create_info->vertex_infos,
		.bindings = create_info->bindings,
		.binding_count = create_info->binding_count
	};
	vulkan_graphics_pipeline_create_no_alloc(renderer, &pipeline_create_info, material->graphics_pipeline);

	if(create_info->binding_count != 0 && material->descriptor_set == NULL)
	{
		material->descriptor_set = vulkan_descriptor_set_new();
		vulkan_descriptor_set_create_info_t set_create_info =
		{
			.pool = renderer->vk_descriptor_pool,
			.layout = material->graphics_pipeline->pipeline_layout->descriptor_set_layout
		};
		vulkan_descriptor_set_create_no_alloc(renderer, &set_create_info, material->descriptor_set);
	}
}

vulkan_material_t* vulkan_material_create(renderer_t* renderer, vulkan_material_create_info_t* create_info)
{
	vulkan_material_t* material = vulkan_material_new();
	vulkan_material_create_no_alloc(renderer, create_info, material);
	material->create_info = *create_info;

	if(create_info->binding_count != 0)
	{
		//Create deap copy of bindings, it is used in recreation of pipeline layout when recreating the graphics pipeline
		material->create_info.bindings = heap_newv(VkDescriptorSetLayoutBinding, create_info->binding_count);
		//TODO: This should be like safe_memcpy()
		memcpy(material->create_info.bindings, create_info->bindings, sizeof(VkDescriptorSetLayoutBinding) * create_info->binding_count);
	}

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
	render_window_subscribe_on_resize(renderer->window, recreate_material, material);
	return material;
}

void vulkan_material_destroy(vulkan_material_t* material, renderer_t* renderer)
{
	if(material->descriptor_set != NULL)
		vulkan_descriptor_set_destroy(material->descriptor_set, renderer);
	vulkan_graphics_pipeline_destroy(material->graphics_pipeline, renderer);
	render_window_unsubscribe_on_resize(renderer->window, recreate_material);
}

void vulkan_material_release_resources(vulkan_material_t* material)
{
	if(material->descriptor_set != NULL)
		vulkan_descriptor_set_release_resources(material->descriptor_set);
	vulkan_graphics_pipeline_release_resources(material->graphics_pipeline);
	u32 vertex_info_count = material->create_info.vertex_info_count;
	for(u32 i = 0; i < vertex_info_count; i++)
	{
		vulkan_vertex_info_t* info = refp(vulkan_vertex_info_t, material->create_info.vertex_infos, i);
		heap_free(info->attribute_formats);
		heap_free(info->attribute_offsets);
	}
	heap_free(material->create_info.vertex_infos);
	if(material->create_info.bindings != NULL)
		heap_free(material->create_info.bindings);
	heap_free(material);
}

void vulkan_material_bind(vulkan_material_t* material, renderer_t* renderer)
{
	u32 image_index = renderer->swapchain->current_image_index;
	vulkan_graphics_pipeline_bind(material->graphics_pipeline, renderer);
	if(material->descriptor_set != NULL)
		vulkan_descriptor_set_bind(material->descriptor_set, renderer, material->graphics_pipeline->pipeline_layout);
}

void vulkan_material_push_constants(vulkan_material_t* material, renderer_t* renderer, void* bytes)
{
	vulkan_pipeline_layout_push_constants(material->graphics_pipeline->pipeline_layout, renderer, bytes);
}

void vulkan_material_set_texture(vulkan_material_t* material, renderer_t* renderer, u32 binding_index, vulkan_texture_t* texture)
{
	vulkan_descriptor_set_write_texture(material->descriptor_set, renderer, binding_index, texture);
}

void vulkan_material_set_uniform_buffer(vulkan_material_t* material, renderer_t* renderer, u32 binding_index, vulkan_buffer_t* buffer)
{
	vulkan_descriptor_set_write_uniform_buffer(material->descriptor_set, renderer, binding_index, buffer);
}

static void recreate_material(render_window_t* window, void* user_data)
{
	vulkan_material_t* material = user_data;
	vulkan_graphics_pipeline_destroy(material->graphics_pipeline, material->renderer);
	vulkan_material_create_no_alloc(material->renderer, &(material->create_info), material);
}

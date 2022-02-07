#include <renderer/internal/vulkan/vulkan_defines.h>
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

static void recreate_material(render_window_t* window, void* user_data);

RENDERER_API vulkan_material_t* vulkan_material_new()
{
	vulkan_material_t* material = heap_new(vulkan_material_t);
	memset(material, 0, sizeof(vulkan_material_t));
	material->graphics_pipeline = vulkan_graphics_pipeline_new();
	material->self_reference = material;
	return material;
}

RENDERER_API void vulkan_material_create_no_alloc(vulkan_renderer_t* renderer, vulkan_material_create_info_t* create_info, vulkan_material_t* material)
{
	assert(renderer->window != NULL);
	assert(create_info->vertex_info_count != 0);
	assert(create_info->vertex_infos != NULL);

	// cache some required data to use them when recreating this material when window resizes
	material->renderer = renderer;
	material->shader = create_info->shader;
	material->is_transparent = create_info->is_transparent;
	
	vulkan_graphics_pipeline_create_info_t pipeline_create_info =
	{
		.shader = create_info->shader,
		.vertex_info_count = create_info->vertex_info_count,
		.vertex_infos = create_info->vertex_infos,
		.push_constant_ranges = create_info->push_constant_ranges,
		.push_constant_range_count = create_info->push_constant_range_count,
		.blend_enabled = create_info->is_transparent
	};
	vulkan_graphics_pipeline_create_no_alloc(renderer, &pipeline_create_info, material->graphics_pipeline);
}

RENDERER_API vulkan_material_t* vulkan_material_create(vulkan_renderer_t* renderer, vulkan_material_create_info_t* create_info)
{
	// allocate memory and intitalize
	vulkan_material_t* material = vulkan_material_new();
	
	// create material
	vulkan_material_create_no_alloc(renderer, create_info, material);

	// cache push constant ranges to use them when recreating this material when window resizes
	material->push_constant_ranges = heap_newv(VkPushConstantRange, create_info->push_constant_range_count);
	material->push_constant_range_count = create_info->push_constant_range_count;
	memcpy(material->push_constant_ranges, create_info->push_constant_ranges, sizeof(VkPushConstantRange) * material->push_constant_range_count);
	
	// cache a deep copp vulkan vertex infos

	// cache vulkan vertex infos to use them when recreating this material when window resizes
	material->vertex_infos = heap_newv(vulkan_vertex_info_t, create_info->vertex_info_count);
	material->vertex_info_count = create_info->vertex_info_count;
	memcpy(material->vertex_infos, create_info->vertex_infos, sizeof(vulkan_vertex_info_t) * create_info->vertex_info_count);
	
	// cache vulkan vertex info's offset and formats to use them when recreating this material when window resizes
	for(u32 i = 0; i < create_info->vertex_info_count; i++)
	{
		vulkan_vertex_info_t* src_info = &create_info->vertex_infos[i];
		vulkan_vertex_info_t* dst_info = &material->vertex_infos[i];

		dst_info->attribute_formats = heap_newv(VkFormat, src_info->attribute_count);
		dst_info->attribute_offsets = heap_newv(u32, src_info->attribute_count);
		
		memcpy(dst_info->attribute_formats, src_info->attribute_formats, sizeof(VkFormat) * src_info->attribute_count);
		memcpy(dst_info->attribute_offsets, src_info->attribute_offsets, sizeof(u32) * src_info->attribute_count);
	}


	// subscribe to resize event to recreate this material
	render_window_subscribe_on_resize(renderer->window, recreate_material, material);
	
	return material;
}

RENDERER_API void vulkan_material_destroy(vulkan_material_t* material)
{
	// destroy the vulkan pipline object
	vulkan_graphics_pipeline_destroy(material->graphics_pipeline, material->renderer);

	// unsubscribe from resize vent	
	render_window_unsubscribe_on_resize(material->renderer->window, recreate_material);
}

RENDERER_API void vulkan_material_release_resources(vulkan_material_t* material)
{
	vulkan_graphics_pipeline_release_resources(material->graphics_pipeline);

	u32 vertex_info_count = material->vertex_info_count;
	for(u32 i = 0; i < vertex_info_count; i++)
	{
		vulkan_vertex_info_t* info = &material->vertex_infos[i];
		heap_free(info->attribute_formats);
		heap_free(info->attribute_offsets);
	}

	heap_free(material->vertex_infos);
	heap_free(material->push_constant_ranges);
	heap_free(material);
}

RENDERER_API void vulkan_material_bind(vulkan_material_t* material)
{
	u32 image_index = material->renderer->swapchain->current_image_index;

	// bind the graphics pipeline
	vulkan_graphics_pipeline_bind(material->graphics_pipeline, material->renderer);

	// bind descriptor set if it is not null
	if(material->shader->vk_set != NULL)
		vulkan_descriptor_set_bind(material->shader->vk_set, material->renderer, material->graphics_pipeline->pipeline_layout);
}

RENDERER_API void vulkan_material_push_constants(vulkan_material_t* material, void* bytes)
{
	vulkan_pipeline_layout_push_constants(material->graphics_pipeline->pipeline_layout, material->renderer, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, bytes);
}

RENDERER_API void vulkan_material_set_texture(vulkan_material_t* material, u32 binding_index, vulkan_texture_t* texture)
{
	if(material->shader->vk_set == NULL)
	{
		assert_wrn(material->shader->vk_set != NULL);
		return;
	}
	vulkan_descriptor_set_write_texture(material->shader->vk_set, material->renderer, binding_index, texture);
}

RENDERER_API void vulkan_material_set_uniform_buffer(vulkan_material_t* material, u32 binding_index, vulkan_buffer_t* buffer)
{
	if(material->shader->vk_set == NULL)
	{
		assert_wrn(material->shader->vk_set != NULL);
		return;
	}
	vulkan_descriptor_set_write_uniform_buffer(material->shader->vk_set, material->renderer, binding_index, buffer);
}

static void recreate_material(render_window_t* window, void* user_data)
{
	vulkan_material_t* material = user_data;

	// destroy the vulkan pipeline object
	vulkan_graphics_pipeline_destroy(material->graphics_pipeline, material->renderer);
	
	// create again
	vulkan_graphics_pipeline_create_info_t pipeline_create_info =
	{
		.shader = material->shader,
		.vertex_info_count = material->vertex_info_count,
		.vertex_infos = material->vertex_infos,
		.blend_enabled = material->is_transparent,
		.push_constant_ranges = material->push_constant_ranges,
		.push_constant_range_count = material->push_constant_range_count
	};
	
	vulkan_graphics_pipeline_create_no_alloc(material->renderer, &pipeline_create_info, material->graphics_pipeline);
}


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
	refp(vulkan_material_t, material, 0)->pipeline = vulkan_graphics_pipeline_new();
	refp(vulkan_material_t, material, 0)->self_reference = material;
	return material;
}

void vulkan_material_create_non_alloc(renderer_t* renderer, vulkan_material_create_info_t* create_info, vulkan_material_t* material)
{
	ASSERT(renderer->window != NULL, "renderer->window == NULL\n");
	vulkan_graphics_pipeline_create_info_t pipeline_create_info =
	{
		.shaders = create_info->shaders,
		.shader_count = create_info->shader_count,
		.vertex_info = create_info->vertex_info
	};
	vulkan_graphics_pipeline_create_non_alloc(renderer, &pipeline_create_info, refp(vulkan_graphics_pipeline_t, refp(vulkan_material_t, material, 0)->pipeline, 0));
	refp(vulkan_material_t, material, 0)->create_info = *create_info;
	refp(vulkan_material_t, material, 0)->renderer = renderer;
	render_window_subscribe_on_resize(renderer->window, recreate_material, refp(vulkan_material_t, material, 0));
}

vulkan_material_t* vulkan_material_create(renderer_t* renderer, vulkan_material_create_info_t* create_info)
{
	vulkan_material_t* material = vulkan_material_new();
	vulkan_material_create_non_alloc(renderer, create_info, refp(vulkan_material_t, material, 0));
	return material;
}


void vulkan_material_bind(vulkan_material_t* material, renderer_t* renderer)
{
	vulkan_graphics_pipeline_bind(refp(vulkan_graphics_pipeline_t, refp(vulkan_material_t, material, 0)->pipeline, 0), renderer);
}

void vulkan_material_destroy(vulkan_material_t* material, renderer_t* renderer)
{
	vulkan_graphics_pipeline_destroy(refp(vulkan_graphics_pipeline_t, refp(vulkan_material_t, material, 0)->pipeline, 0), renderer);
	render_window_unsubscribe_on_resize(renderer->window, recreate_material);
}

void vulkan_material_release_resources(vulkan_material_t* material)
{
	vulkan_graphics_pipeline_release_resources(refp(vulkan_graphics_pipeline_t, refp(vulkan_material_t, material, 0)->pipeline, 0));
	heap_free(material);
}

static void recreate_material(render_window_t* window, void* user_data)
{
	vulkan_material_t* material = user_data;
	vulkan_material_destroy(refp(vulkan_material_t, material, 0), refp(vulkan_material_t, material, 0)->renderer);
	vulkan_material_create_non_alloc(refp(vulkan_material_t, material, 0)->renderer, &(refp(vulkan_material_t, material, 0)->create_info), material);
}

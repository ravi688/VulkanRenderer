
#include <renderer/internal/vulkan/vulkan_render_object.h>

#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>

#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

typedef struct vulkan_render_object_t
{
	vulkan_material_t* material;
	void* user_data;
	draw_call_handler_t draw;
	vulkan_descriptor_set_t* set;
} vulkan_render_object_t;

RENDERER_API vulkan_render_object_t* vulkan_render_object_new()
{
	vulkan_render_object_t* obj = heap_new(vulkan_render_object_t);
	memset(obj, 0, sizeof(vulkan_render_object_t));
	return obj;
}

RENDERER_API vulkan_render_object_t* vulkan_render_object_create(vulkan_render_object_create_info_t* create_info)
{
	vulkan_render_object_t* obj = vulkan_render_object_new();
	obj->material = create_info->material;
	obj->user_data = create_info->user_data;
	obj->draw = create_info->draw_handler;
	obj->set = create_info->set;
	return obj;
}

RENDERER_API void vulkan_render_object_destroy(vulkan_render_object_t* obj)
{
	// TODO
}

RENDERER_API void vulkan_render_object_release_resources(vulkan_render_object_t* obj)
{
	heap_free(obj);
}

RENDERER_API void vulkan_render_object_draw(vulkan_render_object_t* obj)
{
	obj->draw(obj->user_data);
}

RENDERER_API void vulkan_render_object_bind_resource(vulkan_render_object_t* obj, vulkan_pipeline_layout_t* pipeline_layout)
{
	vulkan_descriptor_set_bind(obj->set, VULKAN_DESCRIPTOR_SET_OBJECT, pipeline_layout);
}

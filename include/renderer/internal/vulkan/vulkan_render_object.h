
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct vulkan_material_t vulkan_material_t;
typedef struct vulkan_descriptor_set_t vulkan_descriptor_set_t;
typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;
typedef void (*draw_call_handler_t)(void* object);

typedef struct vulkan_render_object_create_info_t
{
	vulkan_material_t* material;
	void* user_data;
	draw_call_handler_t draw_handler;
	vulkan_descriptor_set_t* set;
} vulkan_render_object_create_info_t;

typedef struct vulkan_render_object_t vulkan_render_object_t;


BEGIN_CPP_COMPATIBLE

/* constructors & destructurs */
RENDERER_API vulkan_render_object_t* vulkan_render_object_new();
RENDERER_API vulkan_render_object_t* vulkan_render_object_create(vulkan_render_object_create_info_t* create_info);
RENDERER_API void vulkan_render_object_destroy(vulkan_render_object_t* obj);
RENDERER_API void vulkan_render_object_release_resources(vulkan_render_object_t* obj);

RENDERER_API void vulkan_render_object_draw(vulkan_render_object_t* obj);
RENDERER_API void vulkan_render_object_bind_resource(vulkan_render_object_t* obj, vulkan_pipeline_layout_t* pipeline_layout);

END_CPP_COMPATIBLE


#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

#include <renderer/internal/vulkan/vulkan_descriptor_set.h> 	// for vulkan_descriptor_set_t
#include <renderer/internal/vulkan/vulkan_buffer.h> 			// for vulkan_buffer_t
#include <renderer/internal/vulkan/vulkan_handles.h> 			// for vulkan_render_object_handle_t
#include <renderer/struct_descriptor.h> 						// for struct_descriptor_t


#include <hpml/memory/header_config.h>
#include <hpml/memory/memory.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

typedef struct vulkan_material_t vulkan_material_t;

// typedef struct vulkan_object_info_t
// {
// 	mat4_t(float) transform;
// 	mat4_t(float) normal;
// } vulkan_object_info_t;

typedef enum vulkan_render_object_type_t
{
	VULKAN_RENDER_OBJECT_TYPE_MESH = 1,
	VULKAN_RENDER_OBJECT_TYPE_TEXT_MESH = 2,
	VULKAN_RENDER_OBJECT_TYPE_TEXT = 3,
	VULKAN_RENDER_OBJECT_TYPE_CAMERA = 4,
	VULKAN_RENDER_OBJECT_TYPE_LIGHT = 5
} vulkan_render_object_type_t;

typedef void (*draw_call_handler_t)(void* object);

typedef struct vulkan_render_object_create_info_t
{
	vulkan_material_t* material; 		// optional
	void* user_data; 					// optional
	draw_call_handler_t draw_handler; 	// optional if type is valid
	vulkan_render_object_type_t type;	// optional if draw_handler is valid
} vulkan_render_object_create_info_t;

typedef struct vulkan_render_queue_t vulkan_render_queue_t;

typedef struct vulkan_render_object_t
{
	vulkan_renderer_t* renderer;

	vulkan_render_queue_t* queue;			 // ptr to the queue in which this object is
	vulkan_render_object_handle_t handle;	 // handle to this object in the render queue

	vulkan_material_t* material;
	void* user_data;
	draw_call_handler_t draw;
	vulkan_descriptor_set_t object_set;
	vulkan_buffer_t buffer;

	// TODO: Since this is identical for all the render objects, 
	// 		it would be part of vulkan_renderer_t [Global Data] to save some memory
	struct_descriptor_t struct_definition;

	struct_field_handle_t transform_handle;
	struct_field_handle_t normal_handle;
} vulkan_render_object_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructurs */
RENDERER_API vulkan_render_object_t* vulkan_render_object_new();
RENDERER_API vulkan_render_object_t* vulkan_render_object_create(vulkan_renderer_t* renderer, vulkan_render_object_create_info_t* create_info);
RENDERER_API void vulkan_render_object_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_object_create_info_t* create_info, vulkan_render_object_t OUT object);
RENDERER_API void vulkan_render_object_destroy(vulkan_render_object_t* object);
RENDERER_API void vulkan_render_object_release_resources(vulkan_render_object_t* object);

RENDERER_API void vulkan_render_object_attach(vulkan_render_object_t* object, void* user_data);
RENDERER_API void vulkan_render_object_draw(vulkan_render_object_t* object);
RENDERER_API void vulkan_render_object_set_material(vulkan_render_object_t* object, vulkan_material_t* material);
RENDERER_API vulkan_material_t* vulkan_render_object_get_material(vulkan_render_object_t* object);
RENDERER_API void vulkan_render_object_set_transform(vulkan_render_object_t* object, mat4_t(float) transform);
RENDERER_API mat4_t(float) vulkan_render_object_get_transform(vulkan_render_object_t* object);
RENDERER_API mat4_t(float) vulkan_render_object_get_normal(vulkan_render_object_t* object);

END_CPP_COMPATIBLE

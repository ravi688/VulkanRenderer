
#include <renderer/internal/vulkan/vulkan_render_object.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_render_queue.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

typedef struct vulkan_mesh_t vulkan_mesh_t;
RENDERER_API void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh);

typedef struct text_mesh_t text_mesh_t;
RENDERER_API void text_mesh_draw(text_mesh_t* text);


RENDERER_API vulkan_render_object_t* vulkan_render_object_new()
{
	vulkan_render_object_t* obj = heap_new(vulkan_render_object_t);
	memzero(obj, vulkan_render_object_t);
	return obj;
}

static void setup_gpu_resources(vulkan_render_object_t* object)
{
	// create descriptor set
	vulkan_descriptor_set_create_info_t set_create_info =
	{
		.vo_pool = object->renderer->vo_descriptor_pool,
		.layout = &object->renderer->object_set_layout
	};
	vulkan_descriptor_set_create_no_alloc(object->renderer, &set_create_info, &object->object_set);

	// setup object struct definiton
	struct_descriptor_begin(&object->struct_definition, "objectInfo", GLSL_TYPE_UNIFORM_BUFFER);
		struct_descriptor_add_field(&object->struct_definition, "transform", GLSL_TYPE_MAT4);
		struct_descriptor_add_field(&object->struct_definition, "normal", GLSL_TYPE_MAT4);
	struct_descriptor_end(&object->struct_definition);

	// create uniform buffers and write to the descriptor set OBJECT_SET at binding TRANSFORM_BINDING
	vulkan_buffer_create_info_t create_info = 
	{
		.size = struct_descriptor_sizeof(&object->struct_definition),
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.vo_sharing_mode = object->renderer->vo_sharing_mode,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	assert(create_info.size == (64 * 2));
	vulkan_buffer_create_no_alloc(object->renderer, &create_info, &object->buffer);
	struct_descriptor_map(&object->struct_definition, vulkan_buffer_map(&object->buffer));
	vulkan_descriptor_set_write_uniform_buffer(&object->object_set, VULKAN_DESCRIPTOR_BINDING_TRANSFORM, &object->buffer);

	object->transform_handle = struct_descriptor_get_field_handle(&object->struct_definition, "transform");
	object->normal_handle = struct_descriptor_get_field_handle(&object->struct_definition, "normal");
}

RENDERER_API vulkan_render_object_t* vulkan_render_object_create(vulkan_renderer_t* renderer, vulkan_render_object_create_info_t* create_info)
{
	vulkan_render_object_t* object = vulkan_render_object_new();
	vulkan_render_object_create_no_alloc(renderer, create_info, object);
	return object;
}
RENDERER_API void vulkan_render_object_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_object_create_info_t* create_info, vulkan_render_object_t OUT object)
{
	memzero(object, vulkan_render_object_t);

	object->renderer = renderer;
	object->handle = VULKAN_RENDER_OBJECT_HANDLE_INVALID;
	object->material = create_info->material;
	object->user_data = create_info->user_data;

	switch(create_info->type)
	{
		case VULKAN_RENDER_OBJECT_TYPE_MESH:
			object->draw = CAST_TO(draw_call_handler_t, vulkan_mesh_draw_indexed);
			break;
		case VULKAN_RENDER_OBJECT_TYPE_TEXT_MESH:
			object->draw = CAST_TO(draw_call_handler_t, text_mesh_draw);
			break;
		case VULKAN_RENDER_OBJECT_TYPE_TEXT: 	// TODO
			LOG_FETAL_ERR("TEXT render object are yet to be defined\n");
			break;
		default:
			if(create_info->draw_handler == NULL)
				LOG_FETAL_ERR("Unsupported vulkan render object type %u and draw_handler is also NULL\n", create_info->type);
			object->draw = create_info->draw_handler;
	}

	setup_gpu_resources(object);
	
	vulkan_render_object_set_transform(object, mat4_identity());
}

RENDERER_API void vulkan_render_object_destroy(vulkan_render_object_t* obj)
{
	if((obj->queue != NULL) && (obj->handle != VULKAN_RENDER_OBJECT_HANDLE_INVALID))
		vulkan_render_queue_removeH(obj->queue, obj->handle);
	vulkan_descriptor_set_destroy(&obj->object_set);
	struct_descriptor_unmap(&obj->struct_definition);
	vulkan_buffer_unmap(&obj->buffer);
	vulkan_buffer_destroy(&obj->buffer);
}

RENDERER_API void vulkan_render_object_release_resources(vulkan_render_object_t* obj)
{
	vulkan_descriptor_set_release_resources(&obj->object_set);
	vulkan_buffer_release_resources(&obj->buffer);
	// TODO
	// heap_free(obj);
}

RENDERER_API void vulkan_render_object_attach(vulkan_render_object_t* obj, void* user_data)
{
	obj->user_data = user_data;
}

RENDERER_API void vulkan_render_object_draw(vulkan_render_object_t* obj)
{
	obj->draw(obj->user_data);
}

RENDERER_API void vulkan_render_object_set_material(vulkan_render_object_t* obj, vulkan_material_t* material)
{
	if(obj->material == material) return;
	if((obj->queue != NULL) && (obj->handle != VULKAN_RENDER_OBJECT_HANDLE_INVALID))
		vulkan_render_queue_removeH(obj->queue, obj->handle);
	obj->material = material;
	vulkan_render_object_handle_t handle = vulkan_render_queue_add(obj->queue, obj);
	assert(handle != VULKAN_RENDER_OBJECT_HANDLE_INVALID);
}

RENDERER_API vulkan_material_t* vulkan_render_object_get_material(vulkan_render_object_t* obj)
{
	return obj->material;
}

RENDERER_API void vulkan_render_object_set_transform(vulkan_render_object_t* obj, mat4_t transform)
{
	mat4_t normal = mat4_inverse(transform);
	mat4_move(transform, mat4_transpose(transform));
	struct_descriptor_set_mat4(&obj->struct_definition, obj->transform_handle, CAST_TO(float*, &transform));
	struct_descriptor_set_mat4(&obj->struct_definition, obj->normal_handle, CAST_TO(float*, &normal));
}

RENDERER_API mat4_t vulkan_render_object_get_transform(vulkan_render_object_t* obj)
{
	mat4_t transform;
	struct_descriptor_get_mat4(&obj->struct_definition, obj->transform_handle, CAST_TO(float*, &transform));
	return transform;
}

RENDERER_API mat4_t vulkan_render_object_get_normal(vulkan_render_object_t* obj)
{
	mat4_t normal;
	struct_descriptor_get_mat4(&obj->struct_definition, obj->normal_handle, CAST_TO(float*, &normal));
	return normal;
}

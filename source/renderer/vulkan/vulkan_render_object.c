
#include <renderer/internal/vulkan/vulkan_render_object.h>

#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

RENDERER_API vulkan_render_object_t* vulkan_render_object_new()
{
	vulkan_render_object_t* obj = heap_new(vulkan_render_object_t);
	memset(obj, 0, sizeof(vulkan_render_object_t));
	return obj;
}

static void setup_gpu_resources(vulkan_render_object_t* object)
{
	// create descriptor set
	vulkan_descriptor_set_create_info_t set_create_info =
	{
		.pool = object->renderer->descriptor_pool,
		.layout = object->renderer->object_set_layout
	}
	vulkan_descriptor_set_create_no_alloc(object->renderer, &set_create_info, &object->object_set);

	// setup object struct definiton
	strcpy(object->struct_definition.name, "objectInfo");
	struct_field_t* fields = heap_newv(struct_field_t, 2);
	memset(fields, 0, sizeof(struct_field_t) * 2);
	strcpy(fields[0].name, "transform"); 		// transform matrix
	strcpy(fields[1].name, "normal");			// normal matrix
	for(int i = 0; i < 2; i++)
	{
		fields[i].type = STRUCT_FIELD_MAT4;
		fields[i].alignment = 16;
		fields[i].size = sizeof(float) * 16;
	}
	object->struct_definition.fields = fields;
	object->struct_definition.field_count = 2;
	struct_descriptor_recalculate(&object->struct_definition);

	// create uniform buffers and write to the descriptor set OBJECT_SET at binding TRANSFORM_BINDING
	vulkan_buffer_create_info_t create_info = 
	{
		.size = struct_descriptor_sizeof(&object->struct_definition),
		.usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.sharing_mode = object->renderer->sharing_mode,
		.memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	}
	assert(create_info.size == (64 * 2));
	vulkan_buffer_create_no_alloc(object->renderer, &object->buffer);
	struct_descriptor_map(&object->buffer, vulkan_buffer_map(&object->buffer));
	vulkan_descriptor_set_write_uniform_buffer(&object->object_set, object->renderer, VULKAN_DESCRIPTOR_SET_OBJECT_TRANSFORM, &object->buffer);

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
	obj->renderer = renderer;
	obj->material = create_info->material;
	obj->user_data = create_info->user_data;
	obj->draw = create_info->draw_handler;

	setup_gpu_resources(obj);
	
	vulkan_render_object_set_transform(obj, mat4_identity(float)());
}

RENDERER_API void vulkan_render_object_destroy(vulkan_render_object_t* obj)
{
	vulkan_descriptor_set_destroy(&obj->object_set, obj->renderer);
	struct_descriptor_unmap(&obj->struct_definition);
	vulkan_buffer_unmap(&obj->buffer);
	vulkan_buffer_destroy(&obj->buffer);
}

RENDERER_API void vulkan_render_object_release_resources(vulkan_render_object_t* obj)
{
	vulkan_descriptor_set_release_resources(&obj->object_set);
	vulkan_buffer_release_resources(&obj->buffer);
	heap_free(obj);
}

RENDERER_API void vulkan_render_object_draw(vulkan_render_object_t* obj)
{
	obj->draw(obj->user_data);
}

RENDERER_API void vulkan_render_object_set_material(vulkan_render_object_t* obj, vulkan_material_t* material)
{
	obj->material;
}

RENDERER_API vulkan_material_t* vulkan_render_object_get_material(vulkan_render_object_t* obj)
{
	return obj->material;
}

RENDERER_API void vulkan_render_object_set_transform(vulkan_render_object_t* obj, mat4_t(float) transform)
{
	mat4_t(float) normal = mat4_transpose(float)(mat4_inverse(float)(transform));
	struct_descriptor_set_mat4(&obj->struct_definition, obj->transform_handle, &transform);
	struct_descriptor_set_mat4(&obj->struct_definition, obj->normal_handle, &normal);
}

RENDERER_API mat4_t(float) vulkan_render_object_get_transform(vulkan_render_object_t* obj)
{
	mat4_t(float) transform;
	struct_descriptor_get_mat4(&obj->struct_definition, obj->transform_handle, &transform);
	return transform;
}

RENDERER_API mat4_t(float) vulkan_render_object_get_normal(vulkan_render_object_t* obj)
{
	mat4_t(float) normal;
	struct_descriptor_get_mat4(&obj->struct_definition, obj->normal_handle, &normal);
	return normal;
}
